#include "Arduino.h"
#include "Interface.h"

#define TRUE 1
#define FALSE 0

Interface::Interface() {
  this->lcd = new LiquidCrystal(8,9,4,5,6,7);
  this->lcd->begin(16,2);
  this->last_lcd_update = millis();
  this->lcd_refresh_interval = 200;
  sprintf(this->lcd_text1, "BH Dynamics");
  sprintf(this->lcd_text2, "");
}

int Interface::get_pressed_button() {
  int value;
  int pressed_button = NO_BUTTON; 
  value = analogRead(0);
  for (int i = RIGHT_BUTTON; i < NO_BUTTON; i++) {
    if (value < this->buttons_threshold[i]) {
      pressed_button = i;
      break;  
    }
  }
  return pressed_button;
}

void Interface::button_release() {
  int button;
  delay(50);
  do {
    button = this->get_pressed_button();
  } while(button != NO_BUTTON);
}

void Interface::wait_for_button(int button) {
  int value;
  int pressed_button;

  do {
    pressed_button = get_pressed_button();
  } while (pressed_button != button); 
}

void Interface::set_lcd_text(char *text1, char *text2) {
  this->set_lcd_text(text1, 1);
  this->set_lcd_text(text2, 2);
}

void Interface::set_lcd_text(char *text, int n) {
  switch(n % 2) {
    case 0:
      sprintf(this->lcd_text2, "%s                 ", text);
      break;
    case 1:
      sprintf(this->lcd_text1, "%s                 ", text);
      break;
  }
  this->refresh_lcd(FALSE);
}

void Interface::refresh_lcd(int now) {
  unsigned long current_time = millis();
  if(now || current_time > this->last_lcd_update + this->lcd_refresh_interval) {
    this->lcd->setCursor(0,0);
    this->lcd->print(this->lcd_text1);
    this->lcd->setCursor(0,1);
    this->lcd->print(this->lcd_text2);
    this->last_lcd_update = current_time;
  }
}

void Interface::refresh_lcd() {
  this->refresh_lcd(FALSE);
}

