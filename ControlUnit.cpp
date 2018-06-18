#include "ControlUnit.h"
#include "math.h"

#define WHEEL_DIAMETER 49.28 //mm
#define GEAR_RATIO 2.5
#define WHEEL_AXIS_WIDTH 16.5 //cm
#define KP 0.2
#define KD 0.1
#define ADJUST_INTERVAL 100
#define TICKS_PER_REVOLUTION 25

#define STOPPED 0
#define GOING 1
#define CURVING 2
#define SPINNING 3

long calculate_straight_line_ticks(double cm) {
  // cm * 10 -> distância em mm
  // WHEEL_DIAMETER * PI -> circunferência da roda mm
  // (cm * 10)/(WHEEL_DIAMETER * PI) -> número de voltas que a roda tem fazer
  // * GEAR_RATION ->  a roda dá uma volta a cada 2.5 voltas do encoder
  // Uma volta do encoder são 25 ticks
  return (long)(((cm * (double)10)/((double)WHEEL_DIAMETER * PI)) * (double)GEAR_RATIO * (double)TICKS_PER_REVOLUTION);
}

double ticks_to_cm(long ticks) {
  double wheel_revolutions = ((double)ticks / (double)TICKS_PER_REVOLUTION) / (double) GEAR_RATIO;
  double wheel_circumference = ((double)WHEEL_DIAMETER * PI) / (double)10;
  return wheel_revolutions * wheel_circumference;
}

long calculate_spin_ticks(int degree) {
  double circumference = (double)WHEEL_AXIS_WIDTH * PI; // comprimento da circunferência que o robô desenha ao girar sobre o próprio eixo
  double arc = (circumference / (double)360) * (double) degree; // comprimento do arco correspondente aos graus do giro
  return calculate_straight_line_ticks(arc);
}

ControlUnit::ControlUnit(int motors_pins[], int encoders_pins[]) {
  this->AFMS = new Adafruit_MotorShield();
  this->AFMS->begin();
  this->motor[this->RIGHT] = AFMS->getMotor(motors_pins[this->RIGHT]);
  this->motor[this->LEFT] = AFMS->getMotor(motors_pins[this->LEFT]);
  this->encoder[this->RIGHT] = new RotaryEncoder(encoders_pins[this->RIGHT], encoders_pins[this->RIGHT+1]);
  this->encoder[this->LEFT] = new RotaryEncoder(encoders_pins[this->LEFT*2], encoders_pins[this->LEFT*2+1]);
  this->motor_power[this->RIGHT] = DEFAULT_POWER;
  this->motor_power[this->LEFT] = DEFAULT_POWER;
  this->target[this->RIGHT] = 0;
  this->target[this->LEFT] = 0;
  this->last_error = 0;
  this->state = this->TARGET_REACHED;
  this->reset_encoders();
  this->last_adjust = millis();
  this->last_pose_update = millis();
  this->set_pose(0,0,0);
  this->action = STOPPED;
  this->direction = FORWARD;
  this->last_action = STOPPED;
  this->last_direction = FORWARD;
}

void ControlUnit::stop() {
  this->motor[this->RIGHT]->setSpeed(0);
  this->motor[this->RIGHT]->run(BRAKE);
  this->motor[this->LEFT]->setSpeed(0);
  this->motor[this->LEFT]->run(BRAKE);
  this->action = STOPPED;
}

void ControlUnit::curve(int side) {
  this->action = CURVING;
  this->direction = side;
  if(side == this->RIGHT) {
    this->motor_power[this->LEFT] = MAX_POWER;
    this->motor_power[this->RIGHT] = 0;
    this->motor[this->LEFT]->setSpeed(this->motor_power[this->LEFT]);
    this->motor[this->LEFT]->run(FORWARD);
    this->motor[this->RIGHT]->setSpeed(this->motor_power[this->RIGHT]);
    this->motor[this->RIGHT]->run(BRAKE);
  } else {
    this->motor_power[this->RIGHT] = MAX_POWER;
    this->motor_power[this->LEFT] = 0;
    this->motor[this->RIGHT]->setSpeed(this->motor_power[this->RIGHT]);
    this->motor[this->RIGHT]->run(FORWARD);
    this->motor[this->LEFT]->setSpeed(this->motor_power[this->LEFT]);
    this->motor[this->LEFT]->run(BRAKE);
  }
}

void ControlUnit::go(int direction) {
  this->go(direction, DEFAULT_POWER);
}

void ControlUnit::go(int direction, int power) {
  this->direction = direction;
  this->action = GOING;
  this->motor_power[this->RIGHT] = power;
  this->motor_power[this->LEFT] = power;
  this->motor[this->LEFT]->setSpeed(this->motor_power[this->LEFT]);
  this->motor[this->LEFT]->run(direction);
  this->motor[this->RIGHT]->setSpeed(this->motor_power[this->RIGHT]);
  this->motor[this->RIGHT]->run(direction);
}

void ControlUnit::spin(int side) {
  this->spin(side, DEFAULT_POWER);
}

void ControlUnit::spin(int side, int power) {
  this->action = SPINNING;
  this->direction = side;
  this->motor_power[this->RIGHT] = power;
  this->motor_power[this->LEFT] = power;
  this->motor[this->LEFT]->setSpeed(this->motor_power[this->LEFT]);
  this->motor[this->RIGHT]->setSpeed(this->motor_power[this->RIGHT]);
  if(side == this->RIGHT) {
    this->motor[this->LEFT]->run(FORWARD);
    this->motor[this->RIGHT]->run(BACKWARD);
  } else {
    this->motor[this->RIGHT]->run(FORWARD);
    this->motor[this->LEFT]->run(BACKWARD);
  }
}

void ControlUnit::reset_encoders() {
  this->last_error = 0;
  this->last_encoder_position[this->RIGHT] = 0;
  this->last_encoder_position[this->LEFT] = 0;
  this->encoder[this->RIGHT]->setPosition(0);
  this->encoder[this->LEFT]->setPosition(0);
}

void ControlUnit::update_pose() {
  long position_r, position_l; // posição atual dos encoders
  long wr, wl; // velocidades angulares (ticks / UPDATE_INTERVAL)
  double vr, vl;

  position_r = this->get_encoder_position(this->RIGHT);
  position_l = this->get_encoder_position(this->LEFT);

  wr = position_r - this->last_encoder_position[this->RIGHT];
  wl = position_l - this->last_encoder_position[this->LEFT];

  vr = ticks_to_cm(wr);
  vl = ticks_to_cm(wl);
  this->pose_x += vr * cos(this->pose_theta) / 2 + vl * cos(this->pose_theta) / 2;
  this->pose_y += vr * sin(this->pose_theta) / 2 + vl * sin(this->pose_theta) / 2;
  this->pose_theta += vr / (double)WHEEL_AXIS_WIDTH - vl / (double)WHEEL_AXIS_WIDTH;

  this->last_encoder_position[this->RIGHT] = position_r;
  this->last_encoder_position[this->LEFT] = position_l;
}

void ControlUnit::update_encoders() {
  unsigned long now = millis();

  this->encoder[this->RIGHT]->tick();
  this->encoder[this->LEFT]->tick();

  if(this->last_action != this->action || this->last_direction != this->direction) {
    Serial.println("POSE UPDATED!");
    this->update_pose();
    this->last_action = this->action;
    this->last_direction = this->direction;
  }
}

long ControlUnit::get_encoder_position(byte side) {
  return this->encoder[side]->getPosition();
}

void ControlUnit::set_target(long r_target, long l_target) {
  this->target[this->RIGHT] = r_target;
  this->target[this->LEFT] = l_target;
  this->state = TARGET_NOT_REACHED;
  Serial.print("Target: ");
  Serial.println(r_target, DEC);
}

int ControlUnit::spin_degrees(int side, int degree) {
  long target;

  // se o estado é TARGET_REACHED quer dizer que o robô já alcançou
  // o último objetivo e podemos setar um novo
  // caso o estado seja TARGET_NOT_REACHED então um novo objetivo
  // não deve ser setado e deve-se seguir para o código de manutençao abaixo
  if(this->state == this->TARGET_REACHED) {
    target = calculate_spin_ticks(degree);
    this->reset_encoders();
    if(side == this->RIGHT) {
      this->set_target(-target, target);
    } else {
      this->set_target(target, -target);
    }
    this->spin(side, DEFAULT_POWER);
  }

  this->update_encoders();
  this->adjust_motor_power(); //sempre atualiza o state caso o objetivo seja alcançado
  return this->state;
}

int ControlUnit::drive_straight(int cm, int direction) {
  if(direction == FORWARD) this->drive_straight(cm);
  long target;

  // se o estado é TARGET_REACHED quer dizer que o robô já alcançou
  // o último objetivo e podemos setar um novo
  // caso o estado seja TARGET_NOT_REACHED então um novo objetivo
  // não deve ser setado e deve-se seguir para o código de manutençao abaixo
  if(this->state == TARGET_REACHED) {
    target = calculate_straight_line_ticks(cm);
    this->reset_encoders();
    this->set_target(-target, -target);
    this->go(BACKWARD, DEFAULT_POWER);
  }
  
  this->update_encoders();
  this->adjust_motor_power(); //sempre atualiza o state caso o objetivo seja alcançado
  return this->state;
}

int ControlUnit::drive_straight(int cm) {
  long target;

  // se o estado é TARGET_REACHED quer dizer que o robô já alcançou
  // o último objetivo e podemos setar um novo
  // caso o estado seja TARGET_NOT_REACHED então um novo objetivo
  // não deve ser setado e deve-se seguir para o código de manutençao abaixo
  if(this->state == TARGET_REACHED) {
    target = calculate_straight_line_ticks(cm);
    this->reset_encoders();
    this->set_target(target, target);
    this->go(FORWARD, DEFAULT_POWER);
  }
  
  this->update_encoders();
  this->adjust_motor_power(); //sempre atualiza o state caso o objetivo seja alcançado
  return this->state;
}

void ControlUnit::adjust_motor_power() {
  unsigned long now = millis();
  long r_value = this->get_encoder_position(this->RIGHT);
  long l_value = this->get_encoder_position(this->LEFT);
  long error;
  int offset;

  if(this->target_reached(r_value, l_value)) {
    this->stop();
    return;
  }

  if(now > this->last_adjust + ADJUST_INTERVAL) {
    error = this->calculate_error(r_value, l_value);
    offset = (int)((float)(error * (float)KP) + (float)((float)(error - last_error) * (float)KD));
    this->last_error = error;
    this->last_adjust = now;
    // se o erro é positvo RIGHT mais rápido que LEFT e offset POSITIVO? (dependo do derivativo)
    // se RIGHT mais rápido -> offset positivo, aumentamos o power do LEFT
    // se RIGHT mais devagar -> offset negativo, diminuimos o power do LEFT
    if(this->motor_power[this->LEFT] + offset > MAX_POWER || this->motor_power[this->LEFT] + offset < MIN_POWER) {
      // nesse caso não é possível alterar o motor LEFT..
      this->motor_power[this->RIGHT] -= offset;
    } else {
      this->motor_power[this->LEFT] += offset;
    }
  }
  this->motor[this->LEFT]->setSpeed(this->motor_power[this->LEFT]);
  this->motor[this->RIGHT]->setSpeed(this->motor_power[this->RIGHT]);
}

int ControlUnit::get_motor_power(int side) {
  return this->motor_power[side];
}

int ControlUnit::get_state() {
  return this->state;
}

int ControlUnit::target_reached(long r_value, long l_value) {
  long target_r = this->target[this->RIGHT];
  long target_l = this->target[this->LEFT];
  if(target_r < 0) {
    r_value = -r_value;
    target_r = -target_r;
  }
  
  if(target_l < 0) {
    l_value = -l_value;
    target_l = -target_l;
  }

  int target_reached = (l_value >= target_l || r_value >= target_r);
  if(target_reached) {
    this->state = this->TARGET_REACHED;
    this->stop();
  }
    
  
  return target_reached;
}

long ControlUnit::calculate_error(long r_value, long l_value) {
  if(this->target[this->RIGHT] < 0)
    r_value = -r_value;

  if(this->target[this->LEFT] < 0)
    l_value = -l_value;
  
  return r_value - l_value;
}

double ControlUnit::get_pose_x() {
  return this->pose_x;
}

double ControlUnit::get_pose_y() {
  return this->pose_y;
}

double ControlUnit::get_pose_theta() {
  return (double)((int)(this->pose_theta / PI * 180) % 360);
}

void ControlUnit::set_pose(double x, double y, double theta) {
  this->pose_x = x;
  this->pose_y = y;
  this->pose_theta = theta;
}

void ControlUnit::set_pose_x(double x) {
  this->pose_x = x;
}

void ControlUnit::set_pose_y(double y) {
  this->pose_y = y;
}

void ControlUnit::set_pose_theta(double theta) {
  this->pose_theta = (theta / (double)180) * PI;
}

