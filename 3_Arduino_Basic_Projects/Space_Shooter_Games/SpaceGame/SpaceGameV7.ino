// --- Space Shooter Game for Arduino Nano ---
// Display: 1.3-inch OLED I2C (SH1106/U8g2)
// Buttons: LEFT=2, RIGHT=3, FIRE=4, PAUSE=6
// Buzzer: 8

#include <U8g2lib.h>
#include <Wire.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

// Display Setup (SH1106, I2C address 0x3C)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Button Pins
const int btnLeft = 2;
const int btnRight = 3;
const int btnFire = 4;
const int btnPause = 6;
const int buzzer = 8;

// Game Constants
const int maxEnemies = 5;
const int maxBullets = 3;
const int maxEnemyBullets = 3;

// Bitmaps in PROGMEM
const unsigned char eagleBitmap[] PROGMEM = {
  0x10, 0x38, 0x7C, 0xFE, 0x38, 0x7C, 0x10
};
const unsigned char squidBitmap[] PROGMEM = {
  0x38, 0x44, 0x6C, 0x44, 0x38, 0x10, 0x28, 0x44
};
const unsigned char heartBitmap[] PROGMEM = {
  0x0A, 0x1F, 0x1F, 0x0E, 0x04
};

// Game State Variables
int playerX = 64;
int playerY = 56;
int score = 0;
int lives = 3;
int highScore = 0;
bool gameStarted = false;
bool paused = false;
unsigned long lastBulletTime = 0;
unsigned long lastEnemyMoveTime = 0;
unsigned long lastPowerupTime = 0;
bool rapidFire = false;
bool shield = false;

// Structures
struct Bullet {
  int x, y;
  bool active;
};
Bullet bullets[maxBullets];
Bullet enemyBullets[maxEnemyBullets];

struct Enemy {
  int x, y;
  bool active;
};
Enemy enemies[maxEnemies];

void setup() {
  u8g2.begin();
  Serial.begin(9600);

  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  pinMode(btnFire, INPUT_PULLUP);
  pinMode(btnPause, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  for (int i = 0; i < maxBullets; i++) bullets[i].active = false;
  for (int i = 0; i < maxEnemyBullets; i++) enemyBullets[i].active = false;
  for (int i = 0; i < maxEnemies; i++) enemies[i].active = false;

  highScore = EEPROM.read(0);
  showStartScreen();
}

void showStartScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 25, "RIYAZ PATHAN");
  u8g2.drawStr(10, 45, "SPACE SHOOTER");
  u8g2.drawStr(10, 60, "Press FIRE to start");
  u8g2.sendBuffer();

  while (digitalRead(btnFire) == HIGH) {
    delay(10);
  }
  gameStarted = true;
  score = 0;
  lives = 3;
  spawnEnemies();
  Serial.println(F("Game started"));
}

void spawnEnemies() {
  for (int i = 0; i < maxEnemies; i++) {
    enemies[i].x = random(0, 120);
    enemies[i].y = random(-60, 0);
    enemies[i].active = true;
  }
}

void loop() {
  if (!gameStarted) return;

  if (digitalRead(btnPause) == LOW) {
    paused = !paused;
    delay(300);
  }
  if (paused) return;

  handleInput();
  updateBullets();
  updateEnemies();
  updateEnemyBullets();
  checkCollisions();
  render();
  delay(50);
}

void handleInput() {
  if (digitalRead(btnLeft) == LOW && playerX > 0) playerX -= 3;
  if (digitalRead(btnRight) == LOW && playerX < 120) playerX += 3;

  if (digitalRead(btnFire) == LOW) {
    unsigned long now = millis();
    if (now - lastBulletTime > (rapidFire ? 100 : 300)) {
      for (int i = 0; i < maxBullets; i++) {
        if (!bullets[i].active) {
          bullets[i] = {playerX + 3, playerY, true};
          lastBulletTime = now;
          tone(buzzer, 1000, 100);
          break;
        }
      }
    }
  }
}

void updateBullets() {
  for (int i = 0; i < maxBullets; i++) {
    if (bullets[i].active) {
      bullets[i].y -= 4;
      if (bullets[i].y < 0) bullets[i].active = false;
    }
  }
}

void updateEnemyBullets() {
  for (int i = 0; i < maxEnemyBullets; i++) {
    if (enemyBullets[i].active) {
      enemyBullets[i].y += 3;
      if (enemyBullets[i].y > 64) enemyBullets[i].active = false;
    }
  }
}

void updateEnemies() {
  unsigned long now = millis();
  if (now - lastEnemyMoveTime > 500) {
    for (int i = 0; i < maxEnemies; i++) {
      if (enemies[i].active) {
        enemies[i].y += 4;
        if (enemies[i].y > 64) {
          enemies[i].y = 0;
          enemies[i].x = random(0, 120);
        }
        if (random(10) < 2) {
          for (int j = 0; j < maxEnemyBullets; j++) {
            if (!enemyBullets[j].active) {
              enemyBullets[j] = {enemies[i].x + 3, enemies[i].y + 8, true};
              break;
            }
          }
        }
      }
    }
    lastEnemyMoveTime = now;
  }
}

void checkCollisions() {
  for (int i = 0; i < maxEnemies; i++) {
    if (enemies[i].active) {
      for (int j = 0; j < maxBullets; j++) {
        if (bullets[j].active &&
            abs(bullets[j].x - enemies[i].x) < 6 &&
            abs(bullets[j].y - enemies[i].y) < 6) {
          enemies[i].active = false;
          bullets[j].active = false;
          score++;
          tone(buzzer, 1200, 100);
          if (score > highScore) {
            highScore = score;
            EEPROM.write(0, highScore);
          }
        }
      }
    }
  }

  for (int i = 0; i < maxEnemyBullets; i++) {
    if (enemyBullets[i].active &&
        abs(enemyBullets[i].x - playerX) < 6 &&
        abs(enemyBullets[i].y - playerY) < 6) {
      enemyBullets[i].active = false;
      lives--;
      tone(buzzer, 400, 200);
      if (lives <= 0) {
        gameStarted = false;
        EEPROM.write(0, highScore);
        showStartScreen();
      }
    }
  }
}

void render() {
  u8g2.clearBuffer();

  // Player
  u8g2.drawXBMP(playerX, playerY, 7, 7, eagleBitmap);

  // Enemies
  for (int i = 0; i < maxEnemies; i++) {
    if (enemies[i].active)
      u8g2.drawXBMP(enemies[i].x, enemies[i].y, 8, 8, squidBitmap);
  }

  // Bullets
  for (int i = 0; i < maxBullets; i++) {
    if (bullets[i].active)
      u8g2.drawBox(bullets[i].x, bullets[i].y, 2, 4);
  }

  // Enemy bullets
  for (int i = 0; i < maxEnemyBullets; i++) {
    if (enemyBullets[i].active)
      u8g2.drawBox(enemyBullets[i].x, enemyBullets[i].y, 2, 4);
  }

  // Score
  u8g2.setFont(u8g2_font_5x8_tf);
  u8g2.setCursor(0, 10);
  u8g2.print(F("Score:"));
  u8g2.print(score);

  // Lives as hearts
  for (int i = 0; i < lives; i++) {
    u8g2.drawXBMP(100 + (i * 10), 0, 5, 5, heartBitmap);
  }

  u8g2.sendBuffer();
}