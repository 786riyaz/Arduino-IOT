// Arduino Space Shooter with full features
#include <U8g2lib.h>
#include <Wire.h>
#include <EEPROM.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BUTTON_LEFT 2
#define BUTTON_RIGHT 3
#define BUTTON_FIRE 4
#define BUTTON_PAUSE 6
#define BUZZER_PIN 8

#define MAX_ENEMIES 10
#define MAX_BULLETS 5
#define MAX_ENEMY_BULLETS 5
#define MAX_POWERUPS 3

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// Bitmaps (8x8 for simplicity)
const uint8_t playerBitmap[8] = {
  0b00011000,
  0b00111100,
  0b01111110,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00100100,
  0b01000010
};

const uint8_t enemyBitmap[8] = {
  0b01111110,
  0b10111101,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00100100,
  0b01011010,
  0b10100101
};

const uint8_t heartBitmap[8] = {
  0b00000000,
  0b01100110,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00000000
};

const uint8_t shieldBitmap[8] = {
  0b00111100,
  0b01000010,
  0b10100101,
  0b10100101,
  0b10100101,
  0b01000010,
  0b00111100,
  0b00000000
};

const uint8_t boltBitmap[8] = {
  0b00011000,
  0b00011000,
  0b00111100,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00011000,
  0b00000000
};

// Game state
struct Bullet { int x, y; bool active; };
struct Enemy { int x, y; bool active; };
struct EnemyBullet { int x, y; bool active; };
struct PowerUp { int x, y; byte type; bool active; };

int playerX = 60;
bool gamePaused = false;
bool gameStarted = false;
int score = 0;
int highScore = 0;
int lives = 3;
bool shield = false;
bool rapidFire = false;
unsigned long lastShot = 0;
unsigned long lastEnemyShot = 0;
unsigned long lastSpawn = 0;

Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
EnemyBullet enemyBullets[MAX_ENEMY_BULLETS];
PowerUp powerUps[MAX_POWERUPS];

void drawBitmap(int x, int y, const uint8_t* bmp) {
  display.drawXBMP(x, y, 8, 8, bmp);
}

void playTone(int freq, int duration) {
  tone(BUZZER_PIN, freq, duration);
}

void resetGame() {
  playerX = 60;
  lives = 3;
  score = 0;
  shield = false;
  rapidFire = false;
  for (auto& b : bullets) b.active = false;
  for (auto& e : enemies) e.active = false;
  for (auto& eb : enemyBullets) eb.active = false;
  for (auto& p : powerUps) p.active = false;
}

void spawnEnemy() {
  for (auto& e : enemies) {
    if (!e.active) {
      e.x = random(0, SCREEN_WIDTH - 8);
      e.y = 0;
      e.active = true;
      break;
    }
  }
}

void shootBullet() {
  unsigned long now = millis();
  if (now - lastShot > (rapidFire ? 150 : 300)) {
    for (auto& b : bullets) {
      if (!b.active) {
        b.x = playerX + 3;
        b.y = 50;
        b.active = true;
        lastShot = now;
        playTone(1000, 50);
        break;
      }
    }
  }
}

void enemyShoot() {
  unsigned long now = millis();
  if (now - lastEnemyShot > 1000) {
    for (auto& eb : enemyBullets) {
      if (!eb.active) {
        for (auto& e : enemies) {
          if (e.active) {
            eb.x = e.x + 3;
            eb.y = e.y + 8;
            eb.active = true;
            lastEnemyShot = now;
            break;
          }
        }
        break;
      }
    }
  }
}

void drawStartup() {
  display.clearBuffer();
  display.setFont(u8g2_font_ncenB08_tr);
  display.drawStr(15, 20, "RIYAZ PATHAN");
  display.drawStr(10, 40, "SPACE SHOOTER");
  display.drawStr(5, 60, "Press FIRE to start");
  display.sendBuffer();
}

void saveHighScore() {
  EEPROM.update(0, highScore / 256);
  EEPROM.update(1, highScore % 256);
}

void loadHighScore() {
  highScore = EEPROM.read(0) * 256 + EEPROM.read(1);
}

void setup() {
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);
  pinMode(BUTTON_FIRE, INPUT);
  pinMode(BUTTON_PAUSE, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  display.begin();
  display.clear();
  loadHighScore();
  drawStartup();
  while (!gameStarted) {
    if (digitalRead(BUTTON_FIRE) == HIGH) {
      gameStarted = true;
      delay(200);
    }
  }
  resetGame();
}

void loop() {
  if (digitalRead(BUTTON_PAUSE) == HIGH) {
    gamePaused = !gamePaused;
    delay(300);
  }
  if (gamePaused) return;

  // Input
  if (digitalRead(BUTTON_LEFT) == HIGH) playerX = max(0, playerX - 2);
  if (digitalRead(BUTTON_RIGHT) == HIGH) playerX = min(SCREEN_WIDTH - 8, playerX + 2);
  if (digitalRead(BUTTON_FIRE) == HIGH) shootBullet();

  // Spawn enemies
  if (millis() - lastSpawn > 1000 && random(0, 10) > 5) {
    spawnEnemy();
    lastSpawn = millis();
  }

  enemyShoot();

  // Update bullets
  for (auto& b : bullets) if (b.active) { b.y -= 4; if (b.y < 0) b.active = false; }
  for (auto& eb : enemyBullets) if (eb.active) { eb.y += 3; if (eb.y > SCREEN_HEIGHT) eb.active = false; }

  // Update enemies
  for (auto& e : enemies) if (e.active) {
    e.y += 1;
    if (e.y > SCREEN_HEIGHT) e.active = false;
  }

  // Collisions
  for (auto& b : bullets) {
    if (!b.active) continue;
    for (auto& e : enemies) {
      if (e.active && b.x < e.x + 8 && b.x + 2 > e.x && b.y < e.y + 8 && b.y + 4 > e.y) {
        e.active = false; b.active = false; score += 10;
        playTone(1500, 50);
      }
    }
  }
  for (auto& eb : enemyBullets) {
    if (!eb.active) continue;
    if (eb.x < playerX + 8 && eb.x + 2 > playerX && eb.y < 56 + 8 && eb.y + 4 > 56) {
      eb.active = false;
      if (!shield) lives--;
      else shield = false;
      playTone(300, 100);
    }
  }

  // Draw everything
  display.clearBuffer();
  drawBitmap(playerX, 56, playerBitmap);
  for (auto& b : bullets) if (b.active) display.drawBox(b.x, b.y, 2, 4);
  for (auto& eb : enemyBullets) if (eb.active) display.drawBox(eb.x, eb.y, 2, 4);
  for (auto& e : enemies) if (e.active) drawBitmap(e.x, e.y, enemyBitmap);

  display.setFont(u8g2_font_5x7_tr);
  display.setCursor(0, 10); display.print("Score: "); display.print(score);
  display.setCursor(80, 10); display.print("Lives: "); display.print(lives);

  if (lives <= 0) {
    if (score > highScore) { highScore = score; saveHighScore(); }
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.drawStr(30, 30, "GAME OVER");
    display.setCursor(10, 50); display.print("High Score: "); display.print(highScore);
    display.sendBuffer();
    delay(3000);
    resetGame();
  }

  display.sendBuffer();
  delay(30);
}