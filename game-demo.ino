namespace demo {

  void printNum(int n) {
    static char str[5];
    sprintf(str, "%04d", n);
    lcd.print(str);
  }

  void start() {

    for (;;) {
      check_joystick();

      lcd.setCursor(0, 0);
      lcd.print("X: ");
      printNum(joystick_x);

      lcd.setCursor(0, 1);
      lcd.print("Y: ");
      printNum(joystick_y);

      lcd.setCursor(8, 0);
      lcd.print("S: ");
      lcd.print(joystick_sw ? "ON " : "OFF");

      lcd.setCursor(8, 1);
      lcd.print("D: ");
      switch (joystick_dir) {
      case UP:    lcd.print("UP   "); break;
      case DOWN:  lcd.print("DOWN "); break;
      case LEFT:  lcd.print("LEFT "); break;
      case RIGHT: lcd.print("RIGHT"); break;
      default:    lcd.print("NONE "); break;
      }

      if (joystick_dir != NONE) joystick_dir = NONE;

      if (joystick_sw) {
        tone(BUZZER_PIN, 200);
      } else {
        noTone(BUZZER_PIN);
      }
    }
  }

}
