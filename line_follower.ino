void line_follower_loop() {
  int button;

  interface.set_lcd_text("Segue linha", "");
  interface.refresh_lcd(TRUE);
  do {
    button = interface.get_pressed_button();
    update_line_sensor();
    print_line_sensor_status();
  } while (button == NO_BUTTON);
  interface.button_release();
  if(button == SELECT_BUTTON) {
    interface.set_lcd_text("Running...", "");
    delay(500);
    line_follower();
  } else if (button == RIGHT_BUTTON) {
    calibrate_line_sensors();
  }
}

void line_follower() {
  // Rotina de seguir a linha
  // se o sensor da esquerda deixar de ver a linha > curva para direita
  // se o sensor da direita deixar de ver a linha > curva para esquerda
  // se os dois estiverem lendo a linha, anda reto
  // se nenhum estiver lendo a linha, depende...
  int r_status, l_status;
  int estado = 0; // 0 = parado, 1 = pra frente, 2 = curva direita, 3 = curva esquerda
  char text[50];
  while(1) {
    update_line_sensor();
    print_line_sensor_status();
    r_status = line_sensor_r->status();
    l_status = line_sensor_l->status();
    if(r_status == line_sensor_r->ON_LINE) {
      if(l_status == line_sensor_l->ON_LINE) {
        // os dois sensores estão sobre a linha
        control_unit->go(FORWARD);
        estado = 1;
        interface.set_lcd_text("Na linha", 1);
      } else {
        // sensor da esquerda fora da linha
        control_unit->curve(control_unit->RIGHT);
        estado = 2;
        interface.set_lcd_text("Linha a direita", 1);
      }
    } else {
      if(l_status == line_sensor_l->ON_LINE) {
        // sensor da direita fora da linha
        control_unit->curve(control_unit->LEFT);
        estado = 3;
        interface.set_lcd_text("Linha a esquerda", 1);
      } else {
        // ambos os sensores estão fora da linha
        // se ele está fazendo curva deve continuar
        switch (estado) {
          case 0:
            // o robo começou parado fora da linha
            control_unit->go(FORWARD);
            interface.set_lcd_text("Linha a frente", 1);
            break;
           case 2:
            // é uma curva fechada para direita
            control_unit->spin(control_unit->RIGHT);
            sprintf(text, "Linha muito a direta       ");
            break;
           case 3:
            // é uma curva fechada para esqerda
            control_unit->spin(control_unit->LEFT);
            sprintf(text, "Linha muito a esquerda       ");
            break;
           default:
            interface.set_lcd_text("Me perdi!", 1);
//            search_line();
        }
      }
    }
    interface.refresh_lcd(TRUE);
  }
}

void search_line() {
  // quando ambos os sensores deixam de ver a linha ao mesmo tempo
  // é uma curva fechada ou a linha acabou
  // o robo vai ficar virando para um lado e para o outro até encontrar a linha
  int curve_time = 500;
  int side = control_unit->LEFT;
  int r_status, l_status;
  int i;
  char text[50];
  do {
    control_unit->stop();
    for(i=0; i<10; i++) {
      update_line_sensor();
      print_line_sensor_status();
      delay(100);
    }
    r_status = line_sensor_r->status();
    l_status = line_sensor_l->status();
    control_unit->spin(side);
    if (side == control_unit->RIGHT) {
      interface.set_lcd_text("Tentando direita...", 1);
      interface.refresh_lcd(TRUE);
    } else {
      interface.set_lcd_text("Tentando esquerda...", 1);
      interface.refresh_lcd(TRUE);
    }
    sprintf(text, "Delay %d", curve_time);
    interface.set_lcd_text(text, 2);
    interface.refresh_lcd(TRUE);
    delay(curve_time);
    curve_time += 100;
    if(side == control_unit->LEFT) {
      side = control_unit->RIGHT;
    } else {
      side = control_unit->LEFT;
    }
    if(curve_time > 5000) {
      curve_time = 100;
      control_unit->go(FORWARD);
      delay(1000);
    }
  } while (r_status == line_sensor_r->OFF_LINE && l_status == line_sensor_l->OFF_LINE);
}

void update_line_sensor() {
  line_sensor_r->read();
  line_sensor_l->read();
}

void calibrate_line_sensors() {
  // menu de calibragem
  int r_line, l_line, r_off, l_off;
  char text1[50];
  char text2[50];
  interface.set_lcd_text("Ler linha", 1);
  interface.refresh_lcd(TRUE);
  do {
    update_line_sensor();
    r_line = line_sensor_r->get_value();
    l_line = line_sensor_l->get_value();
    sprintf(text1, "R:%d L:%d    ", r_line, l_line);
    interface.set_lcd_text(text1, 2);
    interface.refresh_lcd(FALSE);
  } while (interface.get_pressed_button() != SELECT_BUTTON);
  interface.button_release();
  r_line = line_sensor_r->get_value();
  l_line = line_sensor_l->get_value();

  interface.set_lcd_text("Ler fora da linha", 1);
  interface.refresh_lcd(TRUE);
  do {
    update_line_sensor();
    r_off = line_sensor_r->get_value();
    l_off = line_sensor_l->get_value();
    sprintf(text1, "R:%d L:%d     ", r_off, l_off);
    interface.set_lcd_text(text1, 2);
    interface.refresh_lcd(FALSE);
  } while(interface.get_pressed_button() != SELECT_BUTTON);
  interface.button_release();
  r_off = line_sensor_r->get_value();
  l_off = line_sensor_l->get_value();

  sprintf(text1, "R:(L %d,F %d)    ", r_line, r_off);
  sprintf(text2, "L:(L %d,F %d)    ", l_line, l_off);

  interface.set_lcd_text(text1, text2);
  interface.refresh_lcd(TRUE);
  
  line_sensor_r->calibrate(r_line, r_off);
  line_sensor_l->calibrate(l_line, r_off);
  do {
    update_line_sensor();
  } while(interface.get_pressed_button() != SELECT_BUTTON);
  interface.button_release();
}

void print_line_sensor_status() {
  // Printa os valores dos sensores
  char text1[20];
  char text2[20];
  char text[100];
  int r_status = line_sensor_r->status();
  int l_status = line_sensor_l->status();
  int r_value = line_sensor_r->get_value();
  int l_value = line_sensor_l->get_value();
  if(r_status == line_sensor_r->ON_LINE) {
    sprintf(text1, "R-L:%d", r_value);
  } else {
    sprintf(text1, "R-F:%d", r_value);
  }
  if(l_status == line_sensor_l->ON_LINE) {
    sprintf(text2, "L-L:%d", l_value);
  } else {
    sprintf(text2, "L-F:%d", l_value);
  }
  sprintf(text, "%s %s  ", text1, text2);
  interface.set_lcd_text(text, 2);
  interface.refresh_lcd(FALSE);
}
