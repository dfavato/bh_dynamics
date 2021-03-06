#define INITIAL_STATE 0
#define LOOKING_FOR_BLOCK 1
#define RESCUING 2
#define LOST 3
#define REJECTING_BLOCK 4

int block_count;
int rejected_blocks;
int robot_state = INITIAL_STATE;
int side_to_turn;
unsigned long start;
bool recovered_from_trap;

void competition_loop() {
  int state;
  char text[50];
  
  
  wait_for_start();
  start = millis();
  align_with_light();
  
  recovered_from_trap = FALSE;
  side_to_turn = control_unit->RIGHT;
  set_initial_pose();
  block_count = 0;
  rejected_blocks = 0;
  robot_state = INITIAL_STATE;

  do {
    state = control_unit->drive_straight(10);
    update_sensors();
  } while (state != control_unit->TARGET_REACHED && !time_over());
  control_unit->go(FORWARD);
  do {
    update_sensors();
  } while(!over_the_line() && !time_over());
  control_unit->stop();

  if(line_sensor_l->status() == line_sensor_l->ON_LINE) {
   control_unit->go(FORWARD);
   do {
    update_sensors();
   } while(over_the_line() && !time_over());
   control_unit->curve(control_unit->LEFT);
   do {
    update_sensors();
   } while(!over_the_line() && !time_over());
  }
  digitalWrite(block_led, HIGH);
  do {
    update_sensors();
    follow_the_line();
    sprintf(text, "Blocos: %d", block_count);
    interface.set_lcd_text("Vamo time!", text);
    interface.refresh_lcd();
    if(block_captured()) {
      handle_block();
      if(block_count > 3) break;
    }
  } while(!time_over());
  control_unit->stop();
  interface.set_lcd_text("FIM!", 1);
  interface.refresh_lcd(TRUE);
  digitalWrite(block_led, LOW);
  delay(5000);
}

void set_initial_pose() {
  control_unit->set_pose(0.0,0.0,0.0);
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
  byte color;
  unsigned long timer = millis();

  control_unit->go(FORWARD);
  do {
    update_sensors();
  } while(millis() < timer + 500);
  control_unit->stop();
  if(block_captured()) {
    color = rgb_sensor->identify_color();
  } else {
    return;
  }
  
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
      take_block_to_base();
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

bool time_over() {
  return millis() > start + 60000;
}

void take_block_to_base() {
  int state;
  robot_state = RESCUING;
  interface.set_lcd_text("Resgatando!", "");
  interface.refresh_lcd(TRUE);
  double angle;

  recovered_from_trap = TRUE;
  block_count++;
  switch(block_count) {
    case 1:
      do {
        state = control_unit->spin_degrees(control_unit->RIGHT, 110);
        update_sensors();
      } while (state != control_unit->TARGET_REACHED && !time_over());
      do {
        state = control_unit->drive_straight(55);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      control_unit->go(BACKWARD);
      do {
          state = control_unit->drive_straight(40, BACKWARD);
          update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      do {
          state = control_unit->spin_degrees(control_unit->RIGHT, 180);
          update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      control_unit->go(FORWARD);
      while(!over_the_line() && !time_over());
      break;
    case 2:
      do {
        state = control_unit->spin_degrees(control_unit->RIGHT, 100);
        update_sensors();
      } while (state != control_unit->TARGET_REACHED && !time_over());
      do {
        state = control_unit->drive_straight(70);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      do {
        state = control_unit->drive_straight(60, BACKWARD);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      do {
        state = control_unit->spin_degrees(control_unit->RIGHT, 180);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      control_unit->go(FORWARD);
      do {
        update_sensors();
      } while(!over_the_line() && !time_over());
      side_to_turn = control_unit->LEFT;
      break;
    case 3:
      do {
        state = control_unit->spin_degrees(control_unit->RIGHT, 270);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      do {
        state = control_unit->drive_straight(65);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      control_unit->go(BACKWARD);
      do {
          state = control_unit->drive_straight(50, BACKWARD);
          update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      do {
          state = control_unit->spin_degrees(control_unit->RIGHT, 180);
          update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      control_unit->go(FORWARD);
      do {
        update_sensors();
      } while(!over_the_line() && !time_over());
      break;
    case 4:
      if(rejected_blocks >= 2) {
        angle = 250;
      } else {
        angle = 290;
      }
      do {
        state = control_unit->spin_degrees(control_unit->RIGHT, angle);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      do {
        state = control_unit->drive_straight(65);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      break;
  }
}

void reject_block() {
  int state;
  robot_state = REJECTING_BLOCK;

  recovered_from_trap = TRUE;
  rejected_blocks++;
  block_count++;
  switch (block_count) {
    case 1:
      do {
        state = control_unit->spin_degrees(control_unit->RIGHT, 270);
        update_sensors();
      } while (state != control_unit->TARGET_REACHED && !time_over());
      do {
        state = control_unit->drive_straight(15);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      control_unit->go(BACKWARD);
      do {
        update_sensors();
      } while(!over_the_line() && !time_over());
      break;
    case 2:
      do {
        state = control_unit->drive_straight(15, BACKWARD);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      do {
        state = control_unit->spin_degrees(control_unit->LEFT, 150);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      control_unit->spin(control_unit->LEFT);
      do {
        update_sensors();
      } while(!over_the_line() && !time_over());
      side_to_turn = control_unit->LEFT;
      break;
    case 3:
      do {
        state = control_unit->spin_degrees(control_unit->RIGHT, 90);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      do {
        state = control_unit->drive_straight(15);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED && !time_over());
      control_unit->go(BACKWARD);
      do {
        update_sensors();
      } while(!over_the_line() && !time_over());
      do {
        update_sensors();
      } while(over_the_line() && !time_over());
      control_unit->go(FORWARD);
      do {
        update_sensors();
      } while(!over_the_line() && !time_over());
      control_unit->spin(control_unit->RIGHT);
      do {
        update_sensors();
      } while(!over_the_line(TRUE) && !time_over());
      break;
    case 4:
      break;
  }
}

bool start_light_off() {
  return start_sensor->read();
}

bool over_the_line() {
  return (line_sensor_l->status() == line_sensor_l->ON_LINE) || (line_sensor_r->status() == line_sensor_r->ON_LINE);
}

bool over_the_line(bool both) {
  return (line_sensor_l->status() == line_sensor_l->ON_LINE) && (line_sensor_r->status() == line_sensor_r->ON_LINE);
}

void update_sensors() {
  control_unit->update_encoders();
  line_sensor_r->read();
  line_sensor_l->read();
  update_light_sensor();
}

void follow_the_line() {
  int l_status = line_sensor_l->status();
  int r_status = line_sensor_r->status();
  if(r_status == line_sensor_r->ON_LINE) {
    if(robot_state == LOST) {
      robot_state = LOOKING_FOR_BLOCK;
    }
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
      if(robot_state == LOST) {
        robot_state == LOOKING_FOR_BLOCK;
      }
      control_unit->curve(control_unit->LEFT);
    } else {
      // ambos os sensores estão fora da linha
      if(robot_state != LOST) {
//        pause();
        robot_state = LOST;
      }
      decide_where_to_go();
    }
  }
}

void print_position() { 
};

void decide_where_to_go() {
  int x = (int)control_unit->get_pose_x();
  int y = (int)control_unit->get_pose_y();
  int theta = (int)control_unit->get_pose_theta();
  if((x < 30 && y < 0 && (theta > -120 && theta < -60)) && !recovered_from_trap) {
    recover_from_trap();
    return;
  }
  control_unit->spin(side_to_turn);
  do {
    update_sensors();
  } while(!over_the_line(TRUE) && !time_over());
  do {
    update_sensors();
  } while(over_the_line(TRUE) && !time_over());
}

void recover_from_trap() {
  int state;
  do {
    state = control_unit->spin_degrees(control_unit->LEFT, 150);
    update_sensors();
  } while(state != control_unit->TARGET_REACHED && !time_over());
  do {
    state = control_unit->drive_straight(20);
    update_sensors();
  } while(state != control_unit->TARGET_REACHED && !time_over());
  control_unit->go(FORWARD);
  do {
    update_sensors();
  } while(!over_the_line() && !time_over());
  do {
    update_sensors();
  } while(over_the_line() && !time_over());
  control_unit->go(BACKWARD);
  do {
    update_sensors();
  } while(!over_the_line() && !time_over());
  recovered_from_trap = TRUE;
}

