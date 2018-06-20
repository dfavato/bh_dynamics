void control_loop() {
  int button;
  int state;
  interface.set_lcd_text("Odometria e controle", 1);
  interface.refresh_lcd(TRUE);
  do {
    button = interface.get_pressed_button();
    control_unit->update_encoders();
    print_encoder();
  } while (button == NO_BUTTON);
  interface.button_release();
  if(button == SELECT_BUTTON) {
    interface.set_lcd_text("Running...", 1);
    interface.refresh_lcd(TRUE);
    delay(500);
    control_unit->set_pose(0,0,0);
//    do_l();
//    test_line_follow_with_control();
    do_line(60);
//    do_triangle(30);
//    do_square(30);
    control_unit->stop();
    do {
      update_sensors();
      print_encoder();
    } while (interface.get_pressed_button() != SELECT_BUTTON);
    interface.button_release();
  }
}

void do_l() {
  int state;
  do {
    state = control_unit->drive_straight(50);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  do {
    state = control_unit->spin_degrees(control_unit->LEFT, 90);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
}

void test_line_follow_with_control() {
  int r_status;
  int l_status;
  control_unit->set_pose(0,0,0);
  for(int i=0; i<20; i++) {
    update_sensors();
  }
  
  do {
    control_unit->go(FORWARD);
    update_sensors();
    print_encoder();
  } while(!over_the_line());

  do {
    update_sensors();
    print_encoder();
    r_status = line_sensor_r->status();
    l_status = line_sensor_l->status();
    if(r_status == line_sensor_r->ON_LINE) {
      if(l_status == line_sensor_l->ON_LINE) {
        // os dois sensores estão sobre a linha
        control_unit->go(FORWARD);
      } else {
        // sensor da esquerda fora da linha
        control_unit->curve(control_unit->RIGHT);
      }
    } else {
      if(l_status == line_sensor_l->ON_LINE) {
        // sensor da direita fora da linha
        control_unit->curve(control_unit->LEFT);
      } else {
        // ambos os sensores estão fora da linha
        control_unit->spin(control_unit->LEFT);
      }
    }
  } while(over_the_line());
  pause();

  control_unit->spin(control_unit->LEFT);
  do {
    update_sensors();
    print_encoder();
  } while(!over_the_line());
  pause();

  control_unit->curve(control_unit->LEFT);
  do {
    update_sensors();
    print_encoder();
  } while (line_sensor_r->status() == line_sensor_r->OFF_LINE);
}

void pause() {
  unsigned long wait = 5000;
  unsigned long now;
  control_unit->stop();
  now = millis();
  do {
    update_sensors();
    print_encoder();
  } while (now + wait > millis());
}

void do_curve () {
  control_unit->curve(control_unit->LEFT);
  double theta;
  do {
    control_unit->update_encoders();
    theta = control_unit->get_pose_theta();
    print_encoder();
  } while(theta < 360);
  control_unit->stop();
}

void print_encoder() {
  char text[50];
  char text2[50];
  int theta, x, y;
  long l_ticks, r_ticks;
  x = (int)control_unit->get_pose_x();
  y = (int)control_unit->get_pose_y();
  theta = (int)control_unit->get_pose_theta();
  l_ticks = control_unit->get_encoder_position(control_unit->LEFT);
  r_ticks = control_unit->get_encoder_position(control_unit->RIGHT);
  sprintf(text, "x:%d y:%d t:%d", x, y, theta);
  sprintf(text2, "L:%d R:%d", l_ticks, r_ticks);
  interface.set_lcd_text(text, text2);
  interface.refresh_lcd();
}

void do_triangle(int side) {
  int state;
  do {
    state = control_unit->drive_straight(side);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  do {
    state = control_unit->spin_degrees(control_unit->RIGHT, 90);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  do {
    state = control_unit->drive_straight(side);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  do {
    state = control_unit->spin_degrees(control_unit->RIGHT, 135);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  do {
    state = control_unit->drive_straight((int)((float)1.414213562 * (float)side));
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  do {
    state = control_unit->spin_degrees(control_unit->RIGHT, 135);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
}

void do_line(int size) {
  int state;
  do {
    state = control_unit->drive_straight(size);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  pause();
  do {
    state = control_unit->spin_degrees(control_unit->LEFT, 180);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  pause();
  do {
    state = control_unit->drive_straight(size);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  pause();
  do {
    state = control_unit->spin_degrees(control_unit->LEFT, 180);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  pause();
}

void do_square(int side) {
  int state;
  for(int i = 0; i<4; i++) {
    do {
      state = control_unit->drive_straight(side);
      print_encoder();
    } while (state != control_unit->TARGET_REACHED);
    do {
      state = control_unit->spin_degrees(control_unit->RIGHT, 90);
      print_encoder();
    } while (state != control_unit->TARGET_REACHED); 
  } 
}

