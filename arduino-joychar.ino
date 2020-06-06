#define GAMES_LEN 3

namespace invaders { void start(); }
namespace race     { void start(); }
namespace demo     { void start(); }

const void (*games[])() = {invaders::start,
                           race::start,
                           demo::start};

const String games_names[] = {"Invaders",
                              "Race",
                              "Demo"};

#define USE_I2C_LCD
#define BUZZER_PIN 4
#define JOYSTICK_X_PIN A3
#define JOYSTICK_Y_PIN A2
#define JOYSTICK_S_PIN 5

#ifndef USE_I2C_LCD
#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#else
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#endif

// directions
// LEFT and RIGHT are also used for shapes (ored with the type)
#define UP    0x10
#define DOWN  0x20
#define LEFT  0x00
#define RIGHT 0x04
#define NONE  0xff

void setup() {

  Serial.begin(9600);

#ifndef USE_I2C_LCD
  lcd.begin(16, 2);
#else
  lcd.init();
  lcd.backlight();
#endif

  pinMode(JOYSTICK_S_PIN, INPUT);
  digitalWrite(JOYSTICK_S_PIN, HIGH);

  main_menu();

}

// not needed
void loop() {}

bool joystick_sw = false;
byte joystick_dir = NONE;
int joystick_x = 0;
int joystick_y = 0;

void check_joystick() {

  joystick_sw = digitalRead(JOYSTICK_S_PIN) == LOW;

  joystick_y = analogRead(JOYSTICK_Y_PIN);
  if (joystick_y < 200) {
    joystick_dir = DOWN;
  } else if (joystick_y > 824) {
    joystick_dir = UP;
  }

  joystick_x = analogRead(JOYSTICK_X_PIN);
  if (joystick_x < 200) {
    joystick_dir = LEFT;
  } else if (joystick_x > 824) {
    joystick_dir = RIGHT;
  }

  /*Serial.print("JOYSTICK: ");
    Serial.print(joystick_sw); Serial.print(" - ");
    Serial.print(joystick_x); Serial.print(" - ");
    Serial.println(joystick_y);*/

}

void main_menu() {
  int sel_game = 0;
  for (;;) {

    lcd.setCursor(0, 0);
    lcd.print("> ");
    lcd.print(games_names[sel_game]);

    if (sel_game + 1 < GAMES_LEN) {
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.print(games_names[sel_game + 1]);
    }

    if (joystick_dir != NONE) {
      delay(300);
      joystick_dir = NONE;
    }

    check_joystick();
    switch (joystick_dir) {
    case LEFT:
      sel_game = (sel_game + 1) % GAMES_LEN;
      break;
    case RIGHT:
      sel_game = mod(sel_game - 1, GAMES_LEN);
      break;
    }

    if (joystick_sw) {
      games[sel_game]();
      lcd.clear();
    }

    if (joystick_dir != NONE) {
      lcd.clear();
    }

  }
}

int mod(int x, int N){
  return (x % N + N) % N;
}
