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
    do_line(60);
    do_triangle(30);
    do_square(30);
    do {
    } while (interface.get_pressed_button() != SELECT_BUTTON);
    interface.button_release();
  }
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
  print_encoder();
  delay(5000);
  do {
    state = control_unit->spin_degrees(control_unit->LEFT, 180);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  print_encoder();
  delay(5000);
  do {
    state = control_unit->drive_straight(size);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  print_encoder();
  delay(5000);
  do {
    state = control_unit->spin_degrees(control_unit->LEFT, 180);
    print_encoder();
  } while (state != control_unit->TARGET_REACHED);
  print_encoder();
  delay(5000);
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

