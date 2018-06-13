int block_captured() {
  int no_object;
  digitalWrite(block_led, HIGH);
  no_object = block_sensor->read();
  if(no_object) {
    return FALSE;
  }
  return TRUE;
}

void color_loop() {
  int button;
  byte color;
  
  do {
    button = interface.get_pressed_button();
    if(block_captured()) {
      print_color_sensor_status();  
    } else {
      interface.set_lcd_text("Posicione o bloco", "");
      interface.refresh_lcd(FALSE);
    }
  } while (button == NO_BUTTON);
  interface.button_release();
  if(button == RIGHT_BUTTON) {
    calibrate_rgb_sensor();
  }
}

void print_color_sensor_status() {
  // Printa os valores dos sensores
  char text1[100];
  char text2[100];
  byte color = rgb_sensor->identify_color();
  int r_value = rgb_sensor->get_value(rgb_sensor->RED);
  int g_value = rgb_sensor->get_value(rgb_sensor->GREEN);
  int b_value = rgb_sensor->get_value(rgb_sensor->BLUE);
  switch (color) {
    case rgb_sensor->RED:
      sprintf(text1, "Vermelho");
      break;
    case rgb_sensor->GREEN:
      sprintf(text1, "Verde");
      break;
    case rgb_sensor->BLUE:
      sprintf(text1, "Azul");
      break;
    case rgb_sensor->YELLOW:
      sprintf(text1, "Amarelo");
      break;
    case rgb_sensor->BLACK:
      sprintf(text1, "Preto");
      break;
    case rgb_sensor->WHITE:
      sprintf(text1, "Branco");
      break;
    case rgb_sensor->NO_COLOR:
      sprintf(text1, "Posicione o bloco");
      break;
  }
  sprintf(text2, "R%d G%d B%d", r_value, g_value, b_value);
  interface.set_lcd_text(text1, text2);
  interface.refresh_lcd(FALSE);
}

void print_color_sensor_values() {
  // Printa os valores dos sensores
  char text1[100];
  char text2[100];
  rgb_sensor->readRGB();
  int r_value = rgb_sensor->get_value(rgb_sensor->RED);
  int g_value = rgb_sensor->get_value(rgb_sensor->GREEN);
  int b_value = rgb_sensor->get_value(rgb_sensor->BLUE);
  sprintf(text2, "R%d G%d B%d", r_value, g_value, b_value);
  interface.set_lcd_text(text2, 2);
  interface.refresh_lcd(FALSE);
}

void print_color_sensor_calibration() {
  // Printa os valores dos sensores
  char text1[100];
  char text2[100];
  rgb_sensor->readRGB();
  int r_value = rgb_sensor->get_value(rgb_sensor->RED, 1);
  int g_value = rgb_sensor->get_value(rgb_sensor->GREEN, 1);
  int b_value = rgb_sensor->get_value(rgb_sensor->BLUE, 1);
  sprintf(text2, "R%d G%d B%d", r_value, g_value, b_value);
  interface.set_lcd_text(text2, 2);
  interface.refresh_lcd(FALSE);
}

void calibrate_rgb_sensor() {
  // menu de calibragem
  char text1[50];
  char text2[50];
  interface.set_lcd_text("Ler branco", "");
  interface.refresh_lcd(TRUE);
  do {
  } while (!block_captured());
  delay(1000);
  rgb_sensor->calibrate_white();

  interface.set_lcd_text("Retire o branco", "");
  interface.refresh_lcd(TRUE);
  do {
  } while(block_captured());

  interface.set_lcd_text("Ler preto", "");
  interface.refresh_lcd(TRUE);
  do {
  } while(!block_captured());
  delay(1000);
  rgb_sensor->calibrate_black();
}

