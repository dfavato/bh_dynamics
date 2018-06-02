#include "LineSensor.h"

LineSensor *line_sensor_r = new LineSensor(LINE_SENSOR_R);
LineSensor *line_sensor_l = new LineSensor(LINE_SENSOR_L);

void line_follower_loop() {
  int button;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Segue linha");
  do {
    button = get_pressed_button();
    update_line_sensor();
    print_line_sensor_status();
    delay(100);
  } while (button == NO_BUTTON);
  button_release();
  if(button == SELECT) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Rodando...");
    delay(500);
    line_follower();
  } else if (button == RIGHT) {
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
  while(1) {
    update_line_sensor();
    r_status = line_sensor_r->status();
    l_status = line_sensor_l->status();
    if(r_status == line_sensor_r->ON_LINE) {
      if(l_status == line_sensor_l->ON_LINE) {
        // os dois sensores estão sobre a linha
        go_forward();
        estado = 1;
      } else {
        // sensor da esquerda fora da linha
        curve(RIGHT, 0);
        estado = 2;
      }
    } else {
      if(l_status == line_sensor_l->ON_LINE) {
        // sensor da direita fora da linha
        curve(LEFT, 0);
        estado = 3;
      } else {
        // ambos os sensores estão fora da linha
        // se ele está fazendo curva deve continuar
        switch (estado) {
          case 0:
            // o robo começou parado fora da linha
            go_forward();
            break;
           case 2:
            // é uma curva fechada para direita
            curve(RIGHT, 1);
            break;
           case 3:
            // é uma curva fechada para esqerda
            curve(LEFT, 1);
            break;
           default:
            search_line();
            break;
        }
      }
    }
  }
}

void search_line() {
  // quando ambos os sensores deixam de ver a linha ao mesmo tempo
  // é uma curva fechada ou a linha acabou
  // o robo vai ficar virando para um lado e para o outro até encontrar a linha
  int curve_time = 100;
  int side = RIGHT;
  int r_status, l_status;
  int i;
  do {
    motor_l->run(RELEASE);
    motor_r->run(RELEASE);
    for(i=0; i<10; i++) {
      update_line_sensor();  
    }
    r_status = line_sensor_r->status();
    l_status = line_sensor_l->status();
    curve(side, 1);
    delay(curve_time);
    curve_time += 100;
    if(side == RIGHT) {
      side == LEFT;
    } else {
      side == RIGHT;
    }
    if(curve_time > 5000) {
      curve_time = 100;
      go_forward();
      delay(1000);
    }
  } while (r_status == line_sensor_r->OFF_LINE && l_status == line_sensor_l->OFF_LINE);
}

void curve(int side, int turn) {
  // se turn == TRUE faz a curva sobre o próprio eixo
  int speed = 255;
  if(side == RIGHT) {
    motor_l->setSpeed(speed);
    motor_l->run(FORWARD);
    if(turn) {
      motor_r->setSpeed(speed);
      motor_r->run(BACKWARD);
    } else {
      motor_r->setSpeed(0);
      motor_r->run(RELEASE);  
    }
  } else {
    motor_r->setSpeed(speed);
    motor_r->run(FORWARD);
    if(turn) {
      motor_l->setSpeed(speed);
      motor_l->run(BACKWARD);
    } else {
      motor_l->setSpeed(0);
      motor_l->run(RELEASE); 
    }
  }
}

void go_forward() {
  int speed = 255;
  motor_r->setSpeed(speed);
  motor_l->setSpeed(speed);
  motor_r->run(FORWARD);
  motor_l->run(FORWARD);
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
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ler linha");
  do {
    update_line_sensor();
    r_line = line_sensor_r->get_value();
    l_line = line_sensor_l->get_value();
    sprintf(text1, "R:%d L:%d    ", r_line, l_line);
    lcd.setCursor(0,1);
    lcd.print(text1);
  } while (get_pressed_button() != SELECT);
  button_release();
  r_line = line_sensor_r->get_value();
  l_line = line_sensor_l->get_value();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ler fora da linha");
  do {
    update_line_sensor();
    r_off = line_sensor_r->get_value();
    l_off = line_sensor_l->get_value();
    sprintf(text1, "R:%d L:%d     ", r_off, l_off);
    lcd.setCursor(0,1);
    lcd.print(text1);
  } while(get_pressed_button() != SELECT);
  button_release();
  r_off = line_sensor_r->get_value();
  l_off = line_sensor_l->get_value();

  sprintf(text1, "R:(L %d,F %d)    ", r_line, r_off);
  sprintf(text2, "L:(L %d,F %d)    ", l_line, l_off);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(text1);
  lcd.setCursor(0,1);
  lcd.print(text2);
  
  line_sensor_r->calibrate(r_line, r_off);
  line_sensor_l->calibrate(l_line, r_off);
  do {
    update_line_sensor();
  } while(get_pressed_button() != SELECT);
  button_release();
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
  lcd.setCursor(0,1);
  lcd.print(text);
}
