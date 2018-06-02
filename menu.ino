int buttons_threshold[] = {49, 175, 331, 523, 831};

int get_pressed_button() {
  int value;
  int pressed_button = NO_BUTTON;
  
  value = analogRead(0);
  for (int i = RIGHT; i < NO_BUTTON; i++) {
    if (value < buttons_threshold[i]) {
      pressed_button = i;
      break;  
    }
  }
  return pressed_button;
}

void button_release() {
  int button;
  do {
    button = get_pressed_button();
  } while(button != NO_BUTTON);
}

void wait_for_button(int button) {
  int value;
  int pressed_button;

  do {
    pressed_button = get_pressed_button();
  } while (pressed_button != button); 
}
