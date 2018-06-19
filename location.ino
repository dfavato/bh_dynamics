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

byte align_with_light() {
  int max_value = 0;
  int tolerance = 10;
  int current_value;
  int state;
  do {
    state = control_unit->spin_degrees(control_unit->RIGHT, 450);
    update_sensors();
    current_value = abs(light_sensor_r->get_value() - light_sensor_l->get_value());
    if(current_value > max_value) max_value = current_value;
  } while (state != control_unit->TARGET_REACHED);
  control_unit->set_pose_theta(0);
  control_unit->spin(control_unit->LEFT);
  do {
    update_sensors();
    current_value = abs(light_sensor_r->get_value() - light_sensor_l->get_value());
    if(control_unit->get_pose_theta() > 360) {
      control_unit->set_pose_theta(0);
      tolerance += 10;
    }
  } while(current_value < max_value - tolerance);
  control_unit->stop();
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

