#define INITIAL_STATE 0
#define LOOKING_FOR_BLOCK 1
#define RESCUING 2
#define LOST 3

int robot_state = INITIAL_STATE;

void competition_loop() {
  unsigned long start;
  int side;
  digitalWrite(block_led, HIGH);
  wait_for_start();
  start = millis();
  side = align_with_light();
  set_initial_pose(side);
  
  control_unit->go(FORWARD);
  do {
    update_sensors();
  } while(!over_the_line());
  control_unit->stop();

  if(line_sensor_l->status() == line_sensor_l->ON_LINE) {
   control_unit->go(FORWARD);
   do {
    update_sensors();
   } while(over_the_line());
   control_unit->spin(control_unit->LEFT);
   do {
    update_sensors();
   } while(!over_the_line());
  }

  do {
    update_sensors();
    follow_the_line();
    if(block_captured()) {
      handle_block();
      break;
    }
  } while(millis() <= start + 60000);
  control_unit->stop();
}

void set_initial_pose(int side) {
  control_unit->set_pose(0,0,0);
}

void wait_for_start() {
  interface.set_lcd_text("Competicao", "");
  interface.refresh_lcd(TRUE);
  while(start_light_off()) {
    interface.set_lcd_text("Competicao", "Aguardando luz...");
    interface.refresh_lcd();
    update_sensors();
  }
  interface.set_lcd_text("Vamo time!", "");
  interface.refresh_lcd(TRUE);
}

void handle_block() {
  byte color = rgb_sensor->identify_color();
  switch (color) {
    case rgb_sensor->RED:
      take_block_to_base();
      break;
    case rgb_sensor->GREEN:
      take_block_to_base();
      break;
    case rgb_sensor->BLUE:
      take_block_to_base();
      break;
    case rgb_sensor->YELLOW:
      break;
    case rgb_sensor->BLACK:
      reject_block();
      break;
    case rgb_sensor->WHITE:
      break;
    case rgb_sensor->NO_COLOR:
      break;
  } 
}

void take_block_to_base() {
  int state;
  do {
    state = control_unit->spin_degrees(control_unit->RIGHT, 90);
    update_sensors();
  } while (state != control_unit->TARGET_REACHED);
  do {
    state = control_unit->drive_straight(70);
    update_sensors();
  } while(state != control_unit->TARGET_REACHED);
}

void reject_block() {
  int state;
  do {
    state = control_unit->spin_degrees(control_unit->LEFT, 90);
    update_sensors();
  } while (state != control_unit->TARGET_REACHED);
  do {
    state = control_unit->drive_straight(15);
    update_sensors();
  } while(state != control_unit->TARGET_REACHED);
  control_unit->go(BACKWARD);
  do {
    update_sensors();
  } while(!over_the_line());
}

bool start_light_off() {
  return start_sensor->read();
}

bool over_the_line() {
  return (line_sensor_l->status() == line_sensor_l->ON_LINE) || (line_sensor_r->status() == line_sensor_r->ON_LINE);
}

void update_sensors() {
  control_unit->update_encoders();
  line_sensor_r->read();
  line_sensor_l->read();
}

void follow_the_line() {
  int l_status = line_sensor_l->status();
  int r_status = line_sensor_r->status();
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
      if(robot_state != LOST) {
        print_position();
        robot_state = LOST;
      }
      decide_where_to_go();
    }
  }
}

void print_position() {};

void decide_where_to_go() {
  control_unit->spin(control_unit->RIGHT);
}


