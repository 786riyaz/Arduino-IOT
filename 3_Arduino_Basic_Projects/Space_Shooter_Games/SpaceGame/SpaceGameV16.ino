#include <U8g2lib.h>
#include <Wire.h>
#include <EEPROM.h>

#define BTN_LEFT 2
#define BTN_RIGHT 3
#define BTN_FIRE 4
#define BTN_PAUSE 6
#define BUZZER 8

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define MAX_BULLETS 3
#define MAX_ENEMY_BULLETS 3
#define MAX_ENEMY_LASERS 1
#define MAX_ENEMIES 6
#define MAX_POWERUPS 2
#define INVINCIBILITY_MS 1500
#define SHIELD_DURATION 5000
#define MAX_LIVES 10

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const uint8_t PROGMEM heroBitmap[] = {
  0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10
};
const uint8_t PROGMEM colliderBitmap[] = {
  0xFE, 0xC4, 0xC4, 0xFE, 0xFE, 0xC4, 0xC4
};
const uint8_t PROGMEM bulletEnemyBitmap[] = {
  0xFE, 0xFE, 0x38, 0x38, 0x38, 0x38, 0x38
};
const uint8_t PROGMEM laserEnemyBitmap[] = {
  0xFE, 0xFE, 0x7C, 0x7C, 0x38, 0x38, 0x10
};
const uint8_t PROGMEM heartBitmap[] = {
  0x0A, 0x1F, 0x1F, 0x0E, 0x04
};
const uint8_t PROGMEM shieldBitmap[] = {
  0x0E, 0x1F, 0x15, 0x1F, 0x0E
};
const uint8_t PROGMEM bigBangBitmap[] = {
  0x04, 0x15, 0x0E, 0x15, 0x04
};

enum EnemyType { COLLIDER, BULLET, LASER };
enum PowerUpType { NONE, EXTRA_LIFE, SHIELD, BIG_BANG };
struct Entity {
  int8_t x, y;
  bool active;
};
struct Enemy {
  int8_t x, y;
  bool active;
  EnemyType type;
};
struct PowerUp {
  int8_t x, y;
  bool active;
  PowerUpType type;
};

Entity bullets[MAX_BULLETS];
Entity enemyBullets[MAX_ENEMY_BULLETS];
Entity enemyLasers[MAX_ENEMY_LASERS];
Enemy enemies[MAX_ENEMIES];
PowerUp powerups[MAX_POWERUPS];

int8_t playerX = SCREEN_WIDTH / 2;
int8_t lives = 3;
uint16_t score = 0;
bool rapidFire = false;
bool shieldActive = false;
unsigned long lastShot = 0;
unsigned long lastEnemyMove = 0;
unsigned long lastPowerupSpawn = 0;
unsigned long lastEnemyShot = 0;
unsigned long gameStartTime = 0;
unsigned long shieldStartTime = 0;
uint16_t highScore = 0;
bool isPaused = false;
bool inGame = false;

void setup() {
  Serial.begin(9600);
  if (u8g2.begin()) {
    u8g2.setContrast(128);
    Serial.println(F("OLED initialized successfully"));
  } else {
    Serial.println(F("OLED initialization failed!"));
    while (1); // Halt if OLED fails
  }

  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_FIRE, INPUT_PULLUP);
  pinMode(BTN_PAUSE, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  highScore = (EEPROM.read(0) << 8) | EEPROM.read(1);
  showStartupScreen();
}

void playShootSound() {
  tone(BUZZER, 800, 20);
}

void playHitSound() {
  tone(BUZZER, 500, 30);
}

void playDamageSound() {
  tone(BUZZER, 200, 100);
}

void playExtraLifeSound() {
  tone(BUZZER, 1000, 50); delay(60);
  tone(BUZZER, 1200, 50); delay(60);
  tone(BUZZER, 1400, 100);
}

void playShieldSound() {
  tone(BUZZER, 600, 50); delay(60);
  tone(BUZZER, 800, 50); delay(60);
  tone(BUZZER, 1000, 100);
}

void playBigBangSound() {
  tone(BUZZER, 400, 100); delay(120);
  tone(BUZZER, 600, 100); delay(120);
  tone(BUZZER, 800, 200);
}

void saveHighScore() {
  if (score > highScore) {
    highScore = score;
    EEPROM.update(0, highScore >> 8);
    EEPROM.update(1, highScore & 0xFF);
  }
}

void startGame() {
  playerX = SCREEN_WIDTH / 2;
  lives = 3;
  score = 0;
  gameStartTime = millis();
  inGame = true;
  rapidFire = false;
  shieldActive = false;
  shieldStartTime = 0;

  for (uint8_t i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
  for (uint8_t i = 0; i < MAX_ENEMY_BULLETS; i++) enemyBullets[i].active = false;
  for (uint8_t i = 0; i < MAX_ENEMY_LASERS; i++) enemyLasers[i].active = false;
  for (uint8_t i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
  for (uint8_t i = 0; i < MAX_POWERUPS; i++) powerups[i].active = false;
}

void spawnEnemy() {
  uint8_t activeEnemies = 0;
  for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) activeEnemies++;
  }
  if (activeEnemies >= MAX_ENEMIES) return;

  for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) {
      enemies[i].x = random(0, SCREEN_WIDTH - 7);
      enemies[i].y = 0;
      enemies[i].active = true;
      uint8_t randType = random(100);
      enemies[i].type = (randType < 40) ? COLLIDER : (randType < 70) ? BULLET : LASER;
      break;
    }
  }
}

void spawnEnemyBullet(int8_t x, int8_t y) {
  for (uint8_t i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (!enemyBullets[i].active) {
      enemyBullets[i] = { int8_t(x + 3), int8_t(y + 7), true };
      playShootSound();
      break;
    }
  }
}

void spawnEnemyLaser(int8_t x, int8_t y) {
  for (uint8_t i = 0; i < MAX_ENEMY_LASERS; i++) {
    if (!enemyLasers[i].active) {
      enemyLasers[i] = { int8_t(x + 3), int8_t(y + 7), true };
      playShootSound();
      break;
    }
  }
}

void spawnPowerUp() {
  for (uint8_t i = 0; i < MAX_POWERUPS; i++) {
    if (!powerups[i].active) {
      powerups[i].x = random(0, SCREEN_WIDTH - 5);
      powerups[i].y = 0;
      powerups[i].active = true;
      uint8_t randType = random(100);
      powerups[i].type = (randType < 20) ? BIG_BANG : (randType < 50) ? EXTRA_LIFE : SHIELD;
      break;
    }
  }
}

void drawLives() {
  for (uint8_t i = 0; i < lives; i++) {
    u8g2.drawXBMP(2 + i * 8, 2, 5, 5, heartBitmap);
  }
}

void showStartupScreen() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(20, 30, F("SPACE SHOOTER"));
    u8g2.drawStr(20, 50, F("Press FIRE"));
  } while (u8g2.nextPage());
}

void loop() {
  if (!inGame) {
    if (digitalRead(BTN_FIRE) == LOW) {
      delay(200);
      startGame();
      for (uint8_t i = 0; i < 5; i++) spawnEnemy();
    }
    return;
  }

  if (!isPaused) {
    if (digitalRead(BTN_LEFT) == LOW && playerX > 0) playerX -= 2;
    if (digitalRead(BTN_RIGHT) == LOW && playerX < SCREEN_WIDTH - 7) playerX += 2;
    if (digitalRead(BTN_FIRE) == LOW && millis() - lastShot > (rapidFire ? 150 : 400)) {
      for (uint8_t i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
          bullets[i] = { int8_t(playerX + 3), int8_t(SCREEN_HEIGHT - 10), true };
          lastShot = millis();
          playShootSound();
          break;
        }
      }
    }
  }

  if (digitalRead(BTN_PAUSE) == LOW) {
    isPaused = !isPaused;
    delay(200);
  }

  if (!isPaused) {
    if (shieldActive && millis() - shieldStartTime > SHIELD_DURATION) {
      shieldActive = false;
    }

    for (uint8_t i = 0; i < MAX_BULLETS; i++) {
      if (bullets[i].active) {
        bullets[i].y -= 3;
        if (bullets[i].y < 0) bullets[i].active = false;
      }
    }

    for (uint8_t i = 0; i < MAX_ENEMY_BULLETS; i++) {
      if (enemyBullets[i].active) {
        enemyBullets[i].y += 3;
        if (enemyBullets[i].y > SCREEN_HEIGHT) enemyBullets[i].active = false;
      }
    }

    for (uint8_t i = 0; i < MAX_ENEMY_LASERS; i++) {
      if (enemyLasers[i].active) {
        enemyLasers[i].y += 2;
        if (enemyLasers[i].y > SCREEN_HEIGHT) enemyLasers[i].active = false;
      }
    }

    for (uint8_t i = 0; i < MAX_POWERUPS; i++) {
      if (powerups[i].active) {
        powerups[i].y += 1;
        if (powerups[i].y > SCREEN_HEIGHT) powerups[i].active = false;
      }
    }

    if (millis() - lastPowerupSpawn > random(5000, 10000)) {
      spawnPowerUp();
      lastPowerupSpawn = millis();
    }

    if (millis() - lastEnemyMove > 500) {
      uint8_t activeEnemies = 0;
      for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
          enemies[i].y += 4;
          if (enemies[i].y > SCREEN_HEIGHT) enemies[i].active = false;
          else activeEnemies++;
          if (enemies[i].type == BULLET && millis() - lastEnemyShot > 1000 && random(100) < 10) {
            spawnEnemyBullet(enemies[i].x, enemies[i].y);
            lastEnemyShot = millis();
          }
          else if (enemies[i].type == LASER && millis() - lastEnemyShot > 1500 && random(100) < 8) {
            spawnEnemyLaser(enemies[i].x, enemies[i].y);
            lastEnemyShot = millis();
          }
        }
      }
      if (activeEnemies < 5) spawnEnemy();
      lastEnemyMove = millis();
    }

    unsigned long currentTime = millis();
    bool playerVulnerable = currentTime - gameStartTime > INVINCIBILITY_MS && !shieldActive;

    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
      if (!enemies[i].active) continue;
      for (uint8_t j = 0; j < MAX_BULLETS; j++) {
        if (bullets[j].active && abs(enemies[i].x - bullets[j].x) < 8 && abs(enemies[i].y - bullets[j].y) < 8) {
          enemies[i].active = false;
          bullets[j].active = false;
          score++;
          playHitSound();
          saveHighScore();
        }
      }
      if (playerVulnerable && abs(enemies[i].x - playerX) < 8 && enemies[i].y >= SCREEN_HEIGHT - 10) {
        enemies[i].active = false;
        lives--;
        playDamageSound();
        if (lives <= 0) {
          saveHighScore();
          inGame = false;
          showStartupScreen();
          return;
        }
      }
    }

    for (uint8_t i = 0; i < MAX_ENEMY_BULLETS; i++) {
      if (enemyBullets[i].active && playerVulnerable &&
          abs(enemyBullets[i].x - playerX) < 8 && enemyBullets[i].y >= SCREEN_HEIGHT - 10) {
        enemyBullets[i].active = false;
        lives--;
        playDamageSound();
        if (lives <= 0) {
          saveHighScore();
          inGame = false;
          showStartupScreen();
          return;
        }
      }
    }

    for (uint8_t i = 0; i < MAX_ENEMY_LASERS; i++) {
      if (enemyLasers[i].active && playerVulnerable &&
          abs(enemyLasers[i].x - playerX) < 8 && enemyLasers[i].y >= SCREEN_HEIGHT - 10) {
        enemyLasers[i].active = false;
        lives--;
        playDamageSound();
        if (lives <= 0) {
          saveHighScore();
          inGame = false;
          showStartupScreen();
          return;
        }
      }
    }

    for (uint8_t i = 0; i < MAX_POWERUPS; i++) {
      if (powerups[i].active && abs(powerups[i].x - playerX) < 8 && abs(powerups[i].y - (SCREEN_HEIGHT - 8)) < 8) {
        powerups[i].active = false;
        if (powerups[i].type == EXTRA_LIFE && lives < MAX_LIVES) {
          lives++;
          playExtraLifeSound();
        } else if (powerups[i].type == SHIELD) {
          shieldActive = true;
          shieldStartTime = currentTime;
          playShieldSound();
        } else if (powerups[i].type == BIG_BANG) {
          for (uint8_t j = 0; j < MAX_ENEMIES; j++) enemies[j].active = false;
          for (uint8_t j = 0; j < MAX_ENEMY_BULLETS; j++) enemyBullets[j].active = false;
          for (uint8_t j = 0; j < MAX_ENEMY_LASERS; j++) enemyLasers[j].active = false;
          score += 5;
          playBigBangSound();
          saveHighScore();
        }
      }
    }
  }

  u8g2.firstPage();
  do {
    if (!shieldActive || (millis() % 200 < 100) && playerX <= 121) {
      u8g2.drawXBMP(playerX, SCREEN_HEIGHT - 8, 7, 7, heroBitmap);
    }
    for (uint8_t i = 0; i < MAX_BULLETS; i++) {
      if (bullets[i].active && bullets[i].x <= 126)
        u8g2.drawBox(bullets[i].x, bullets[i].y, 2, 4);
    }
    for (uint8_t i = 0; i < MAX_ENEMY_BULLETS; i++) {
      if (enemyBullets[i].active && enemyBullets[i].x <= 126)
        u8g2.drawBox(enemyBullets[i].x, enemyBullets[i].y, 2, 2);
    }
    for (uint8_t i = 0; i < MAX_ENEMY_LASERS; i++) {
      if (enemyLasers[i].active && enemyLasers[i].x <= 126)
        u8g2.drawBox(enemyLasers[i].x, enemyLasers[i].y, 2, 8);
    }
    for (uint8_t i = 0; i < MAX_ENEMIES; i++) {
      if (enemies[i].active && enemies[i].x <= 121) {
        const uint8_t* bitmap = (enemies[i].type == COLLIDER) ? colliderBitmap :
                                (enemies[i].type == BULLET) ? bulletEnemyBitmap : laserEnemyBitmap;
        u8g2.drawXBMP(enemies[i].x, enemies[i].y, 7, 7, bitmap);
      }
    }
    for (uint8_t i = 0; i < MAX_POWERUPS; i++) {
      if (powerups[i].active && powerups[i].x <= 123) {
        const uint8_t* bitmap = (powerups[i].type == EXTRA_LIFE) ? heartBitmap :
                                (powerups[i].type == SHIELD) ? shieldBitmap : bigBangBitmap;
        u8g2.drawXBMP(powerups[i].x, powerups[i].y, 5, 5, bitmap);
      }
    }
    drawLives();
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(80, 10);
    u8g2.print(F("S: ")); u8g2.print(score);
  } while (u8g2.nextPage());

  delay(30);
}