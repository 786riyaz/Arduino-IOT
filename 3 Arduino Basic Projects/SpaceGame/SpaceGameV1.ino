// Space Shooter Game for Arduino Nano using U8g2 Library
// Display: 1.3" I2C OLED (SH1106 at 0x3C)

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <EEPROM.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Button pins
#define BTN_LEFT   2
#define BTN_RIGHT  3
#define BTN_FIRE   4
#define BTN_PAUSE  6

#define SHIP_WIDTH   8
#define SHIP_HEIGHT  5
#define BULLET_SPEED 4
#define ENEMY_SPEED  2
#define MAX_BULLETS  3
#define MAX_ENEMIES  15

int shipX = 60;
int shipY = 56;
bool gameRunning = false;
bool gamePaused = false;
bool showMenu = true;
bool gameOver = false;

unsigned long lastFrame = 0;
unsigned long frameDelay = 40;

int score = 0;
int highScore = 0;
int lives = 3;
int enemyCount = 5;

struct Bullet {
  int x, y;
  bool active;
};

struct Enemy {
  int x, y, dir, speed;
  bool active;
};

Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];

void resetBullets() {
  for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
}

void resetEnemies() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (i < enemyCount) {
      enemies[i].x = random(0, 120);
      enemies[i].y = random(5, 20);
      enemies[i].dir = random(0, 2) ? 1 : -1;
      enemies[i].speed = random(1, 3);
      enemies[i].active = true;
    } else {
      enemies[i].active = false;
    }
  }
}

void drawShip() {
  u8g2.drawBox(shipX, shipY, SHIP_WIDTH, SHIP_HEIGHT);
  u8g2.drawTriangle(shipX, shipY, shipX + 4, shipY - 4, shipX + 8, shipY);
}

void drawBullets() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      u8g2.drawBox(bullets[i].x, bullets[i].y, 2, 4);
      bullets[i].y -= BULLET_SPEED;
      if (bullets[i].y < 0) bullets[i].active = false;
    }
  }
}

void fireBullet() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) {
      bullets[i].x = shipX + 3;
      bullets[i].y = shipY - 4;
      bullets[i].active = true;
      break;
    }
  }
}

void drawEnemies() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      u8g2.drawBox(enemies[i].x, enemies[i].y, 8, 5);
      enemies[i].x += enemies[i].dir * enemies[i].speed;
      if (enemies[i].x <= 0 || enemies[i].x >= 120) enemies[i].dir *= -1;
    }
  }
}

void checkCollisions() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) continue;
    for (int j = 0; j < MAX_BULLETS; j++) {
      if (bullets[j].active && bullets[j].x >= enemies[i].x && bullets[j].x <= enemies[i].x + 8 &&
          bullets[j].y >= enemies[i].y && bullets[j].y <= enemies[i].y + 5) {
        bullets[j].active = false;
        enemies[i].active = false;
        score++;
        if (score > highScore) EEPROM.put(0, score);
        if (score % 10 == 0 && enemyCount < MAX_ENEMIES) {
          enemyCount++;
          resetEnemies();
        }
      }
    }
    if (enemies[i].y + 5 >= shipY && enemies[i].x >= shipX && enemies[i].x <= shipX + SHIP_WIDTH) {
      lives--;
      enemies[i].active = false;
      if (lives <= 0) {
        gameOver = true;
        gameRunning = false;
      }
    }
  }
}

void drawHUD() {
  u8g2.setCursor(0, 10);
  u8g2.print("Score:");
  u8g2.print(score);
  u8g2.setCursor(80, 10);
  u8g2.print("Lives:");
  u8g2.print(lives);
}

void drawMenu() {
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(30, 20, "RIYAZ PATHAN");
  u8g2.drawStr(20, 40, "SPACE SHOOTER");
  u8g2.drawStr(25, 60, "Press Fire to Start");
}

void drawGameOver() {
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(40, 25, "GAME OVER");
  u8g2.setCursor(20, 45);
  u8g2.print("Score: ");
  u8g2.print(score);
  u8g2.setCursor(20, 55);
  u8g2.print("High Score: ");
  u8g2.print(highScore);
  u8g2.drawStr(15, 64, "Fire = Restart | Pause = Menu");
}

void setup() {
  u8g2.begin();
  u8g2.enableUTF8Print();
  pinMode(BTN_LEFT, INPUT);
  pinMode(BTN_RIGHT, INPUT);
  pinMode(BTN_FIRE, INPUT);
  pinMode(BTN_PAUSE, INPUT);
  EEPROM.get(0, highScore);
}

void loop() {
  if (millis() - lastFrame < frameDelay) return;
  lastFrame = millis();
  u8g2.clearBuffer();

  if (showMenu) {
    drawMenu();
    u8g2.sendBuffer();
    if (digitalRead(BTN_FIRE) == HIGH) {
      showMenu = false;
      gameRunning = true;
      gameOver = false;
      score = 0;
      lives = 3;
      enemyCount = 5;
      resetEnemies();
      resetBullets();
    }
    return;
  }

  if (gameOver) {
    drawGameOver();
    u8g2.sendBuffer();
    if (digitalRead(BTN_FIRE) == HIGH) {
      gameOver = false;
      showMenu = false;
      gameRunning = true;
      score = 0;
      lives = 3;
      enemyCount = 5;
      resetEnemies();
      resetBullets();
    } else if (digitalRead(BTN_PAUSE) == HIGH) {
      gameOver = false;
      showMenu = true;
    }
    return;
  }

  if (digitalRead(BTN_PAUSE) == HIGH) {
    gamePaused = !gamePaused;
    delay(300); // debounce
  }

  if (gamePaused) {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(50, 30, "PAUSED");
    u8g2.sendBuffer();
    return;
  }

  if (digitalRead(BTN_LEFT) == HIGH) shipX = max(0, shipX - 2);
  if (digitalRead(BTN_RIGHT) == HIGH) shipX = min(120, shipX + 2);
  if (digitalRead(BTN_FIRE) == HIGH) fireBullet();

  drawShip();
  drawBullets();
  drawEnemies();
  checkCollisions();
  drawHUD();
  u8g2.sendBuffer();
}