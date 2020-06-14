namespace race {

  const int LOC_UP   = 0;
  const int LOC_DOWN = 1;

  const int CAR_UP_1      = 1;
  const int CAR_DOWN_1    = 2;
  const int CAR_UP_2      = 3;
  const int CAR_DOWN_2    = 4;
  const int STREET_UP_1   = 5;
  const int STREET_DOWN_1 = 6;
  const int STREET_UP_2   = 7;
  const int STREET_DOWN_2 = 8;

  const int W = 2;
  const int H = 16;

  const byte car_up_1[8] = {0b10100,
                            0b00000,
                            0b00000,
                            0b01001,
                            0b11110,
                            0b01001,
                            0b00000,
                            0b00000};

  const byte car_down_1[8] = {0b00000,
                              0b00000,
                              0b00000,
                              0b01001,
                              0b11110,
                              0b01001,
                              0b00000,
                              0b10100};

  const byte car_up_2[8] = {0b01010,
                            0b00000,
                            0b00000,
                            0b01001,
                            0b11110,
                            0b01001,
                            0b00000,
                            0b00000};

  const byte car_down_2[8] = {0b00000,
                              0b00000,
                              0b00000,
                              0b01001,
                              0b11110,
                              0b01001,
                              0b00000,
                              0b01010};

  const byte street_up_1[8] = {0b10100,
                               0b00000,
                               0b00000,
                               0b00000,
                               0b00000,
                               0b00000,
                               0b00000,
                               0b00000};

  const byte street_down_1[8] = {0b00000,
                                 0b00000,
                                 0b00000,
                                 0b00000,
                                 0b00000,
                                 0b00000,
                                 0b00000,
                                 0b10100};

  const byte street_up_2[8] = {0b01010,
                               0b00000,
                               0b00000,
                               0b00000,
                               0b00000,
                               0b00000,
                               0b00000,
                               0b00000};

  const byte street_down_2[8] = {0b00000,
                                 0b00000,
                                 0b00000,
                                 0b00000,
                                 0b00000,
                                 0b00000,
                                 0b00000,
                                 0b01010};

  bool running = false;

  int distance;
  int location;
  int other_location;

  int count;

  int score;

  int extra_speed = 0;

  char mat[2][H + 1];

  // https://www.norwegiancreations.com/2017/09/arduino-tutorial-using-millis-instead-of-delay/
  unsigned long time_now = 0;

  void reset() {
    count = 0;
    score = 1;
    extra_speed = 0;
    distance = H - 1;
    location = LOC_UP;
    other_location = random(2);
    time_now = millis();
  }

  char get_car_char(bool mode1, int location) {
    if (mode1) {
      return location == LOC_UP ? CAR_UP_1 : CAR_DOWN_1;
    } else {
      return location == LOC_UP ? CAR_UP_2 : CAR_DOWN_2;
    }
  }

  void game_loop() {

    // read controls
    check_joystick();
    switch (joystick_dir) {
    case RIGHT: location = LOC_UP;   break;
    case LEFT:  location = LOC_DOWN; break;
    case UP:    ++extra_speed;       break;
    case DOWN:  --extra_speed;       break;
    }

    if (extra_speed < 0)   extra_speed = 0;
    if (extra_speed > 200) extra_speed = 200;

    int period = 700 / log(M_E + score + extra_speed);
    if (millis() >= time_now + period) {
      time_now += period;

      bool mode1 = count++ % 2 == 0;

      for (int i = 0; i < H; ++i) {
        if (mode1) {
          mat[0][i] = STREET_UP_1;
          mat[1][i] = STREET_DOWN_1;
        } else {
          mat[0][i] = STREET_UP_2;
          mat[1][i] = STREET_DOWN_2;
        }
      }

      if (distance < 0) {
        if (location == other_location) {
          for (int i = 0; i < 3; ++i) {
            tone(BUZZER_PIN, 200);
            delay(100);
            noTone(BUZZER_PIN);
            delay(100);
          }
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("GAME OVER!");
          lcd.setCursor(0, 1);
          lcd.print("SCORE: ");
          lcd.print(score);
          reset();
          delay(3000);
          running = false;
          return;
        } else {
          other_location = random(2);
          distance = H - 1;
        }
        ++score;
      }

      mat[location][H - 1] = get_car_char(mode1, location);
      mat[other_location][H - distance] = get_car_char(mode1, other_location);
      --distance;

      lcd.setCursor(0, 0);
      lcd.print(mat[0]);
      lcd.setCursor(0, 1);
      lcd.print(mat[1]);

    }
  }

  void start() {

    lcd.createChar(CAR_UP_1, car_up_1);
    lcd.createChar(CAR_DOWN_1, car_down_1);
    lcd.createChar(CAR_UP_2, car_up_2);
    lcd.createChar(CAR_DOWN_2, car_down_2);
    lcd.createChar(STREET_UP_1, street_up_1);
    lcd.createChar(STREET_DOWN_1, street_down_1);
    lcd.createChar(STREET_UP_2, street_up_2);
    lcd.createChar(STREET_DOWN_2, street_down_2);

    reset();
    mat[0][H] = mat[1][H] = '\0';

    running = true;
    while (running) game_loop();
  }

}
