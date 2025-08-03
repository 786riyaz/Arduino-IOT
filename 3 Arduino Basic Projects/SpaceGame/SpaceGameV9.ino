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
#define MAX_ENEMY_BULLETS 5
#define MAX_ENEMIES 5
#define MAX_POWERUPS 2
#define INVINCIBILITY_MS 1500

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Bitmaps
const uint8_t PROGMEM eagleBitmap[] = {
  0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10
};
const uint8_t PROGMEM squidBitmap[] = {
  0x38, 0x7C, 0x54, 0xFE, 0x54, 0x7C, 0x38
};
const uint8_t PROGMEM heartBitmap[] = {
  0x0A, 0x1F, 0x1F, 0x0E, 0x04
};

// Structures
struct Entity {
  int x, y;
  bool active;
};

Entity bullets[MAX_BULLETS];
Entity enemyBullets[MAX_ENEMY_BULLETS];
Entity enemies[MAX_ENEMIES];
Entity powerups[MAX_POWERUPS];

int playerX = SCREEN_WIDTH / 2;
int lives = 3;
int score = 0;
bool rapidFire = false;
unsigned long lastShot = 0;
unsigned long lastEnemyMove = 0;
unsigned long lastPowerup = 0;
unsigned long lastEnemyShot = 0;
unsigned long gameStartTime = 0;
int highScore = 0;
bool isPaused = false;
bool inGame = false;

// Setup
void setup() {
  u8g2.begin();
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_FIRE, INPUT_PULLUP);
  pinMode(BTN_PAUSE, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);
  // Read high score from EEPROM (2 bytes)
  highScore = (EEPROM.read(0) << 8) | EEPROM.read(1);
  showStartupScreen();
}

// Beep
void beep(int duration = 50) {
  digitalWrite(BUZZER, HIGH);
  delay(duration);
  digitalWrite(BUZZER, LOW);
}

// Save high score (2 bytes)
void saveHighScore() {
  if (score > highScore) {
    highScore = score;
    EEPROM.write(0, highScore >> 8); // High byte
    EEPROM.write(1, highScore & 0xFF); // Low byte
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

  for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) enemyBullets[i].active = false;
  for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
  for (int i = 0; i < MAX_POWERUPS; i++) powerups[i].active = false;
}

// Spawn Enemy
void spawnEnemy() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) {
      enemies[i].x = random(0, SCREEN_WIDTH - 8);
      enemies[i].y = 0;
      enemies[i].active = true;
      break;
    }
  }
}

// Spawn Enemy Bullet
void spawnEnemyBullet(int x, int y) {
  for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
    if (!enemyBullets[i].active) {
      enemyBullets[i] = { x + 3, y + 7, true };
      beep(15);
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
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(20, 20, "RIYAZ PATHAN");
    u8g2.drawStr(10, 40, "SPACE SHOOTER");
    u8g2.setCursor(15, 60);
    u8g2.print(F("High Score: ")); u8g2.print(highScore);
  } while (u8g2.nextPage());
  Serial.println(F("[STARTUP SCREEN DISPLAYED]"));
}

// Main Loop
void loop() {
  if (!inGame) {
    if (digitalRead(BTN_FIRE) == LOW) {
      delay(200);
      startGame();
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
          beep(20);
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
    // Move player bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
      if (bullets[i].active) {
        bullets[i].y -= 3;
        if (bullets[i].y < 0) bullets[i].active = false;
      }
    }

    // Move enemy bullets
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
      if (enemyBullets[i].active) {
        enemyBullets[i].y += 3;
        if (enemyBullets[i].y > SCREEN_HEIGHT) enemyBullets[i].active = false;
      }
    }

    // Enemy movement and shooting
    if (millis() - lastEnemyMove > 500) {
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
          enemies[i].y += 4;
          if (enemies[i].y > SCREEN_HEIGHT) enemies[i].active = false;
          // Enemy shooting (random chance)
          if (millis() - lastEnemyShot > 1000 && random(0, 100) < 10) {
            spawnEnemyBullet(enemies[i].x, enemies[i].y);
            lastEnemyShot = millis();
          }
        } else if (random(0, 100) < 30) {
          spawnEnemy();
        }
      }
      lastEnemyMove = millis();
    }

    // Collision: player bullet vs enemy
    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (!enemies[i].active) continue;
      for (int j = 0; j < MAX_BULLETS; j++) {
        if (bullets[j].active && abs(enemies[i].x - bullets[j].x) < 8 && abs(enemies[i].y - bullets[j].y) < 8) {
          enemies[i].active = false;
          bullets[j].active = false;
          score++;
          beep(30);
          saveHighScore();
        }
      }
    }

    // Collision: enemy bullet vs player
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
      if (enemyBullets[i].active && millis() - gameStartTime > INVINCIBILITY_MS &&
          abs(enemyBullets[i].x - playerX) < 8 && enemyBullets[i].y >= SCREEN_HEIGHT - 10) {
        enemyBullets[i].active = false;
        lives--;
        beep(100);
        if (lives <= 0) {
          saveHighScore();
          inGame = false;
          showStartupScreen();
          return;
        }
      }
    }

    // Collision: enemy vs player
    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (enemies[i].active && millis() - gameStartTime > INVINCIBILITY_MS &&
          abs(enemies[i].x - playerX) < 8 && enemies[i].y >= SCREEN_HEIGHT - 10) {
        enemies[i].active = false;
        lives--;
        beep(100);
        if (lives <= 0) {
          saveHighScore();
          inGame = false;
          showStartupScreen();
          return;
        }
      }
    }
  }

  // Draw Frame
  u8g2.firstPage();
  do {
    u8g2.drawXBMP(playerX, SCREEN_HEIGHT - 8, 7, 7, eagleBitmap);
    // Draw player bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
      if (bullets[i].active)
        u8g2.drawBox(bullets[i].x, bullets[i].y, 2, 4);
    }
    // Draw enemy bullets
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
      if (enemyBullets[i].active)
        u8g2.drawBox(enemyBullets[i].x, enemyBullets[i].y, 2, 2); // Smaller bullet for enemies
    }
    // Draw enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (enemies[i].active)
        u8g2.drawXBMP(enemies[i].x, enemies[i].y, 7, 7, squidBitmap);
    }
    drawLives();
    u8g2.setCursor(100, 10);
    u8g2.print(F("S:")); u8g2.print(score);
  } while (u8g2.nextPage());

  delay(30);
}