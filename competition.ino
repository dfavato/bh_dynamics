#define INITIAL_STATE 0
#define LOOKING_FOR_BLOCK 1
#define RESCUING 2
#define LOST 3
#define REJECTING_BLOCK 4

int block_count = 0;
int robot_state = INITIAL_STATE;

void competition_loop() {
  unsigned long start;
  int side;
  int state;
  wait_for_start();
  start = millis();
  side = align_with_light2();
  set_initial_pose(side);

  do {
    state = control_unit->drive_straight(10);
  } while (state != control_unit->TARGET_REACHED);
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
   control_unit->curve(control_unit->LEFT);
   do {
    update_sensors();
   } while(!over_the_line());
  }
  digitalWrite(block_led, HIGH);
  do {
    update_sensors();
    follow_the_line_four();
    if(block_captured()) {
      handle_block();
      if(block_count > 1) break;
    }
  } while(millis() <= start + 60000);
  control_unit->stop();
}

void set_initial_pose(int side) {
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


void follow_the_line_four(){
  int ll_status = line_sensor_ll->status();
  int rr_status = line_sensor_rr->status();
  int l_status = line_sensor_l->status();
  int r_status = line_sensor_r->status(); 
  if(rr_status == line_sensor_rr->ON_LINE) {
    if(ll_status == line_sensor_ll->ON_LINE) {
      // os dois sensores extremos estão sobre a linha
      decide_where_to_go();
    } else {
      // sensor da esquerda fora da linha
      //control_unit->curve(control_unit->RIGHT);
      //curve_right() - curva até o sernsor_r ou senror_l on_line
      int state;
        do {
      state = control_unit->spin_degrees(control_unit->RIGHT, 100);
      //print_encoder();
    } while (state != control_unit->TARGET_REACHED);
      do {
      //update line sensor
      control_unit->curve(control_unit->LEFT);
    } while (l_status != line_sensor_l->ON_LINE && r_status != line_sensor_r->ON_LINE);
      
    }
  } else {
    if(ll_status == line_sensor_ll->ON_LINE) {
      // sensor da direita fora da linha
      //control_unit->curve(control_unit->LEFT);
       int state;
        do {
      state = control_unit->spin_degrees(control_unit->LEFT, 100);
      //print_encoder();
    } while (state != control_unit->TARGET_REACHED);
      do {
      //update line sensor
      control_unit->curve(control_unit->RIGHT);
    } while (l_status != line_sensor_l->ON_LINE && r_status != line_sensor_r->ON_LINE);
      
    }
      // ambos os sensores estão fora da linha    
  }
  follow_line()
}


void handle_block() {
  byte color = rgb_sensor->identify_color();
  block_count++;
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
  robot_state = RESCUING;
  interface.set_lcd_text("Resgatando!", "");
  interface.refresh_lcd(TRUE);

  switch(block_count) {
    case 1:
      do {
        state = control_unit->spin_degrees(control_unit->RIGHT, 90);
        update_sensors();
      } while (state != control_unit->TARGET_REACHED);
      do {
        state = control_unit->drive_straight(70);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED);
      control_unit->go(BACKWARD);
      do {
          state = control_unit->drive_straight(50, BACKWARD);
          update_sensors();
      } while(state != control_unit->TARGET_REACHED);
      do {
          state = control_unit->spin_degrees(control_unit->RIGHT, 180);
          update_sensors();
      } while(state != control_unit->TARGET_REACHED);
      control_unit->go(FORWARD);
      while(!over_the_line());
      break;
    case 2:
      do {
        state = control_unit->spin_degrees(control_unit->RIGHT, 90);
        update_sensors();
      } while (state != control_unit->TARGET_REACHED);
      do {
        state = control_unit->drive_straight(70);
        update_sensors();
      } while(state != control_unit->TARGET_REACHED);
      break;
  }
  
}

void reject_block() {
  int state;
  robot_state = REJECTING_BLOCK;

  switch (block_count) {
    case 1:
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
      break;
    case 2:
      break;
  }
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
        print_position();
        robot_state = LOST;
      }
      decide_where_to_go();
    }
  }
}

void print_position() { 
};

void decide_where_to_go() {
//  int state;
//  int x, y, theta;
//  x = (int)control_unit->get_pose_x();
//  y = (int)control_unit->get_pose_y();
//  theta = (int)control_unit->get_pose_theta();

  control_unit->spin(control_unit->RIGHT);  

//  if(x < 35 && y < -20) {
//    do {
//      state = control_unit->spin_degrees(control_unit->LEFT, 90);
//    } while (state != control_unit->TARGET_REACHED);
//    do {
//      state = control_unit->drive_straight(10);
//    } while (state != control_unit->TARGET_REACHED);
//    do {
//      state = control_unit->spin_degrees(control_unit->LEFT, 90);
//    } while (state != control_unit->TARGET_REACHED);
//    control_unit->go(FORWARD);
//    while(!over_the_line());
//  } else {
//    control_unit->spin(control_unit->RIGHT);  
//  }
}


