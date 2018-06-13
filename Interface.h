#ifndef Interface_h
#define Interface_h

#include <LiquidCrystal.h>

#define RIGHT_BUTTON 0
#define UP_BUTTON 1
#define DOWN_BUTTON 2
#define LEFT_BUTTON 3
#define SELECT_BUTTON 4
#define NO_BUTTON 5

class Interface {
  unsigned long last_lcd_update;
  unsigned long lcd_refresh_interval;
  const static int buttons_threshold[5] = {49, 175, 331, 523, 831};
  char lcd_text1[255];
  char lcd_text2[255];
  
  public:
    Interface();
    LiquidCrystal *lcd;
    void refresh_lcd(int now);
    void refresh_lcd();
    void set_lcd_text(char *text1, char *text2);
    void set_lcd_text(char *text, int n);
    int get_pressed_button();
    void button_release();
    void wait_for_button(int button);
};

#endif
