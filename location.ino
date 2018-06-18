void location_loop() {
  int button;

  interface.set_lcd_text("Localizacao", 1);
  interface.refresh_lcd(TRUE);
  do {
    button = interface.get_pressed_button();
    update_light_sensor();
    print_light_sensor();
  } while (button == NO_BUTTON);
  interface.button_release();
  if(button == SELECT_BUTTON) {
    interface.set_lcd_text("Running...", 1);
    interface.refresh_lcd(TRUE);
    delay(1000);
    align_with_light();
  }
  interface.button_release();
}

void collect_data() {
  control_unit->spin(control_unit->LEFT);
  Serial.println("left\tright");
  while(1) {
    update_light_sensor();
    Serial.print(light_sensor_l->get_value(), DEC);
    Serial.print("\t");
    Serial.println(light_sensor_r->get_value(), DEC);
  }
}

byte align_with_light2() {
  int max_value = 0;
  int tolerance = 10;
  int current_value;
  int state;
  do {
    state = control_unit->spin_degrees(control_unit->RIGHT, 360);
    update_sensors();
    current_value = abs(light_sensor_r->get_value() - light_sensor_l->get_value());
    if(current_value > max_value) max_value = current_value;
  } while (state != control_unit->TARGET_REACHED);
  control_unit->spin(control_unit->LEFT);
  do {
    update_sensors();
    current_value = abs(light_sensor_r->get_value() - light_sensor_l->get_value());
  } while(current_value < max_value - tolerance);
}

byte align_with_light() {
  byte side;
  int max_value;
  int diff;
  int last_value;
  int r_value, l_value;
  int tolerance = 3;
  int decrease_counter = 0;
  int increase_counter = 0;
  int increasing = FALSE;
  unsigned long read_interval = 100;
  unsigned long last_read;

  Serial.print("max");
  Serial.print("\t");
  Serial.print("last");
  Serial.print("\t");
  Serial.print("value");
  Serial.print("\t");
  Serial.print("dec.");
  Serial.print("\t");
  Serial.println("inc.");

  side = search_light();
  
  r_value = light_sensor_r->get_value();
  l_value = light_sensor_l->get_value();
  max_value = abs(r_value - l_value);
  last_value = max_value;
  control_unit->spin(control_unit->RIGHT, MIN_POWER);

  last_read = millis();
  do {
    update_light_sensor();
    if(millis() > last_read + read_interval) {
      r_value = light_sensor_r->get_value();
      l_value = light_sensor_l->get_value();
      if(side == control_unit->RIGHT) {
        diff = l_value - r_value;
      } else {
        diff = r_value - l_value;
      }
      last_read = millis();
      print_light_sensor();
      
      Serial.print(max_value, DEC);
      Serial.print("\t");
      Serial.print(last_value, DEC);
      Serial.print("\t");
      Serial.print(diff, DEC);
      if(diff > last_value + tolerance) {
        increasing = TRUE; // aproximando da luz
        increase_counter++;
        last_value = diff;
      } else if (diff < last_value - tolerance) {
        //afastando da luz
        if(increasing && increase_counter >= 0) {
          break;
        }
        decrease_counter++;
        last_value = diff;
      }
      Serial.print("\t");
      Serial.print(decrease_counter, DEC);
      Serial.print("\t");
      Serial.println(increase_counter, DEC);
      if(decrease_counter > 10 || diff < 200) {
        control_unit->spin(control_unit->LEFT, MIN_POWER);
        increase_counter -= decrease_counter;
        decrease_counter = 0;
      }
      if(diff > max_value) {
        max_value = diff;
      }
    }
  } while(1);
  Serial.println();
  control_unit->stop();
  return side;
}

byte search_light() {
  int l_value, r_value;
  control_unit->spin(control_unit->RIGHT);
  do {
    update_light_sensor();
    print_light_sensor();
    l_value = light_sensor_l->get_value();
    r_value = light_sensor_r->get_value();
  } while(abs(l_value - r_value) < 230);
  control_unit->stop();
  if(l_value < r_value) {
    return control_unit->LEFT;
  } else {
    return control_unit->RIGHT;
  }
}

void update_light_sensor() {
  light_sensor_r->read();
  light_sensor_l->read();
}

void print_light_sensor() {
    // Printa os valores dos sensores
  char text1[20];
  char text2[20];
  char text[100];
  int r_value = light_sensor_r->get_value();
  int l_value = light_sensor_l->get_value();
  sprintf(text1, "R:%d", r_value);
  sprintf(text2, "L:%d", l_value);
  sprintf(text, "%s %s  ", text1, text2);
  interface.set_lcd_text(text, 2);
  interface.refresh_lcd(FALSE);
}

