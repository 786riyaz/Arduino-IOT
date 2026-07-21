#include <Arduino.h>
#include <U8g2lib.h>
#include <EEPROM.h>

#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_FIRE 4
#define BUTTON_PAUSE 6
#define BUZZER_PIN 8

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Bitmaps (vertical)
const uint8_t PROGMEM eagle_bitmap[] = {
  0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10
};
const uint8_t PROGMEM squid_bitmap[] = {
  0x38, 0x7C, 0x54, 0x7C, 0x54, 0x6C, 0x44
};
const uint8_t PROGMEM shield_bitmap[] = {
  0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10
};
const uint8_t PROGMEM rapidfire_bitmap[] = {
  0x10, 0x28, 0x44, 0x82, 0x44, 0x28, 0x10
};

struct Bullet {
  int x, y;
  bool active;
};

struct Enemy {
  int x, y;
  bool alive;
  Bullet bullet;
};

struct PowerUp {
  int x, y;
  uint8_t type;
  bool active;
};

#define MAX_ENEMIES 10
#define MAX_BULLETS 5
#define MAX_POWERUPS 2

int playerX = 60;
int playerY = 56;
bool paused = false;

Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
PowerUp powerups[MAX_POWERUPS];

int score = 0;
int highScore = 0;
int level = 1;
int lives = 3;

bool rapidFire = false;
bool shield = false;
unsigned long lastShot = 0;
unsigned long lastEnemyFire = 0;
unsigned long lastPowerUpSpawn = 0;

void beep(int duration = 50) {
  tone(BUZZER_PIN, 1000);
  delay(duration);
  noTone(BUZZER_PIN);
}

void drawPlayer() {
  u8g2.drawXBMP(playerX, playerY, 7, 7, eagle_bitmap);
}

void drawEnemy(Enemy &e) {
  if (e.alive) u8g2.drawXBMP(e.x, e.y, 7, 7, squid_bitmap);
}

void drawPowerUp(PowerUp &p) {
  if (!p.active) return;
  switch (p.type) {
    case 0: u8g2.drawXBMP(p.x, p.y, 7, 7, shield_bitmap); break;
    case 1: u8g2.drawXBMP(p.x, p.y, 7, 7, rapidfire_bitmap); break;
  }
}

void resetGame() {
  score = 0;
  level = 1;
  lives = 3;
  rapidFire = false;
  shield = false;
  playerX = 60;
  for (auto &e : enemies) e.alive = false;
  for (auto &b : bullets) b.active = false;
  for (auto &p : powerups) p.active = false;
}

void fireBullet() {
  if (!rapidFire && millis() - lastShot < 400) return;
  for (auto &b : bullets) {
    if (!b.active) {
      b.active = true;
      b.x = playerX + 3;
      b.y = playerY;
      lastShot = millis();
      beep(20);
      return;
    }
  }
}

void enemyShoot(Enemy &e) {
  if (millis() - lastEnemyFire > 800) {
    e.bullet.x = e.x + 3;
    e.bullet.y = e.y + 7;
    e.bullet.active = true;
    lastEnemyFire = millis();
  }
}

void spawnEnemy() {
  for (auto &e : enemies) {
    if (!e.alive) {
      e.x = random(0, 121);
      e.y = 0;
      e.alive = true;
      return;
    }
  }
}

void spawnPowerUp() {
  for (auto &p : powerups) {
    if (!p.active) {
      p.x = random(0, 121);
      p.y = 0;
      p.active = true;
      p.type = random(0, 2);
      return;
    }
  }
}

void setup() {
  u8g2.begin();
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);
  pinMode(BUTTON_FIRE, INPUT);
  pinMode(BUTTON_PAUSE, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  EEPROM.get(0, highScore);
  resetGame();
}

void drawHUD() {
  u8g2.setCursor(0, 10);
  u8g2.print("S:" + String(score));
  u8g2.setCursor(45, 10);
  u8g2.print("L:" + String(level));
  u8g2.setCursor(90, 10);
  u8g2.print("V:" + String(lives));
}

void loop() {
  if (digitalRead(BUTTON_PAUSE) == HIGH) {
    paused = !paused;
    delay(300);
  }
  if (paused) return;

  if (digitalRead(BUTTON_LEFT)) playerX = max(0, playerX - 2);
  if (digitalRead(BUTTON_RIGHT)) playerX = min(121, playerX + 2);
  if (digitalRead(BUTTON_FIRE)) fireBullet();

  u8g2.clearBuffer();
  drawPlayer();
  drawHUD();

  for (auto &b : bullets) {
    if (!b.active) continue;
    u8g2.drawBox(b.x, b.y, 2, 4);
    b.y -= 5;
    if (b.y < 0) b.active = false;
  }

  for (auto &e : enemies) {
    if (!e.alive) continue;
    drawEnemy(e);
    e.y += 1; // enemy moves toward player
    if (random(0, 100) < 2) enemyShoot(e);
    if (e.bullet.active) {
      u8g2.drawBox(e.bullet.x, e.bullet.y, 2, 4);
      e.bullet.y += 3;
      if (e.bullet.y > 63) e.bullet.active = false;
      if (e.bullet.x >= playerX && e.bullet.x <= playerX + 7 && e.bullet.y >= playerY && e.bullet.y <= playerY + 7) {
        e.bullet.active = false;
        if (!shield) lives--;
        beep();
      }
    }
    for (auto &b : bullets) {
      if (b.active && e.alive && b.x >= e.x && b.x <= e.x + 7 && b.y >= e.y && b.y <= e.y + 7) {
        e.alive = false;
        b.active = false;
        score += 10;
        beep();
      }
    }
    if (e.alive && e.x + 7 >= playerX && e.x <= playerX + 7 && e.y + 7 >= playerY && e.y <= playerY + 7) {
      e.alive = false;
      if (!shield) lives--;
      beep();
    }
  }

  for (auto &p : powerups) {
    if (!p.active) continue;
    drawPowerUp(p);
    p.y += 2;
    if (p.y > 63) p.active = false;
    if (p.x >= playerX && p.x <= playerX + 7 && p.y >= playerY && p.y <= playerY + 7) {
      if (p.type == 0) shield = true;
      else if (p.type == 1) rapidFire = true;
      p.active = false;
      beep();
    }
  }

  if (millis() - lastPowerUpSpawn > 7000) {
    spawnPowerUp();
    lastPowerUpSpawn = millis();
  }

  if (random(0, 100) < 5) spawnEnemy();

  if (score >= level * 100) level++;

  if (lives <= 0) resetGame();

  u8g2.sendBuffer();
  delay(40);
}
