// Arduino Space Shooter Game with Bitmap, Power-Ups, Score, EEPROM, Sound Effects
// Hardware: 1.3" OLED (SH1106 @ 0x3C), Buttons on pins 2, 3, 4, 6, Buzzer on 8

#include <U8g2lib.h>
#include <EEPROM.h>

#define BTN_LEFT   2
#define BTN_RIGHT  3
#define BTN_FIRE   4
#define BTN_PAUSE  6
#define BUZZER     8

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// Bitmaps
const uint8_t HERO_BMP[] PROGMEM = {
  0b00010000,
  0b00111000,
  0b01111100,
  0b11111110,
  0b01111100,
  0b00111000,
  0b00010000,
  0b00010000
};

const uint8_t ENEMY_BMP[] PROGMEM = {
  0b00111000,
  0b01111100,
  0b11111110,
  0b11011011,
  0b11111111,
  0b01111110,
  0b00100100,
  0b01000010
};

const uint8_t HEART_BMP[] PROGMEM = {
  0b00000000,
  0b01100110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000
};

struct Entity {
  int x, y;
  bool active;
};

Entity bullets[5];
Entity enemyBullets[5];
Entity enemies[10];
Entity powerUp;

int heroX = 60;
int score = 0;
int lives = 3;
int highScore = 0;
bool paused = false;
unsigned long lastFire = 0;
unsigned long lastEnemyFire = 0;
unsigned long lastPowerUp = 0;

void setup() {
  pinMode(BTN_LEFT, INPUT);
  pinMode(BTN_RIGHT, INPUT);
  pinMode(BTN_FIRE, INPUT);
  pinMode(BTN_PAUSE, INPUT);
  pinMode(BUZZER, OUTPUT);

  display.begin();
  display.clear();
  EEPROM.get(0, highScore);
  showSplash();
  spawnEnemies();
  powerUp.active = false;
}

void showSplash() {
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(25, 20, "RIYAZ PATHAN");
  display.setFont(u8g2_font_8x13B_tr);
  display.drawStr(20, 40, "SPACE SHOOTER");
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(20, 58, "Press FIRE to Start");
  display.sendBuffer();
  while (digitalRead(BTN_FIRE) == LOW);
  delay(300);
}

void loop() {
  if (digitalRead(BTN_PAUSE) == HIGH) {
    paused = !paused;
    delay(300);
  }
  if (paused) return;

  handleInput();
  updateBullets();
  updateEnemies();
  updateEnemyBullets();
  checkCollisions();
  updatePowerUp();

  drawGame();
  delay(40);
}

void handleInput() {
  if (digitalRead(BTN_LEFT))  heroX = max(0, heroX - 2);
  if (digitalRead(BTN_RIGHT)) heroX = min(SCREEN_WIDTH - 8, heroX + 2);
  if (digitalRead(BTN_FIRE) && millis() - lastFire > 250) {
    for (auto &b : bullets) {
      if (!b.active) {
        b = {heroX + 3, 52, true};
        lastFire = millis();
        break;
      }
    }
  }
}

void updateBullets() {
  for (auto &b : bullets) {
    if (b.active) {
      b.y -= 4;
      if (b.y < 0) b.active = false;
    }
  }
}

void updateEnemies() {
  for (auto &e : enemies) {
    if (e.active) {
      e.y += 1;
      if (e.y > 64) {
        e.y = 0;
        e.x = random(0, SCREEN_WIDTH - 8);
      }
    }
  }

  if (millis() - lastEnemyFire > 1000) {
    for (auto &b : enemyBullets) {
      if (!b.active) {
        for (auto &e : enemies) {
          if (e.active) {
            b = {e.x + 3, e.y + 6, true};
            lastEnemyFire = millis();
            break;
          }
        }
        break;
      }
    }
  }
}

void updateEnemyBullets() {
  for (auto &b : enemyBullets) {
    if (b.active) {
      b.y += 3;
      if (b.y > 64) b.active = false;
    }
  }
}

void updatePowerUp() {
  if (!powerUp.active && millis() - lastPowerUp > 10000) {
    powerUp = {random(0, 120), 0, true};
    lastPowerUp = millis();
  }
  if (powerUp.active) {
    powerUp.y += 1;
    if (powerUp.y > 64) powerUp.active = false;
  }
}

void checkCollisions() {
  for (auto &b : bullets) {
    if (b.active) {
      for (auto &e : enemies) {
        if (e.active && b.x > e.x && b.x < e.x + 8 && b.y < e.y + 8 && b.y > e.y) {
          e.active = false;
          b.active = false;
          tone(BUZZER, 1200, 100);
          score++;
          if (score > highScore) EEPROM.put(0, score);
        }
      }
    }
  }
  for (auto &b : enemyBullets) {
    if (b.active && b.x > heroX && b.x < heroX + 8 && b.y > 56) {
      b.active = false;
      lives--;
      tone(BUZZER, 400, 200);
      if (lives <= 0) gameOver();
    }
  }
  if (powerUp.active && powerUp.x > heroX && powerUp.x < heroX + 8 && powerUp.y > 56) {
    powerUp.active = false;
    lives = min(5, lives + 1);
    tone(BUZZER, 900, 150);
  }
}

void drawGame() {
  display.clearBuffer();

  // Draw Hero
  display.drawXBMP(heroX, 56, 8, 8, HERO_BMP);

  // Draw Bullets
  for (auto &b : bullets)
    if (b.active) display.drawBox(b.x, b.y, 2, 4);

  // Draw Enemies
  for (auto &e : enemies)
    if (e.active) display.drawXBMP(e.x, e.y, 8, 8, ENEMY_BMP);

  // Draw Enemy Bullets
  for (auto &b : enemyBullets)
    if (b.active) display.drawBox(b.x, b.y, 2, 4);

  // Draw PowerUp
  if (powerUp.active) display.drawCircle(powerUp.x + 4, powerUp.y + 4, 3);

  // Draw Score
  display.setFont(u8g2_font_6x10_tr);
  display.setCursor(0, 10);
  display.print("Score: "); display.print(score);

  // Draw Lives as hearts
  for (int i = 0; i < lives; i++)
    display.drawXBMP(SCREEN_WIDTH - (i + 1) * 10, 0, 8, 8, HEART_BMP);

  display.sendBuffer();
}

void spawnEnemies() {
  for (int i = 0; i < 5; i++) {
    enemies[i] = {random(0, SCREEN_WIDTH - 8), random(0, 30), true};
  }
  for (int i = 5; i < 10; i++) {
    enemies[i].active = false;
  }
}

void gameOver() {
  display.clearBuffer();
  display.setFont(u8g2_font_8x13B_tr);
  display.drawStr(20, 30, "GAME OVER");
  display.setFont(u8g2_font_6x10_tr);
  display.setCursor(25, 50);
  display.print("Score: "); display.print(score);
  display.sendBuffer();
  while (true);
}
