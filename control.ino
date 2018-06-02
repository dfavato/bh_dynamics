#include <RotaryEncoder.h>

#define WHEEL_DIAMETER 42 //mm
#define GEAR_RATIO 2.5
#define MAX_POWER 255
#define MIN_POWER 100
#define CONTROL_RIGHT 0
#define CONTROL_LEFT 1

RotaryEncoder* encoder_l = new RotaryEncoder(ENCODER_L);
RotaryEncoder* encoder_r = new RotaryEncoder(ENCODER_R);
int r_power = 255;
int l_power = 255;

int last_lcd_update;

void control_loop() {
  int button;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Odometria e controle");
  last_lcd_update = millis();
  encoder_l->setPosition(0);
  encoder_r->setPosition(0);
  do {
    button = get_pressed_button();
    update_encoder();
    print_encoder();
    delay(100);
  } while (button == NO_BUTTON);
  button_release();
  if(button == SELECT) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Rodando...");
    delay(500);
    drive_straight(60);
  }
  do {
  } while (get_pressed_button() != SELECT);
  button_release();
}

void update_encoder() {
  encoder_l->tick();
  encoder_r->tick();
}

void print_encoder() {
  char text[50];
  char text2[50];
  int l_value, r_value;
  int current_time = millis();
  if(current_time > last_lcd_update + 500) {
    l_value = encoder_l->getPosition();
    r_value = encoder_r->getPosition();
    sprintf(text, "L:%d R:%d      ", l_value, r_value);
    sprintf(text2, "ML:%d MR:%d      ", l_power, r_power);
    lcd.setCursor(0,0);
    lcd.print(text2);
    lcd.setCursor(0,1);
    lcd.print(text);
    last_lcd_update = current_time;
  }
}

void drive_straight(int cm) {
  int r_value, l_value;
  int target = encoder_target(cm);

  encoder_l->setPosition(0);
  encoder_r->setPosition(0);
  motor_l->setSpeed(l_power);
  motor_r->setSpeed(r_power);
  motor_l->run(FORWARD);
  motor_r->run(FORWARD);
  do {
    update_encoder();
    print_encoder();
    adjust_motor_power();
    l_value = encoder_l->getPosition();
    r_value = encoder_r->getPosition();
    if(l_value >= target) {
      motor_l->setSpeed(0);
      motor_l->run(RELEASE);
    }
    if(r_value >= target) {
      motor_r->setSpeed(0);
      motor_r->run(RELEASE);
    }
  } while (l_value < target || r_value < target);
  print_encoder();
  motor_l->run(RELEASE);
  motor_r->run(RELEASE);
}

void adjust_motor_power() {
  // se turn = TRUE quer dizer que o robo está girando e não indo reto
  int l_value = encoder_l->getPosition();
  int r_value = encoder_r->getPosition();
  int error = l_value - r_value;
  int kp = 50; // ganho proporcional ao erro
  if(error > 0) {
    // esquerda está indo mais rápido
    if(r_power >= MAX_POWER) {
      // não é possível acelarar o da direita
      l_power = l_power + (kp * error);
    } else {
      r_power = r_power + (kp * error);
    }
  } else if (error < 0) {
    // direita está mais rápido
    if(l_power >= MAX_POWER) {
      //não é possível acelarar o da esquerda
      r_power = r_power + (kp * error);
    } else {
      l_power = l_power + (kp * error);
    }
  }
  if(r_power > MAX_POWER) {
    r_power = MAX_POWER;
  } else if (r_power < MIN_POWER) {
    r_power = MIN_POWER;
  }
  if(l_power > MAX_POWER) {
    l_power = MAX_POWER;
  } else if (l_power < MIN_POWER) {
    l_power = MIN_POWER;
  }
  motor_r->setSpeed(r_power);
  motor_l->setSpeed(l_power);
}

int encoder_target(int cm) {
  // cm * 10 -> distância em mm
  // WHEEL_DIAMETER * PI -> circunferência da roda mm
  // (cm * 10)/(WHEEL_DIAMETER * PI) -> número de voltas que a roda tem fazer
  // * GEAR_RATION ->  a roda dá uma volta a cada 2.5 voltar do encoder
  // Uma volta do encoder são 25 ticks
  return ((cm * 10)/(WHEEL_DIAMETER * PI)) * GEAR_RATIO * 25;
}

