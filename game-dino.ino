namespace dino {

  const int H = 2;
  const int W = 16;

  const byte DINO_0   = 1;
  const byte DINO_1   = 2;
  const byte DINO_2   = 3;
  const byte CACTUS_0 = 4;
  const byte CACTUS_1 = 5;
  const byte CACTUS_2 = 6;

  const byte dino_0[8] = {0b01110,
                          0b10111,
                          0b11100,
                          0b01110,
                          0b01100,
                          0b01110,
                          0b11100,
                          0b01010};

  const byte dino_1[8] = {0b01110,
                          0b10111,
                          0b11100,
                          0b01110,
                          0b01100,
                          0b01110,
                          0b11100,
                          0b01000};

  const byte dino_2[8] = {0b01110,
                          0b10111,
                          0b11100,
                          0b01110,
                          0b01100,
                          0b01110,
                          0b11100,
                          0b00010};

  const byte cactus_0[8] = {0b00000,
                            0b00100,
                            0b00101,
                            0b10101,
                            0b10101,
                            0b01110,
                            0b00100,
                            0b00100};

  const byte cactus_1[8] = {0b00000,
                            0b00000,
                            0b00000,
                            0b00100,
                            0b10101,
                            0b10101,
                            0b01110,
                            0b00100};

  const byte cactus_2[8] = {0b00100,
                            0b10101,
                            0b10101,
                            0b10101,
                            0b10110,
                            0b01100,
                            0b00100,
                            0b00100};

  const byte DINO_X = 2;
  const byte JUMP   = 3;

  bool running = false;

  int score;
  int extra_speed;
  byte jump;

  char mat[W][H];

  // https://www.norwegiancreations.com/2017/09/arduino-tutorial-using-millis-instead-of-delay/
  unsigned long time_now = 0;

  void draw() {
    for (int i = 0; i < W; ++i) {
      for (int j = 0; j < H; ++j) {
        lcd.setCursor(i, j);
        lcd.print(mat[i][j]);
      }
    }
  }

  void reset() {
    score       = 1;
    extra_speed = 0;
    jump        = 0;
    for (int i = 0; i < W; ++i) {
      for (int j = 0; j < H; ++j) {
        mat[i][j] = ' ';
      }
    }
    time_now = millis();
  }

  void game_over() {
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
  }

  bool is_dino(char c) {
    return c == DINO_0 || c == DINO_1 || c == DINO_2;
  }

  void game_loop() {

    // read controls
    check_joystick();
    switch (joystick_dir) {
    case UP:    --extra_speed; break;
    case DOWN:  ++extra_speed; break;
    case LEFT:
      jump = 0;
      break;
    case RIGHT:
      jump = JUMP;
      break;
    }
    joystick_dir = NONE;

    if (extra_speed < 0)   extra_speed = 0;
    if (extra_speed > 200) extra_speed = 200;

    // main loop
    int period = 700 / log(M_E + score + extra_speed);
    if (millis() >= time_now + period) {
      time_now += period;

      // reduce jump timer
      if (jump) {
        --jump;
      }

      // move world
      for (int i = 1; i < W; ++i) {
        // dino passes cactus
        if (is_dino(mat[i - 1][1]) && mat[i][1] != ' ') {
          game_over();
          return;
        } else if (is_dino(mat[i - 1][0]) && mat[i][1] != ' ') {
          ++score;
          tone(BUZZER_PIN, 500);
          delay(50);
          noTone(BUZZER_PIN);
          delay(50);
        }

        // shift stuff
        if (!is_dino(mat[i][1]) &&
            (mat[i][1] != ' ' || i < DINO_X)) {
          mat[i - 1][1] = mat[i][1];
          mat[i][1] = ' ';
        }
      }

      // check min cactus distance
      bool make_cactus = true;
      for (int i = W - JUMP - 3; i < W; ++i) {
        if (mat[i][0] != ' ' || mat[i][1] != ' ') {
          make_cactus = false;
        }
      }

      // create a new cactus
      if (random(10) < 2 && make_cactus) {
        mat[W - 1][1] = CACTUS_0 + random(3);
      }

    }

    // move dino
    if (millis() >= time_now + 100) {
      if (jump) {
        if (is_dino(mat[DINO_X][1])) {
          mat[DINO_X][1] = ' ';
        }
        mat[DINO_X][0] = DINO_0;
      } else {
        if (!is_dino(mat[DINO_X][1]) && mat[DINO_X][1] != ' ') {
          game_over();
          return;
        }
        if (is_dino(mat[DINO_X][0])) {
          mat[DINO_X][0] = ' ';
        }
        mat[DINO_X][1] = mat[DINO_X][1] == DINO_1 ?
          DINO_2 : DINO_1;
      }
    }
    
    draw();
  }

  void start() {

    lcd.createChar(DINO_0,   dino_0  );
    lcd.createChar(DINO_1,   dino_1  );
    lcd.createChar(DINO_2,   dino_2  );
    lcd.createChar(CACTUS_0, cactus_0);
    lcd.createChar(CACTUS_1, cactus_1);
    lcd.createChar(CACTUS_2, cactus_2);

    reset();
    mat[DINO_X][1] = DINO_0;
    draw();
    running = true;
    while (running) game_loop();
  }

}
