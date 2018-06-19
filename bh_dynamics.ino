#include "ControlUnit.h"
#include "Interface.h"
#include "LineSensor.h"
#include "RGBSensor.h"

#define TRUE 1
#define FALSE 0

#define RIGHT_SIDE 0
#define LEFT_SIDE 1

#define MOTOR_R 3 // vermelho para frente
#define MOTOR_L 4 // amarelo para frente
#define LINE_SENSOR_R 13
#define LINE_SENSOR_L 12
#define ENCODER_L 47, 49 // cinza e preto
#define ENCODER_R 51, 53 // laranja e marrom
#define POLAR_LIGHT_SENSOR_R 15
#define POLAR_LIGHT_SENSOR_L 10
#define RGBSENSOR_PIN 8
#define RGB_LEDS_PINS 33, 31, 29
#define BLOCK_SENSOR_PIN 25
#define BLOCK_LED_PIN 23
#define START_SENSOR_PIN 35

// Mudança
#define LINE_SENSOR_RR 11
#define LINE_SENSOR_LL 15
LineSensor *line_sensor_rr;
LineSensor *line_sensor_ll;

Interface interface = Interface();
LineSensor *line_sensor_r;
LineSensor *line_sensor_l;
RGBSensor *rgb_sensor;
GenericSensor *block_sensor;
ControlUnit *control_unit;
GenericSensor *light_sensor_r;
GenericSensor *light_sensor_l;
GenericSensor *start_sensor;
int block_led = BLOCK_LED_PIN;

const char* menu_display[] = {
  "Competicao",
  "Sensores",
  "Localizacao",
  "Odometria e Controle",
  "Navegacao",
  "Cores",
  "Botoes"
};
const int menu_length = sizeof(menu_display) / sizeof(*menu_display);
void (*action[menu_length])();
int menu_selection = 0;

void setup() {
  // put your setup code here, to run once:
  char text[50];
  Serial.begin(9600);

  initialize_control_unit();
  initialize_sensors();
  
  change_menu(0);
  action[0] = competition_loop;
  action[1] = sensor_loop;
  action[2] = location_loop;
  action[3] = control_loop;
  action[4] = line_follower_loop;
  action[5] = color_loop;
  action[9] = lcd_buttons_loop;

  pinMode(block_led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int button;
  do {
    button = interface.get_pressed_button();
  } while (button == NO_BUTTON);
  interface.button_release();
  switch (button) {
    case UP_BUTTON:
      change_menu(-1);
      break;
    case DOWN_BUTTON:
      change_menu(1);
      break;
    case SELECT_BUTTON:
      (*action[menu_selection])();
      change_menu(0);
      break;
    default:
      break;
  }
}

void initialize_control_unit() {
  const int motor_pins[2] = {MOTOR_R, MOTOR_L};
  const int encoder_pins[4] = {ENCODER_R, ENCODER_L};
  control_unit = new ControlUnit(motor_pins, encoder_pins);
}

void initialize_sensors() {
  const int rgbleds_pins[3] = {RGB_LEDS_PINS};
  light_sensor_r = new GenericSensor(POLAR_LIGHT_SENSOR_R, 0);
  light_sensor_l = new GenericSensor(POLAR_LIGHT_SENSOR_L, 0);
  line_sensor_r = new LineSensor(LINE_SENSOR_R);
  line_sensor_l = new LineSensor(LINE_SENSOR_L);
  //mudança
  
  line_sensor_rr = new LineSensor(LINE_SENSOR_RR);
  line_sensor_ll = new LineSensor(LINE_SENSOR_LL);
  
  rgb_sensor = new RGBSensor(RGBSENSOR_PIN, rgbleds_pins);
  block_sensor = new GenericSensor(BLOCK_SENSOR_PIN, 1);
  start_sensor = new GenericSensor(START_SENSOR_PIN, 1);
}

void change_menu(int i) {
  char text[50];
  menu_selection = (menu_selection + i) % menu_length;
  if(menu_selection < 0) {
    menu_selection +=  menu_length;
  }
  sprintf(text, "%d. %s", menu_selection, menu_display[menu_selection]);
  interface.set_lcd_text("BH Dynamics", text);
  interface.refresh_lcd(TRUE);
}

