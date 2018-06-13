#ifndef RGBSensor_h
#define RGBSensor_h

#include "Arduino.h"

class RGBSensor {
  int sensor_pin;
  int RGBleds_pins[3];
  int state; // NO_BLOCK
  int last_read; // guarda a posição da array que deve ser atualizada na próxima leitura
  int last_reads[40]; // guarda as últimas 10 leituras do sensor para cada cor.. R G B e OFF
  int white_rgb[3];
  int black_rgb[3];
  byte led_on; // qual led está aceso
  void load_calibration();
  void store_calibration();
  
  public:
    const static byte RED = 0;
    const static byte GREEN = 1;
    const static byte BLUE = 2;
    const static byte OFF = 3; // quando o led está apagado
    const static byte BLACK = 4;
    const static byte WHITE = 5;
    const static byte YELLOW = 6;
    const static byte NO_COLOR = 7;
    
    RGBSensor(int sensor_pin, int RGBleds_pins[]); // construtor
    ~RGBSensor(); // destructor
    void turn_led_on(byte color);
    void turn_leds_off();
    int get_value(byte color);
    int get_value(byte color, byte raw);
    byte identify_color();
    int read();
    void readRGB();
    void calibrate_white();
    void calibrate_black();
};

#endif
