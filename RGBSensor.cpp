#include "RGBSensor.h"
#include <EEPROM.h>

RGBSensor::RGBSensor(int sensor_pin, int RGBleds_pins[]) {
  this->sensor_pin = sensor_pin;
  for(byte i = this->RED; i<=BLUE; i++) {
    this->RGBleds_pins[i] = RGBleds_pins[i];
    pinMode(this->RGBleds_pins[i], OUTPUT);
    this->turn_led_on(i);
    delay(100);
    for(int i=0; i<10; i++) {
      this->read();
    }
  }
  this->load_calibration();
  this->turn_leds_off();
  delay(100);
  for(int i=0; i<10; i++) {
    this->read();
  }
  turn_leds_off();
}

void RGBSensor::turn_leds_off() {
  for(byte i = this->RED; i<=BLUE; i++) {
    digitalWrite(this->RGBleds_pins[i], HIGH);
  }
  this->led_on = this->OFF;
}

void RGBSensor::turn_led_on(byte color) {
  this->turn_leds_off();
  if(color >= this->RED && color <= this->BLUE) {
    digitalWrite(this->RGBleds_pins[color], LOW);
    this->led_on = color;
  } else {
    this->led_on = this->OFF;
  }
}

void RGBSensor::readRGB() {
  this->turn_leds_off();
  for(byte i = this->RED; i<=BLUE; i++) {
    this->turn_led_on(i);
    delay(100);
    for(int i=0; i<10; i++) {
      this->read();
    }
  }
  this->turn_leds_off();
}

int RGBSensor::read() {
  // o vetor last_reads está didivido da seguinte maneira:
  //+-------+---------+--------+-------+
  //|  RED  |  GREEN  |  BLUE  |  OFF  |
  //+-------+---------+--------+-------+
  //0       10        20       30      40
  this->last_reads[this->last_read + (this->led_on*10)] = analogRead(this->sensor_pin);
  this->last_read = (this->last_read + 1) % 10;
}

int RGBSensor::get_value(byte color) {
  return this->get_value(color, 0);
}

int RGBSensor::get_value(byte color, byte raw) {
  // if raw returns raw value
  int avg = 0;
  for(int i = (color * 10); i < (color + 1) * 10; i++) {
    avg += this->last_reads[i];
  }
  avg = avg / 10;
  if(raw) {
    return avg;
  }
  return (int)((float)(avg - this->black_rgb[color])) / ((float)(this->white_rgb[color] - this->black_rgb[color])) * 255;
}

void RGBSensor::calibrate_white() {
  this->readRGB();
  this->white_rgb[this->RED] = this->get_value(this->RED, 1);
  this->white_rgb[this->GREEN] = this->get_value(this->GREEN, 1);
  this->white_rgb[this->BLUE] = this->get_value(this->BLUE, 1);
  this->store_calibration();
}

void RGBSensor::calibrate_black() {
  this->readRGB();
  this->black_rgb[this->RED] = this->get_value(this->RED, 1);
  this->black_rgb[this->GREEN] = this->get_value(this->GREEN, 1);
  this->black_rgb[this->BLUE] = this->get_value(this->BLUE, 1);
  this->store_calibration();
}

void RGBSensor::store_calibration() {
  EEPROM.write(this->RGBleds_pins[this->RED], this->black_rgb[this->RED] / 4);
  EEPROM.write(this->RGBleds_pins[this->GREEN], this->black_rgb[this->GREEN] / 4);
  EEPROM.write(this->RGBleds_pins[this->BLUE], this->black_rgb[this->BLUE] / 4);
  EEPROM.write(this->RGBleds_pins[this->RED] + 1, this->white_rgb[this->RED] / 4);
  EEPROM.write(this->RGBleds_pins[this->GREEN] + 1, this->white_rgb[this->GREEN] / 4);
  EEPROM.write(this->RGBleds_pins[this->BLUE] + 1, this->white_rgb[this->BLUE] / 4);
  Serial.print("Stored - White RGB(");
  Serial.print(this->white_rgb[this->RED], DEC);
  Serial.print(", ");
  Serial.print(this->white_rgb[this->GREEN], DEC);
  Serial.print(", ");
  Serial.print(this->white_rgb[this->BLUE], DEC);
  Serial.print(") Black RGB(");
  Serial.print(this->black_rgb[this->RED], DEC);
  Serial.print(", ");
  Serial.print(this->black_rgb[this->GREEN], DEC);
  Serial.print(", ");
  Serial.print(this->black_rgb[this->BLUE], DEC);
  Serial.println(")");
}

void RGBSensor::load_calibration() {
  this->black_rgb[this->RED] = EEPROM.read(this->RGBleds_pins[this->RED]) * 4;
  this->black_rgb[this->GREEN] = EEPROM.read(this->RGBleds_pins[this->GREEN]) * 4;
  this->black_rgb[this->BLUE] = EEPROM.read(this->RGBleds_pins[this->BLUE]) * 4;
  this->white_rgb[this->RED] = EEPROM.read(this->RGBleds_pins[this->RED] + 1) * 4;
  this->white_rgb[this->GREEN] = EEPROM.read(this->RGBleds_pins[this->GREEN] + 1) * 4;
  this->white_rgb[this->BLUE] = EEPROM.read(this->RGBleds_pins[this->BLUE] + 1) * 4;
  Serial.print("Loaded - White RGB(");
  Serial.print(this->white_rgb[this->RED], DEC);
  Serial.print(", ");
  Serial.print(this->white_rgb[this->GREEN], DEC);
  Serial.print(", ");
  Serial.print(this->white_rgb[this->BLUE], DEC);
  Serial.print(") Black RGB(");
  Serial.print(this->black_rgb[this->RED], DEC);
  Serial.print(", ");
  Serial.print(this->black_rgb[this->GREEN], DEC);
  Serial.print(", ");
  Serial.print(this->black_rgb[this->BLUE], DEC);
  Serial.println(")");
}

byte RGBSensor::identify_color() {
  int r, g, b;
  int max_value, min_value;
  byte max_color;
  
  this->readRGB();
  r = this->get_value(this->RED);
  g = this->get_value(this->GREEN);
  b = this->get_value(this->BLUE);

  if(r>255) r = 255;
  if(g>255) g = 255;
  if(b>255) b = 255;
  if(r<0) r = 0;
  if(g<0) g = 0;
  if(b<0) b = 0;

  if(r > g) {
    max_value = r;
    max_color = this->RED;
    min_value = g;
  } else {
    max_value = g;
    max_color = this->GREEN;
    min_value = r;
  }
  if(b > max_value) {
    max_value = b;
    max_color = this->BLUE;
  } else if(b < min_value) {
    min_value = b;
  }

  if(max_value == 0) return this->BLACK;

  if((1 - (float)min_value / (float)max_value) < 0.5) {
    // escala de cinza
    if(max_value > 255/2) {
      // é claro, então é branco
      return this->WHITE;
    } else {
      // é escuro, então preto
      return this->BLACK;
    }
  } else {
    // não é uma escala de cinza
    if(max_color == this->BLUE) {
      if(max_value < 30) return this->BLACK;
      return this->BLUE;
    }
    if(max_color == this->RED) {
      if(max_value < 50) return this->BLACK;
      // vermelho ou amarelo
      if(((float) g / (float) r) > 0.5) {
        return this->YELLOW;
      } else {
        return this->RED;
      }
    }
    if(max_color == this->GREEN) {
      if(max_value < 30) return this->BLACK;
      // verde ou amarelo
      if(((float) r / (float) g) > 0.5) {
        return this->YELLOW;
      } else {
        return this->GREEN;
      }
    }
  }
  return this->NO_COLOR;
}

RGBSensor::~RGBSensor() {
  this->turn_leds_off();
}


