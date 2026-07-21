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
// Alternative: U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, A5, A4, U8X8_PIN_NONE);

// Bitmaps
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

// Enemy and Power-up types
enum EnemyType { COLLIDER, BULLET, LASER };
enum PowerUpType { NONE, EXTRA_LIFE, SHIELD, BIG_BANG };
struct Enemy {
  int x, y;
  bool active;
  EnemyType type;
};
struct PowerUp {
  int x, y;
  bool active;
  PowerUpType type;
};
struct Entity {
  int x, y;
  bool active;
};

Entity bullets[MAX_BULLETS];
Entity enemyBullets[MAX_ENEMY_BULLETS];
Entity enemyLasers[MAX_ENEMY_LASERS];
Enemy enemies[MAX_ENEMIES];
PowerUp powerups[MAX_POWERUPS];

int playerX = SCREEN_WIDTH / 2;
int lives = 3;
int score = 0;
bool rapidFire = false;
bool shieldActive = false;
unsigned long lastShot = 0;
unsigned long lastEnemyMove = 0;
unsigned long lastPowerupSpawn = 0;
unsigned long lastEnemyShot = 0;
unsigned long gameStartTime = 0;
unsigned long shieldStartTime = 0;
int highScore = 0;
bool isPaused = false;
bool inGame = false;

// Setup
void setup() {
  Serial.begin(9600);
  Serial.println(F("Initializing OLED..."));
  if (u8g2.begin()) {
    Serial.println(F("OLED initialized successfully"));
    u8g2.setContrast(128);
  } else {
    Serial.println(F("OLED initialization failed!"));
  }

  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_FIRE, INPUT_PULLUP);
  pinMode(BTN_PAUSE, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  highScore = (EEPROM.read(0) << 8) | EEPROM.read(1);
  showStartupScreen();
}

// Sound effects
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

// Save high score
void saveHighScore() {
  if (score > highScore) {
    highScore = score;
    EEPROM.update(0, highScore >> 8);
    EEPROM.update(1, highScore & 0xFF);
  }
}

// Start Game
void startGame() {
  Serial.println(F("[START GAME]"));
  playerX = SCREEN_WIDTH / 2;
  lives = 3;
  score = 0;
  gameStartTime = millis();
  inGame = true;
  rapidFire = false;
  shieldActive = false;
  shieldStartTime = 0;

  for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) enemyBullets[i].active = false;
  for (int i = 0; i < MAX_ENEMY_LASERS; i++) enemyLasers[i].active = false;
  for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
  for (int i = 0; i < MAX_POWERUPS; i++) powerups[i].active = false;
}

// Spawn Enemy
void spawnEnemy() {
  int activeEnemies = 0;
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) activeEnemies++;
  }
  if (activeEnemies >= 6) return;

  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) {
      enemies[i].x = random(0, SCREEN_WIDTH - 8);
      enemies[i].y = 0;
      enemies[i].active = true;
      int randType = random(0, 100);
      if (randType < 40) enemies[i].type = COLLIDER;
      else if (randType < 70) enemies[i].type = BULLET;
      else enemies[i].type = LASER;
      Serial.print(F("Spawned enemy type: ")); Serial.println(enemies[i].type);
      break;
    }
  }
}

// Spawn Enemy Bullet
void spawnEnemyBullet(int x, int y) {
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (!enemyBullets[i].active) {
      enemyBullets[i] = { x + 3, y + 7, true };
      playShootSound();
      Serial.println(F("Enemy bullet spawned"));
      break;
    }
  }
}

// Spawn Enemy Laser
void spawnEnemyLaser(int x, int y) {
  for (int i = 0; i < MAX_ENEMY_LASERS; i++) {
    if (!enemyLasers[i].active) {
      enemyLasers[i] = { x + 3, y + 7, true };
      playShootSound();
      Serial.println(F("Enemy laser spawned"));
      break;
    }
  }
}

// Spawn Power-up
void spawnPowerUp() {
  for (int i = 0; i < MAX_POWERUPS; i++) {
    if (!powerups[i].active) {
      powerups[i].x = random(0, SCREEN_WIDTH - 5);
      powerups[i].y = 0;
      powerups[i].active = true;
      int randType = random(0, 100);
      if (randType < 20) powerups[i].type = BIG_BANG;
      else if (randType < 50) powerups[i].type = EXTRA_LIFE;
      else powerups[i].type = SHIELD;
      Serial.print(F("Spawned power-up type: ")); Serial.println(powerups[i].type);
      break;
    }
  }
}

// Draw Heart Lives
void drawLives() {
  for (int i = 0; i < lives; i++) {
    u8g2.drawXBMP(2 + i * 8, 2, 5, 5, heartBitmap);
  }
}

// Startup Screen
void showStartupScreen() {
  Serial.println(F("Drawing startup screen..."));
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(20, 30, "SPACE SHOOTER");
    u8g2.drawStr(20, 50, "Press FIRE");
  } while (u8g2.nextPage());
  Serial.println(F("[STARTUP SCREEN DISPLAYED]"));
}

// Main Loop
void loop() {
  Serial.print(F("inGame: ")); Serial.println(inGame);
  if (!inGame) {
    if (digitalRead(BTN_FIRE) == LOW) {
      delay(200);
      startGame();
      for (int i = 0; i < 5; i++) spawnEnemy();
    }
    return;
  }

  // Controls
  if (!isPaused) {
    if (digitalRead(BTN_LEFT) == LOW && playerX > 0) playerX -= 2;
    if (digitalRead(BTN_RIGHT) == LOW && playerX < SCREEN_WIDTH - 8) playerX += 2;
    if (digitalRead(BTN_FIRE) == LOW && millis() - lastShot > (rapidFire ? 150 : 400)) {
      for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
          bullets[i] = { playerX + 3, SCREEN_HEIGHT - 10, true };
          lastShot = millis();
          playShootSound();
          Serial.println(F("Player shot fired"));
          break;
        }
      }
    }
  }

  if (digitalRead(BTN_PAUSE) == LOW) {
    isPaused = !isPaused;
    delay(200);
    Serial.println(isPaused ? F("[PAUSED]") : F("[RESUMED]"));
  }

  // Game Logic
  if (!isPaused) {
    if (shieldActive && millis() - shieldStartTime > SHIELD_DURATION) {
      shieldActive = false;
      Serial.println(F("[SHIELD DEACTIVATED]"));
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
      if (bullets[i].active) {
        bullets[i].y -= 3;
        if (bullets[i].y < 0) bullets[i].active = false;
      }
    }

    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
      if (enemyBullets[i].active) {
        enemyBullets[i].y += 3;
        if (enemyBullets[i].y > SCREEN_HEIGHT) enemyBullets[i].active = false;
      }
    }

    for (int i = 0; i < MAX_ENEMY_LASERS; i++) {
      if (enemyLasers[i].active) {
        enemyLasers[i].y += 2;
        if (enemyLasers[i].y > SCREEN_HEIGHT) enemyLasers[i].active = false;
      }
    }

    for (int i = 0; i < MAX_POWERUPS; i++) {
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
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
          enemies[i].y += 4;
          if (enemies[i].y > SCREEN_HEIGHT) enemies[i].active = false;
          if (enemies[i].type == BULLET && millis() - lastEnemyShot > 1000 && random(0, 100) < 10) {
            spawnEnemyBullet(enemies[i].x, enemies[i].y);
            lastEnemyShot = millis();
          }
          if (enemies[i].type == LASER && millis() - lastEnemyShot > 1500 && random(0, 100) < 8) {
            spawnEnemyLaser(enemies[i].x, enemies[i].y);
            lastEnemyShot = millis();
          }
        } else {
          int activeEnemies = 0;
          for (int j = 0; j < MAX_ENEMIES; j++) {
            if (enemies[j].active) activeEnemies++;
          }
          if (activeEnemies < 5) spawnEnemy();
        }
      }
      lastEnemyMove = millis();
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (!enemies[i].active) continue;
      for (int j = 0; j < MAX_BULLETS; j++) {
        if (bullets[j].active && abs(enemies[i].x - bullets[j].x) < 8 && abs(enemies[i].y - bullets[j].y) < 8) {
          enemies[i].active = false;
          bullets[j].active = false;
          score++;
          playHitSound();
          saveHighScore();
          Serial.println(F("Enemy hit"));
        }
      }
    }

    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
      if (enemyBullets[i].active && !shieldActive && millis() - gameStartTime > INVINCIBILITY_MS &&
          abs(enemyBullets[i].x - playerX) < 8 && enemyBullets[i].y >= SCREEN_HEIGHT - 10) {
        enemyBullets[i].active = false;
        lives--;
        playDamageSound();
        Serial.println(F("Player hit by bullet"));
        if (lives <= 0) {
          saveHighScore();
          inGame = false;
          showStartupScreen();
          return;
        }
      }
    }

    for (int i = 0; i < MAX_ENEMY_LASERS; i++) {
      if (enemyLasers[i].active && !shieldActive && millis() - gameStartTime > INVINCIBILITY_MS &&
          abs(enemyLasers[i].x - playerX) < 8 && enemyLasers[i].y >= SCREEN_HEIGHT - 10) {
        enemyLasers[i].active = false;
        lives--;
        playDamageSound();
        Serial.println(F("Player hit by laser"));
        if (lives <= 0) {
          saveHighScore();
          inGame = false;
          showStartupScreen();
          return;
        }
      }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (enemies[i].active && !shieldActive && millis() - gameStartTime > INVINCIBILITY_MS &&
          abs(enemies[i].x - playerX) < 8 && enemies[i].y >= SCREEN_HEIGHT - 10) {
        enemies[i].active = false;
        lives--;
        playDamageSound();
        Serial.println(F("Player hit by enemy"));
        if (lives <= 0) {
          saveHighScore();
          inGame = false;
          showStartupScreen();
          return;
        }
      }
    }

    for (int i = 0; i < MAX_POWERUPS; i++) {
      if (powerups[i].active && abs(powerups[i].x - playerX) < 8 && abs(powerups[i].y - (SCREEN_HEIGHT - 8)) < 8) {
        powerups[i].active = false;
        if (powerups[i].type == EXTRA_LIFE && lives < MAX_LIVES) {
          lives++;
          playExtraLifeSound();
          Serial.println(F("[EXTRA LIFE COLLECTED]"));
        } else if (powerups[i].type == SHIELD) {
          shieldActive = true;
          shieldStartTime = millis();
          playShieldSound();
          Serial.println(F("[SHIELD ACTIVATED]"));
        } else if (powerups[i].type == BIG_BANG) {
          for (int j = 0; j < MAX_ENEMIES; j++) enemies[j].active = false;
          for (int j = 0; j < MAX_ENEMY_BULLETS; j++) enemyBullets[j].active = false;
          for (int j = 0; j < MAX_ENEMY_LASERS; j++) enemyLasers[j].active = false;
          score += 5;
          playBigBangSound();
          saveHighScore();
          Serial.println(F("[BIG BANG COLLECTED]"));
        }
      }
    }
  }

  // Draw Frame
  u8g2.firstPage();
  do {
    if (!shieldActive || (millis() % 200 < 100)) {
      u8g2.drawXBMP(playerX, SCREEN_HEIGHT - 8, 7, 7, heroBitmap);
    }
    for (int i = 0; i < MAX_BULLETS; i++) {
      if (bullets[i].active)
        u8g2.drawBox(bullets[i].x, bullets[i].y, 2, 4);
    }
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
      if (enemyBullets[i].active)
        u8g2.drawBox(bullets[i].x, enemyBullets[i].y, 2, 2);
    }
    for (int i = 0; i < MAX_ENEMY_LASERS; i++) {
      if (enemyLasers[i].active)
        u8g2.drawBox(enemyLasers[i].x, enemyLasers[i].y, 2, 8);
    }
    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (enemies[i].active) {
        if (enemies[i].type == COLLIDER)
          u8g2.drawXBMP(enemies[i].x, enemies[i].y, 7, 7, colliderBitmap);
        else if (enemies[i].type == BULLET)
          u8g2.drawXBMP(enemies[i].x, enemies[i].y, 7, 7, bulletEnemyBitmap);
        else if (enemies[i].type == LASER)
          u8g2.drawXBMP(enemies[i].x, enemies[i].y, 7, 7, laserEnemyBitmap);
      }
    }
    for (int i = 0; i < MAX_POWERUPS; i++) {
      if (powerups[i].active) {
        if (powerups[i].type == EXTRA_LIFE)
          u8g2.drawXBMP(powerups[i].x, powerups[i].y, 5, 5, heartBitmap);
        else if (powerups[i].type == SHIELD)
          u8g2.drawXBMP(powerups[i].x, powerups[i].y, 5, 5, shieldBitmap);
        else if (powerups[i].type == BIG_BANG)
          u8g2.drawXBMP(powerups[i].x, powerups[i].y, 5, 5, bigBangBitmap);
      }
    }
    drawLives();
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(80, 10);
    u8g2.print(F("S: ")); u8g2.print(score);
  } while (u8g2.nextPage());
  Serial.print(F("Frame drawn, lives: ")); Serial.print(lives);
  Serial.print(F(", score: ")); Serial.print(score);
  Serial.print(F(", playerX: ")); Serial.println(playerX);

  delay(30);
}