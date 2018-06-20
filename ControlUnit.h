#ifndef ControlUnit_h
#define ControlUnit_h

#include <Adafruit_MotorShield.h>
#include <RotaryEncoder.h>

#define MIN_POWER 100
#define MAX_POWER 200
#define DEFAULT_POWER 200

class ControlUnit {
  Adafruit_MotorShield *AFMS; // AdaFruitMotorShield
  Adafruit_DCMotor *motor[2]; // motores
  RotaryEncoder *encoder[2]; // encoders
  int motor_power[2]; // valores utilizados no motor.setSpeed
  long last_error; // erro utilizado no último ajuste
  long target[2]; // objetivo de cada um dos encoders
  long last_encoder_position[2]; // leitura anterior à atualização da pose
  int state; // objetivo de deslocamento alcançado ou não
  unsigned long last_adjust; // última vez que a potência dos motores foi ajustada
  unsigned long last_pose_update; // última vez que a pose foi atualizada
  void reset_encoders(); // reseta os encoders
  void adjust_motor_power(); // ajusta a potência dos motores para que o robô ande reto ou gire sobre o próprio eixo
  int target_reached(long r_value, long l_value); // calcula se o objetivo foi alcançado ou não
  long calculate_error(long r_value, long l_value); // calcula o erro
  void set_target(long r_target, long l_target); // define um novo objetivo
  double pose_x; // posicao x do robo no campo
  double pose_y; // posicao y do robo no campo
  double pose_theta; // angulo do robo
  void update_pose();
  int action;
  int direction;
  int last_action;
  int last_direction;
  
  public:
    // Constantes para ajudar nas funções
    static const byte RIGHT = 0;
    static const byte LEFT = 1;
    static const byte TARGET_NOT_REACHED = 0;
    static const byte TARGET_REACHED = 1;

    ControlUnit(int motors_pins[], int encoders_pins[]); // constructor
    void stop(); // para os dois motores
    void go(int direction, int power); // FORWARD ou BACKWARD com a mesma potência em ambos motores
    void go(int direction); // FORWARD ou BACKWARD com uma potência default
    void curve(int side); // Uma roda parada enquanto a outra movimenta para frente
    void ControlUnit::curve(int side, int backward); // Uma roda parada enquanto a outra movimenta para tras
    void spin(int side); // uma roda para frente outra para trás
    void spin(int side, int power);
    int spin_degrees(int side, int degree); // giro preciso
    int drive_straight(int cm); // anda em linha reta
    drive_straight(int cm, int direction);
    long get_encoder_position(byte side); // pega a posição atual do encoder
    int get_motor_power(int side); // pega o valor atual da potência do motor
    void update_encoders(); // faz a leitura dos encoders
    int get_state(); // diz se o objetivo foi alcançado ou não.
    void set_pose(double new_x, double new_y, double new_theta);
    void set_pose_x(double new_x);
    void set_pose_y(double new_y);
    void set_pose_theta(double new_theta);
    double get_pose_x();
    double get_pose_y();
    double get_pose_theta();
};

#endif
