#include "LineSensor.h"
#include <EEPROM.h>

LineSensor::LineSensor(int pin) : GenericSensor(pin, 0) {
  this->state = this->ON_LINE;
  this->persistent_address_line = this->pin;
  this->persistent_address_offline = this->persistent_address_line + 60;
  this->load_values();
  this->last_read = 0;
  for(int i=0; i<10; i++) {
    this->read();
  }
}

int LineSensor::status() {
  int mean = this->get_value();
  if(this->state == this->ON_LINE) {
    if(mean < this->offline_threshold) {
      this->state = OFF_LINE;
    }
  } else {
    if(mean > this->line_threshold) {
      this->state = ON_LINE;
    }
  }
  return this->state;
}

void LineSensor::store_values() {
  char text[250];
  EEPROM.write(this->persistent_address_line, this->line_threshold / 4);
  EEPROM.write(this->persistent_address_offline, this->offline_threshold / 4);
  sprintf(text, "Stored - Line: %d Offline: %d", this->line_threshold, this->offline_threshold);
  Serial.println(text);
}

void LineSensor::load_values() {
  char text[250];
  int line = EEPROM.read(this->persistent_address_line) * 4;
  int offline = EEPROM.read(this->persistent_address_offline) * 4;
  sprintf(text, "Loaded - Line: %d Offline: %d", line, offline);
  Serial.println(text);
  this->line_threshold = line;
  this->offline_threshold = offline;
}

void LineSensor::calibrate(int line, int offline) {
  int diff = line - offline;
  this->line_threshold = line - (diff / 3);
  this->offline_threshold = offline + (diff / 3);
  this->store_values();
}
