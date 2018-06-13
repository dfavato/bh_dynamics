int const static pins[] = {8, 9, 10, 11, 15, 14, 13, 12, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53};

int get_pin_type (int pin) {
  int const static digital[] {0, 0, 0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1};  
  return digital[pin];
}

void sensor_loop() {
  int pin = 0;
  int button;
  char text[250];
  GenericSensor *sensor = NULL;

  do {
    do {
      button = interface.get_pressed_button();
      if(!sensor) {
        sensor = new GenericSensor(pins[pin], get_pin_type(pin));
        sprintf(text, "Port:%d", pins[pin]);
        interface.set_lcd_text(text, 1);
        interface.refresh_lcd(TRUE);
      }
      sensor->read();
      sprintf(text, "Value:%d", sensor->get_value());
      interface.set_lcd_text(text, 2);
      interface.refresh_lcd();
    } while (button == NO_BUTTON);
    interface.button_release();
    if(button == RIGHT_BUTTON || button == LEFT_BUTTON) {
      delete sensor;
      sensor = NULL;
      if (button == RIGHT_BUTTON) {
        pin++;
        if(pin >= (sizeof(pins) / sizeof(int)))
          pin = 0;
      } else {
        pin--;
        if(pin < 0)
          pin = (sizeof(pins) / sizeof(int)) - 1;
      }
    }
  } while (button == RIGHT_BUTTON || button == LEFT_BUTTON);
}

