#include <Adafruit_MotorShield.h>
#include <LiquidCrystal.h>

#define MOTOR_R 3
#define MOTOR_L 4
#define LINE_SENSOR_R 12
#define LINE_SENSOR_L 13
#define ENCODER_L 47, 49
#define ENCODER_R 51, 53

#define RIGHT 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define SELECT 4
#define NO_BUTTON 5

LiquidCrystal lcd(8,9,4,5,6,7);
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motor_r = AFMS.getMotor(MOTOR_R);
Adafruit_DCMotor *motor_l = AFMS.getMotor(MOTOR_L);

void (*action[3])();
int menu_selection = 0;
const char* menu_display[] = {
  "Localizacao",
  "Odometria e Controle",
  "Navegacao",
};
char lcd_text1[50];
char lcd_text2[50];
int menu_length = sizeof(action) / sizeof(*action);

void update_lcd() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(lcd_text1);
  lcd.setCursor(0,1);
  lcd.print(lcd_text2);
}

void change_menu(int i) {
  menu_selection = (menu_selection + i) % menu_length;
  if(menu_selection < 0) {
    menu_selection += menu_length;
  }
  sprintf(lcd_text2, "%d. %s", menu_selection, menu_display[menu_selection]);
  update_lcd();
}

void setup() {
  // put your setup code here, to run once:
  AFMS.begin();
  lcd.begin(16,2);
  Serial.begin(9600);
  sprintf(lcd_text1, "BH Dynamics");
  sprintf(lcd_text2, "%d. %s", menu_selection, menu_display[menu_selection]);
  update_lcd();
  action[2] = line_follower_loop;
  action[1] = control_loop;
}

void loop() {
  // put your main code here, to run repeatedly:
  int button;
  do {
    button = get_pressed_button();
  } while (button == NO_BUTTON);
  button_release();
  switch (button) {
    case UP:
      change_menu(1);
      break;
    case DOWN:
      change_menu(-1);
      break;
    case SELECT:
      (*action[menu_selection])();
      update_lcd();
      break;
    default:
      break;
  }
}
