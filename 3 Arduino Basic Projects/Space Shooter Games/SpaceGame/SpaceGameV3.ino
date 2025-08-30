// Space Shooter Game for 1.3-inch SH1106 OLED with Eagle Hero and Squid Enemies
// OLED Address: 0x3C | Controls: LEFT=2, RIGHT=3, FIRE=4, PAUSE=6 | Buzzer=8

#include <U8g2lib.h>
#include <EEPROM.h>
#include <Wire.h>

#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_FIRE 4
#define BUTTON_PAUSE 6
#define BUZZER_PIN 8

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Bitmaps (12x10)
const unsigned char bitmapHero[] PROGMEM = {
  0x10, 0x38, 0x7C, 0xFE, 0xFF, 0xFE, 0x7C, 0x38, 0x10, 0x00,
  0x00, 0x00
};

const unsigned char bitmapEnemy[] PROGMEM = {
  0x38, 0x7C, 0xFE, 0xFE, 0x7C, 0x6D, 0x24, 0x42, 0x81, 0x00,
  0x00, 0x00
};

struct Entity {
  int x, y;
  bool alive;
};

#define MAX_ENEMIES 8
Entity enemies[MAX_ENEMIES];
int enemyCount = 3;
int level = 1;
unsigned long lastEnemyShot = 0;
unsigned long lastFire = 0;
int fireRate = 300;

struct Bullet {
  int x, y;
  bool active;
};
Bullet bullets[5];
Bullet enemyBullets[3];

int heroX = 58;
int heroY = 52;
int score = 0;
int highScore = 0;
bool shieldActive = false;
unsigned long shieldTimer = 0;

bool gamePaused = false;
bool inGame = false;

void setupGame();
void spawnEnemies();
void drawStartup();
void handleInput();
void drawGame();
void updateGame();
void checkCollisions();
void fireBullet();
void fireEnemyBullet();
void playSound(int toneFreq, int duration);
void drawPowerUps();
void resetGame();

void setup() {
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);
  pinMode(BUTTON_FIRE, INPUT);
  pinMode(BUTTON_PAUSE, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  u8g2.begin();
  highScore = EEPROM.read(0);
  drawStartup();
}

void loop() {
  if (!inGame) {
    if (digitalRead(BUTTON_FIRE)) {
      inGame = true;
      setupGame();
    }
    return;
  }

  if (digitalRead(BUTTON_PAUSE)) {
    gamePaused = !gamePaused;
    delay(300);
  }

  if (!gamePaused) {
    handleInput();
    updateGame();
    checkCollisions();
  }

  drawGame();
  delay(40);
}

void drawStartup() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(20, 20, "RIYAZ PATHAN PRESENTS");
  u8g2.drawStr(34, 40, "SPACE SHOOTER");
  u8g2.drawStr(18, 60, "Press FIRE to start...");
  u8g2.sendBuffer();
}

void setupGame() {
  heroX = 58;
  score = 0;
  enemyCount = 3;
  level = 1;
  fireRate = 300;
  shieldActive = false;
  shieldTimer = 0;
  spawnEnemies();
  for (int i = 0; i < 5; i++) bullets[i].active = false;
  for (int i = 0; i < 3; i++) enemyBullets[i].active = false;
}

void spawnEnemies() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    enemies[i].x = random(0, 116);
    enemies[i].y = random(5, 30);
    enemies[i].alive = i < enemyCount;
  }
}

void handleInput() {
  if (digitalRead(BUTTON_LEFT)) heroX = max(0, heroX - 2);
  if (digitalRead(BUTTON_RIGHT)) heroX = min(116, heroX + 2);
  if (digitalRead(BUTTON_FIRE) && millis() - lastFire > fireRate) {
    fireBullet();
    lastFire = millis();
  }
}

void fireBullet() {
  for (int i = 0; i < 5; i++) {
    if (!bullets[i].active) {
      bullets[i] = {heroX + 6, heroY, true};
      playSound(1000, 30);
      break;
    }
  }
}

void fireEnemyBullet() {
  if (millis() - lastEnemyShot < 1000) return;
  lastEnemyShot = millis();
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].alive) {
      for (int j = 0; j < 3; j++) {
        if (!enemyBullets[j].active) {
          enemyBullets[j] = {enemies[i].x + 6, enemies[i].y + 8, true};
          playSound(200, 20);
          return;
        }
      }
    }
  }
}

void updateGame() {
  for (int i = 0; i < 5; i++) {
    if (bullets[i].active) {
      bullets[i].y -= 4;
      if (bullets[i].y < 0) bullets[i].active = false;
    }
  }
  for (int i = 0; i < 3; i++) {
    if (enemyBullets[i].active) {
      enemyBullets[i].y += 3;
      if (enemyBullets[i].y > 64) enemyBullets[i].active = false;
    }
  }
  fireEnemyBullet();

  // Level up
  bool allDead = true;
  for (int i = 0; i < enemyCount; i++) if (enemies[i].alive) allDead = false;
  if (allDead) {
    level++;
    enemyCount = min(enemyCount + 1, MAX_ENEMIES);
    fireRate = max(150, fireRate - 20);
    spawnEnemies();
  }

  if (shieldActive && millis() - shieldTimer > 5000) shieldActive = false;
}

void checkCollisions() {
  for (int i = 0; i < 5; i++) {
    if (bullets[i].active) {
      for (int j = 0; j < MAX_ENEMIES; j++) {
        if (enemies[j].alive && bullets[i].x > enemies[j].x && bullets[i].x < enemies[j].x + 12 &&
            bullets[i].y > enemies[j].y && bullets[i].y < enemies[j].y + 10) {
          bullets[i].active = false;
          enemies[j].alive = false;
          score += 10;
          if (score > highScore) EEPROM.write(0, score);
        }
      }
    }
  }

  for (int i = 0; i < 3; i++) {
    if (enemyBullets[i].active &&
        enemyBullets[i].x > heroX && enemyBullets[i].x < heroX + 12 &&
        enemyBullets[i].y > heroY && enemyBullets[i].y < heroY + 10) {
      if (!shieldActive) {
        drawStartup();
        inGame = false;
        delay(1000);
        return;
      }
      enemyBullets[i].active = false;
    }
  }
}

void drawGame() {
  u8g2.clearBuffer();

  // Draw hero
  u8g2.drawXBMP(heroX, heroY, 12, 10, bitmapHero);

  // Draw bullets
  for (int i = 0; i < 5; i++)
    if (bullets[i].active) u8g2.drawBox(bullets[i].x, bullets[i].y, 2, 4);

  // Draw enemies
  for (int i = 0; i < MAX_ENEMIES; i++)
    if (enemies[i].alive) u8g2.drawXBMP(enemies[i].x, enemies[i].y, 12, 10, bitmapEnemy);

  for (int i = 0; i < 3; i++)
    if (enemyBullets[i].active) u8g2.drawBox(enemyBullets[i].x, enemyBullets[i].y, 2, 4);

  if (shieldActive) u8g2.drawCircle(heroX + 6, heroY + 5, 8);

  // HUD
  u8g2.setFont(u8g2_font_5x8_tf);
  u8g2.setCursor(0, 8);
  u8g2.print("S:" + String(score) + " L:" + String(level));
  u8g2.setCursor(70, 8);
  u8g2.print("HI:" + String(highScore));

  u8g2.sendBuffer();
}

void playSound(int toneFreq, int duration) {
  tone(BUZZER_PIN, toneFreq, duration);
}