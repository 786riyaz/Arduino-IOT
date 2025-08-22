#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// === TFT PINS (Change if needed) ===
#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// === COLORS ===
#define COLOR_PLAYER  ST77XX_CYAN
#define COLOR_BULLET  ST77XX_YELLOW
#define COLOR_ENEMY   ST77XX_RED
#define COLOR_BG      ST77XX_WHITE

// === SCREEN SIZE ===
#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 128

// === PLAYER ===
int playerX = SCREEN_WIDTH / 2;
int oldPlayerX = playerX;

// === BULLETS ===
#define MAX_BULLETS 5
struct Bullet {
  int x, y;
  bool active;
};
Bullet bullets[MAX_BULLETS];

// === ENEMIES ===
#define MAX_ENEMIES 5
struct Enemy {
  int x, y;
  bool active;
};
Enemy enemies[MAX_ENEMIES];

// === BITMAPS (1-bit) ===
const unsigned char PROGMEM heroBitmap[] = {
  0b00111000,
  0b01111100,
  0b11111110,
  0b11111110,
  0b01111100,
  0b00111000,
  0b00010000
}; // 7x7 Eagle-like

const unsigned char PROGMEM enemyBitmap[] = {
  0b00111000,
  0b01111100,
  0b11111110,
  0b11111110,
  0b01111100,
  0b00111000,
  0b00111000
}; // 7x7 Squid-like

// === INPUT PINS ===
#define LEFT_BTN   2
#define RIGHT_BTN  3
#define FIRE_BTN   4

// === FUNCTIONS ===
void fireBullet() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) {
      bullets[i].x = playerX + 3;
      bullets[i].y = SCREEN_HEIGHT - 12;
      bullets[i].active = true;
      return;
    }
  }
}

void spawnEnemy() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) {
      enemies[i].x = random(0, SCREEN_WIDTH - 7);
      enemies[i].y = 0;
      enemies[i].active = true;
      return;
    }
  }
}

// === SETUP ===
void setup() {
  pinMode(LEFT_BTN, INPUT_PULLUP);
  pinMode(RIGHT_BTN, INPUT_PULLUP);
  pinMode(FIRE_BTN, INPUT_PULLUP);

  tft.initR(INITR_REDTAB);
  tft.setRotation(1);
  tft.fillScreen(COLOR_BG);

  randomSeed(analogRead(A0));
}

// === LOOP ===
void loop() {
  // ---- Erase Old Player ----
  tft.drawBitmap(oldPlayerX, SCREEN_HEIGHT - 8, heroBitmap, 7, 7, COLOR_BG, COLOR_BG);

  // ---- Controls ----
  if (!digitalRead(LEFT_BTN) && playerX > 0) playerX -= 2;
  if (!digitalRead(RIGHT_BTN) && playerX < SCREEN_WIDTH - 7) playerX += 2;
  if (!digitalRead(FIRE_BTN)) fireBullet();

  // ---- Draw Player ----
  tft.drawBitmap(playerX, SCREEN_HEIGHT - 8, heroBitmap, 7, 7, COLOR_PLAYER, COLOR_BG);
  oldPlayerX = playerX;

  // ---- Bullets ----
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      // erase old
      tft.fillRect(bullets[i].x, bullets[i].y, 2, 4, COLOR_BG);
      // move
      bullets[i].y -= 4;
      if (bullets[i].y <= 0) {
        bullets[i].active = false;
      } else {
        // draw new
        tft.fillRect(bullets[i].x, bullets[i].y, 2, 4, COLOR_BULLET);
      }
    }
  }

  // ---- Enemies ----
  if (random(0, 40) == 0) spawnEnemy();

  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      // erase old
      tft.drawBitmap(enemies[i].x, enemies[i].y, enemyBitmap, 7, 7, COLOR_BG, COLOR_BG);

      // move
      enemies[i].y += 1;
      if (enemies[i].y >= SCREEN_HEIGHT - 8) {
        enemies[i].active = false;
      } else {
        // check collision with bullets
        for (int j = 0; j < MAX_BULLETS; j++) {
          if (bullets[j].active &&
              bullets[j].x >= enemies[i].x && bullets[j].x <= enemies[i].x + 7 &&
              bullets[j].y >= enemies[i].y && bullets[j].y <= enemies[i].y + 7) {
            // erase enemy
            tft.drawBitmap(enemies[i].x, enemies[i].y, enemyBitmap, 7, 7, COLOR_BG, COLOR_BG);
            enemies[i].active = false;
            bullets[j].active = false;
          }
        }
      }

      // draw new
      if (enemies[i].active) {
        tft.drawBitmap(enemies[i].x, enemies[i].y, enemyBitmap, 7, 7, COLOR_ENEMY, COLOR_BG);
      }
    }
  }

  delay(30); // control speed
}
