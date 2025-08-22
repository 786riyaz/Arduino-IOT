/*
  SPACE SHOOTER (TFT ST7735, WHITE BG, NO FLICKER)
  - Panel: ST7735 (INITR_BLACKTAB confirmed by your tests)
  - Libraries: Adafruit_GFX, Adafruit_ST7735, SPI, EEPROM
  - Pins: Change to match your wiring if needed
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <EEPROM.h>

// ---------- TFT PINS ----------
#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

// ---------- INPUTS / BUZZER ----------
#define BTN_LEFT   2
#define BTN_RIGHT  3
#define BTN_FIRE   4
#define BTN_PAUSE  6
#define BUZZER     7

// ---------- GAME LIMITS ----------
#define MAX_BULLETS          3
#define MAX_ENEMY_BULLETS    3
#define MAX_ENEMY_LASERS     1
#define MAX_ENEMIES          6
#define MAX_POWERUPS         2
#define INVINCIBILITY_MS     1500UL
#define SHIELD_DURATION_MS   5000UL
#define MAX_LIVES            10

// Colors
#define ST77XX_DARKGREY 0x7BEF  // R:123 G:123 B:123
#define ST77XX_PURPLE   0x780F  // R:120 G:0   B:240

// Extra custom colors not in Adafruit ST77XX
#define ST77XX_DARKGREY 0x7BEF  
#define ST77XX_PURPLE   0x780F  

// Will be set after init (rotation)
int16_t SCREEN_W, SCREEN_H;

enum EnemyType    { E_COLLIDER, E_BULLET, E_LASER };
enum PowerUpType  { P_NONE, P_EXTRA_LIFE, P_SHIELD, P_BIG_BANG };

struct Sprite {
  int16_t x, y, w, h;
  int16_t px, py;     // previous position (for erasing)
  bool active;
};

struct Enemy {
  int16_t x, y, w, h;
  int16_t px, py;
  bool active;
  EnemyType type;
};

struct PowerUp {
  int16_t x, y, w, h;
  int16_t px, py;
  bool active;
  PowerUpType type;
};

// ---------- ARRAYS ----------
Sprite bullets[MAX_BULLETS];
Sprite enemyBullets[MAX_ENEMY_BULLETS];
Sprite enemyLasers[MAX_ENEMY_LASERS];
Enemy  enemies[MAX_ENEMIES];
PowerUp powerups[MAX_POWERUPS];

// ---------- GAME STATE ----------
int16_t playerX;
int16_t playerY;
const int16_t playerW = 9;
const int16_t playerH = 9;

int8_t  lives = 3;
uint16_t score = 0;
uint16_t highScore = 0;

bool rapidFire   = false;        // you have this in OLED; kept for parity (unused toggle)
bool shieldActive = false;
bool isPaused     = false;
bool inGame       = false;

unsigned long lastShot          = 0;
unsigned long lastEnemyMove     = 0;
unsigned long lastPowerupSpawn  = 0;
unsigned long lastEnemyShot     = 0;
unsigned long gameStartTime     = 0;
unsigned long shieldStartTime   = 0;

// ---------- COLORS ----------
const uint16_t BG         = ST77XX_WHITE;     // keep white for vibrancy
const uint16_t COL_TEXT   = ST77XX_BLACK;
const uint16_t COL_PLAYER = ST77XX_BLUE;
const uint16_t COL_BULLET = ST77XX_BLACK;
const uint16_t COL_EN_C   = ST77XX_RED;       // Collider enemy
const uint16_t COL_EN_B   = ST77XX_ORANGE;    // Bullet enemy
const uint16_t COL_EN_L   = ST77XX_MAGENTA;   // Laser enemy
const uint16_t COL_EB     = ST77XX_DARKGREY;  // enemy bullet
const uint16_t COL_EL     = ST77XX_PURPLE;    // enemy laser
const uint16_t COL_HP     = ST77XX_RED;       // heart
const uint16_t COL_SHIELD = ST77XX_CYAN;
const uint16_t COL_BANG   = ST77XX_GREEN;

// ---------- UTILS ----------
inline void beep(uint16_t f, uint16_t d){ tone(BUZZER, f, d); }
inline void playShootSound(){ beep(800,20); }
inline void playHitSound(){ beep(500,30); }
inline void playDamageSound(){ beep(200,100); }
inline void playExtraLifeSound(){ beep(1000,50); delay(60); beep(1200,50); delay(60); beep(1400,100); }
inline void playShieldSound(){ beep(600,50); delay(60); beep(800,50); delay(60); beep(1000,100); }
inline void playBigBangSound(){ beep(400,100); delay(120); beep(600,100); delay(120); beep(800,200); }

// Draw/erase helpers (no flicker: overwrite old with BG only where needed)
void eraseRect(int16_t x, int16_t y, int16_t w, int16_t h){
  tft.fillRect(x, y, w, h, BG);
}

// Draw tiny heart icon
void drawHeart(int16_t x, int16_t y){
  // 7x6 heart using small blocks
  tft.drawPixel(x+2, y+1, COL_HP); tft.drawPixel(x+3, y+1, COL_HP); tft.drawPixel(x+4, y+1, COL_HP);
  tft.drawPixel(x+1, y+2, COL_HP); tft.drawPixel(x+2, y+2, COL_HP); tft.drawPixel(x+3, y+2, COL_HP); tft.drawPixel(x+4, y+2, COL_HP); tft.drawPixel(x+5, y+2, COL_HP);
  tft.drawPixel(x+1, y+3, COL_HP); tft.drawPixel(x+2, y+3, COL_HP); tft.drawPixel(x+3, y+3, COL_HP); tft.drawPixel(x+4, y+3, COL_HP); tft.drawPixel(x+5, y+3, COL_HP);
  tft.drawPixel(x+2, y+4, COL_HP); tft.drawPixel(x+3, y+4, COL_HP); tft.drawPixel(x+4, y+4, COL_HP);
  tft.drawPixel(x+3, y+5, COL_HP);
}

// Player: small triangle ship
void drawPlayer(int16_t x, int16_t y){
  tft.fillTriangle(x, y+playerH-1, x+playerW-1, y+playerH-1, x+playerW/2, y, COL_PLAYER);
  if (shieldActive){
    // outline to show shield
    tft.drawRect(x-1, y-1, playerW+2, playerH+2, COL_SHIELD);
  }
}

// Enemies (three styles)
void drawEnemy(const Enemy &e){
  switch(e.type){
    case E_COLLIDER:
      tft.fillRect(e.x, e.y, e.w, e.h, COL_EN_C);
      break;
    case E_BULLET:
      tft.fillRoundRect(e.x, e.y, e.w, e.h, 2, COL_EN_B);
      break;
    case E_LASER:
      tft.drawRect(e.x, e.y, e.w, e.h, COL_EN_L);
      tft.drawFastHLine(e.x, e.y + e.h/2, e.w, COL_EN_L);
      break;
  }
}

// Powerups
void drawPowerUp(const PowerUp &p){
  switch(p.type){
    case P_EXTRA_LIFE:
      drawHeart(p.x, p.y);
      break;
    case P_SHIELD:
      tft.drawCircle(p.x+3, p.y+3, 3, COL_SHIELD);
      tft.drawPixel(p.x+3, p.y+3, COL_SHIELD);
      break;
    case P_BIG_BANG:
      // starburst
      tft.drawFastHLine(p.x, p.y+3, 7, COL_BANG);
      tft.drawFastVLine(p.x+3, p.y, 7, COL_BANG);
      tft.drawLine(p.x, p.y, p.x+6, p.y+6, COL_BANG);
      tft.drawLine(p.x+6, p.y, p.x, p.y+6, COL_BANG);
      break;
    default: break;
  }
}

// Projectiles
void drawBullet(const Sprite &b){
  tft.fillRect(b.x, b.y, b.w, b.h, COL_BULLET);
}
void drawEnemyBullet(const Sprite &b){
  tft.fillRect(b.x, b.y, b.w, b.h, COL_EB);
}
void drawEnemyLaser(const Sprite &l){
  tft.fillRect(l.x, l.y, l.w, l.h, COL_EL);
}

// ---------- PERSIST HIGH SCORE ----------
void saveHighScore(){
  if (score > highScore){
    highScore = score;
    EEPROM.update(0, highScore >> 8);
    EEPROM.update(1, highScore & 0xFF);
  }
}

// ---------- INITIALIZE / RESET ----------
void clearAll(){
  for (uint8_t i=0;i<MAX_BULLETS;i++) bullets[i].active = false;
  for (uint8_t i=0;i<MAX_ENEMY_BULLETS;i++) enemyBullets[i].active = false;
  for (uint8_t i=0;i<MAX_ENEMY_LASERS;i++) enemyLasers[i].active = false;
  for (uint8_t i=0;i<MAX_ENEMIES;i++) enemies[i].active = false;
  for (uint8_t i=0;i<MAX_POWERUPS;i++) powerups[i].active = false;
}

void startGame(){
  tft.fillScreen(BG);

  // UI header
  tft.setTextSize(1);
  tft.setTextColor(COL_TEXT, BG);
  tft.setCursor(2, 2);
  tft.print("S:");
  tft.print(score);
  tft.setCursor(70, 2);
  tft.print("HI:");
  tft.print(highScore);

  // Player bottom center
  playerX = SCREEN_W/2 - playerW/2;
  playerY = SCREEN_H - playerH - 4;

  lives = 3;
  score = 0;
  inGame = true;
  isPaused = false;
  shieldActive = false;
  shieldStartTime = 0;
  gameStartTime = millis();

  clearAll();

  // Spawn a few to start
  for (uint8_t i=0; i<5; i++){
    // will be spawned by spawnEnemy()
    // do it directly:
    for (uint8_t j=0; j<MAX_ENEMIES; j++){
      if (!enemies[j].active){
        enemies[j].w = 10; enemies[j].h = 10;
        enemies[j].x = random(0, SCREEN_W - enemies[j].w);
        enemies[j].y = random(14, 30);
        enemies[j].px = enemies[j].x; enemies[j].py = enemies[j].y;
        enemies[j].active = true;
        uint8_t r = random(100);
        enemies[j].type = (r<40) ? E_COLLIDER : (r<70) ? E_BULLET : E_LASER;
        break;
      }
    }
  }

  // Draw initial
  drawPlayer(playerX, playerY);
  for (uint8_t i=0;i<MAX_ENEMIES;i++) if (enemies[i].active) drawEnemy(enemies[i]);
  drawLives();
}

void showStartupScreen(){
  tft.fillScreen(BG);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.setTextColor(COL_TEXT, BG);

  String author = "RIYAZ PATHAN";
  String game   = "SPACE SHOOTER";
  String prompt = "Press FIRE";

  int16_t xAuthor = (SCREEN_W - (int16_t)author.length()*12)/2;
  int16_t xGame   = (SCREEN_W - (int16_t)game.length()*12)/2;
  int16_t xPrompt = (SCREEN_W - (int16_t)prompt.length()*12)/2;

  tft.setCursor(xAuthor<0?0:xAuthor, 30);
  tft.print(author);
  tft.setCursor(xGame<0?0:xGame, 55);
  tft.print(game);
  tft.setCursor(xPrompt<0?0:xPrompt, 90);
  tft.print(prompt);

  // High score line
  tft.setTextSize(1);
  tft.setCursor(4, 4);
  tft.print("HI: ");
  tft.print(highScore);
}

void drawLives(){
  // Erase lives bar area
  tft.fillRect(SCREEN_W- (MAX_LIVES*8+6), 0, MAX_LIVES*8+6, 12, BG);
  for (uint8_t i=0;i<lives;i++){
    drawHeart(SCREEN_W- (i+1)*8 - 2, 2);
  }
}

// ---------- SPAWN ----------
void spawnEnemy(){
  uint8_t active=0;
  for(uint8_t i=0;i<MAX_ENEMIES;i++) if (enemies[i].active) active++;
  if (active >= MAX_ENEMIES) return;

  for (uint8_t i=0;i<MAX_ENEMIES;i++){
    if (!enemies[i].active){
      enemies[i].w=10; enemies[i].h=10;
      enemies[i].x = random(0, SCREEN_W - enemies[i].w);
      enemies[i].y = 14;
      enemies[i].px = enemies[i].x; enemies[i].py = enemies[i].y;
      enemies[i].active = true;
      uint8_t r = random(100);
      enemies[i].type = (r<40) ? E_COLLIDER : (r<70) ? E_BULLET : E_LASER;
      drawEnemy(enemies[i]);
      break;
    }
  }
}

void spawnEnemyBullet(int16_t x, int16_t y){
  for (uint8_t i=0;i<MAX_ENEMY_BULLETS;i++){
    if (!enemyBullets[i].active){
      enemyBullets[i].w=2; enemyBullets[i].h=4;
      enemyBullets[i].x = x + 4; enemyBullets[i].y = y + 10;
      enemyBullets[i].px = enemyBullets[i].x; enemyBullets[i].py = enemyBullets[i].y;
      enemyBullets[i].active = true;
      playShootSound();
      drawEnemyBullet(enemyBullets[i]);
      break;
    }
  }
}

void spawnEnemyLaser(int16_t x, int16_t y){
  for (uint8_t i=0;i<MAX_ENEMY_LASERS;i++){
    if (!enemyLasers[i].active){
      enemyLasers[i].w=3; enemyLasers[i].h=10;
      enemyLasers[i].x = x + 4; enemyLasers[i].y = y + 10;
      enemyLasers[i].px = enemyLasers[i].x; enemyLasers[i].py = enemyLasers[i].y;
      enemyLasers[i].active = true;
      playShootSound();
      drawEnemyLaser(enemyLasers[i]);
      break;
    }
  }
}

void spawnPowerUp(){
  for (uint8_t i=0;i<MAX_POWERUPS;i++){
    if (!powerups[i].active){
      powerups[i].w=7; powerups[i].h=7;
      powerups[i].x = random(0, SCREEN_W - powerups[i].w);
      powerups[i].y = 14;
      powerups[i].px = powerups[i].x; powerups[i].py = powerups[i].y;
      powerups[i].active = true;
      uint8_t r = random(100);
      powerups[i].type = (r<20) ? P_BIG_BANG : (r<50) ? P_EXTRA_LIFE : P_SHIELD;
      drawPowerUp(powerups[i]);
      break;
    }
  }
}

// ---------- SETUP ----------
void setup(){
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_FIRE, INPUT_PULLUP);
  pinMode(BTN_PAUSE, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  tft.initR(INITR_BLACKTAB);   // Your panel is BLACKTAB
  tft.setRotation(1);          // Landscape
  tft.fillScreen(BG);

  SCREEN_W = tft.width();      // 160 on rotation(1)
  SCREEN_H = tft.height();     // 128 on rotation(1)

  // Load high score
  highScore = (EEPROM.read(0) << 8) | EEPROM.read(1);

  randomSeed(analogRead(A0));

  showStartupScreen();
}

// ---------- INPUT ----------
inline bool btnL(){ return digitalRead(BTN_LEFT)==LOW; }
inline bool btnR(){ return digitalRead(BTN_RIGHT)==LOW; }
inline bool btnF(){ return digitalRead(BTN_FIRE)==LOW; }
inline bool btnP(){ return digitalRead(BTN_PAUSE)==LOW; }

// ---------- GAME LOOP HELPERS ----------
void updateUIHeader(){
  // Score (left), High score (mid), lives drawn separately
  tft.fillRect(0,0, 64,12, BG);
  tft.setTextSize(1);
  tft.setTextColor(COL_TEXT, BG);
  tft.setCursor(2,2);
  tft.print("S:");
  tft.print(score);

  tft.fillRect(64,0, 48,12, BG);
  tft.setCursor(66,2);
  tft.print("HI:");
  tft.print(highScore);

  drawLives();
}

void gameOverScreen(){
  tft.fillScreen(BG);
  tft.setTextColor(COL_TEXT, BG);
  tft.setTextSize(2);
  tft.setCursor(20, 40);
  tft.print("GAME OVER");
  tft.setTextSize(1);
  tft.setCursor(20, 70);
  tft.print("Score: ");
  tft.print(score);
  tft.setCursor(20, 86);
  tft.print("High : ");
  tft.print(highScore);
  tft.setCursor(20, 110);
  tft.print("Press FIRE...");
}

// ---------- LOOP ----------
void loop(){
  if (!inGame){
    if (btnF()){
      delay(200);
      startGame();
    }
    return;
  }

  // Pause toggle
  static bool pauseLatch = false;
  if (btnP() && !pauseLatch){
    isPaused = !isPaused;
    pauseLatch = true;
    // show pause banner
    tft.fillRect(SCREEN_W/2-24, 0, 48, 12, BG);
    tft.setTextSize(1);
    tft.setTextColor(COL_TEXT, BG);
    tft.setCursor(SCREEN_W/2-20, 2);
    tft.print(isPaused ? "PAUSE" : "PLAY ");
  } else if (!btnP()){
    pauseLatch = false;
  }

  if (isPaused){ delay(20); return; }

  // -------- INPUT: MOVE & SHOOT --------
  // Erase old player
  eraseRect(playerX-1, playerY-1, playerW+2, playerH+2);

  if (btnL() && playerX>0) playerX -= 2;
  if (btnR() && playerX<SCREEN_W-playerW) playerX += 2;

  // FIRE (rate limit similar to OLED)
  unsigned long now = millis();
  uint16_t fireDelay = rapidFire ? 150 : 400;
  if (btnF() && (now - lastShot > fireDelay)){
    for (uint8_t i=0;i<MAX_BULLETS;i++){
      if (!bullets[i].active){
        bullets[i].w=2; bullets[i].h=6;
        bullets[i].x = playerX + playerW/2 - bullets[i].w/2;
        bullets[i].y = playerY - bullets[i].h;
        bullets[i].px = bullets[i].x; bullets[i].py = bullets[i].y;
        bullets[i].active = true;
        playShootSound();
        break;
      }
    }
    lastShot = now;
  }

  // Redraw player (with shield outline if active)
  drawPlayer(playerX, playerY);

  // -------- TIMERS --------
  if (shieldActive && (now - shieldStartTime > SHIELD_DURATION_MS)){
    // erase shield outline by redrawing player area
    shieldActive = false;
    eraseRect(playerX-1, playerY-1, playerW+2, playerH+2);
    drawPlayer(playerX, playerY);
  }

  // -------- MOVE PLAYER BULLETS --------
  for (uint8_t i=0;i<MAX_BULLETS;i++){
    if (!bullets[i].active) continue;
    // erase old
    eraseRect(bullets[i].x, bullets[i].y, bullets[i].w, bullets[i].h);
    bullets[i].py = bullets[i].y;
    bullets[i].y -= 3;
    if (bullets[i].y < 12){ bullets[i].active=false; continue; }
    drawBullet(bullets[i]);
  }

  // -------- MOVE ENEMY BULLETS --------
  for (uint8_t i=0;i<MAX_ENEMY_BULLETS;i++){
    if (!enemyBullets[i].active) continue;
    eraseRect(enemyBullets[i].x, enemyBullets[i].y, enemyBullets[i].w, enemyBullets[i].h);
    enemyBullets[i].y += 3;
    if (enemyBullets[i].y > SCREEN_H-1){ enemyBullets[i].active=false; continue; }
    drawEnemyBullet(enemyBullets[i]);
  }

  // -------- MOVE ENEMY LASERS --------
  for (uint8_t i=0;i<MAX_ENEMY_LASERS;i++){
    if (!enemyLasers[i].active) continue;
    eraseRect(enemyLasers[i].x, enemyLasers[i].y, enemyLasers[i].w, enemyLasers[i].h);
    enemyLasers[i].y += 2;
    if (enemyLasers[i].y > SCREEN_H-1){ enemyLasers[i].active=false; continue; }
    drawEnemyLaser(enemyLasers[i]);
  }

  // -------- MOVE POWERUPS --------
  for (uint8_t i=0;i<MAX_POWERUPS;i++){
    if (!powerups[i].active) continue;
    eraseRect(powerups[i].x-1, powerups[i].y-1, powerups[i].w+2, powerups[i].h+2);
    powerups[i].y += 1;
    if (powerups[i].y > SCREEN_H-1){ powerups[i].active=false; continue; }
    drawPowerUp(powerups[i]);
  }

  // -------- ENEMY STEP / SHOOT --------
  if (now - lastEnemyMove > 80){
    uint8_t active = 0;
    for (uint8_t i=0;i<MAX_ENEMIES;i++){
      if (!enemies[i].active) continue;
      active++;

      // erase old
      eraseRect(enemies[i].x, enemies[i].y, enemies[i].w, enemies[i].h);

      enemies[i].py = enemies[i].y;
      enemies[i].y += 2; // falling

      if (enemies[i].y > SCREEN_H - 1){
        enemies[i].active = false;
        continue;
      }

      // Random fire
      if (enemies[i].type == E_BULLET && (now - lastEnemyShot > 220) && (random(100) < 12)){
        spawnEnemyBullet(enemies[i].x, enemies[i].y);
        lastEnemyShot = now;
      } else if (enemies[i].type == E_LASER && (now - lastEnemyShot > 380) && (random(100) < 9)){
        spawnEnemyLaser(enemies[i].x, enemies[i].y);
        lastEnemyShot = now;
      }

      // redraw
      drawEnemy(enemies[i]);
    }
    if (active < 5) spawnEnemy();
    lastEnemyMove = now;
  }

  // Spawn powerups periodically
  if (now - lastPowerupSpawn > (unsigned long)random(5000, 10000)){
    spawnPowerUp();
    lastPowerupSpawn = now;
  }

  // -------- COLLISIONS --------
  bool playerVulnerable = (now - gameStartTime > INVINCIBILITY_MS) && !shieldActive;

  // bullets vs enemies
  for (uint8_t i=0;i<MAX_ENEMIES;i++){
    if (!enemies[i].active) continue;
    for (uint8_t j=0;j<MAX_BULLETS;j++){
      if (!bullets[j].active) continue;
      if (bullets[j].x < enemies[i].x + enemies[i].w &&
          bullets[j].x + bullets[j].w > enemies[i].x &&
          bullets[j].y < enemies[i].y + enemies[i].h &&
          bullets[j].y + bullets[j].h > enemies[i].y){
        // erase both rects
        eraseRect(enemies[i].x, enemies[i].y, enemies[i].w, enemies[i].h);
        eraseRect(bullets[j].x, bullets[j].y, bullets[j].w, bullets[j].h);
        enemies[i].active = false;
        bullets[j].active = false;
        score++;
        playHitSound();
        saveHighScore();
        updateUIHeader();
      }
    }
  }

  // enemies vs player
  for (uint8_t i=0;i<MAX_ENEMIES;i++){
    if (!enemies[i].active) continue;
    if (playerVulnerable &&
        playerX < enemies[i].x + enemies[i].w &&
        playerX + playerW > enemies[i].x &&
        playerY < enemies[i].y + enemies[i].h &&
        playerY + playerH > enemies[i].y){
      eraseRect(enemies[i].x, enemies[i].y, enemies[i].w, enemies[i].h);
      enemies[i].active = false;
      lives--;
      playDamageSound();
      drawLives();
      if (lives <= 0){
        saveHighScore();
        inGame = false;
        gameOverScreen();
        return;
      }
      // brief invincibility reset
      gameStartTime = now;
    }
  }

  // enemy bullets vs player
  for (uint8_t i=0;i<MAX_ENEMY_BULLETS;i++){
    if (!enemyBullets[i].active) continue;
    if (playerVulnerable &&
        playerX < enemyBullets[i].x + enemyBullets[i].w &&
        playerX + playerW > enemyBullets[i].x &&
        playerY < enemyBullets[i].y + enemyBullets[i].h &&
        playerY + playerH > enemyBullets[i].y){
      eraseRect(enemyBullets[i].x, enemyBullets[i].y, enemyBullets[i].w, enemyBullets[i].h);
      enemyBullets[i].active = false;
      lives--;
      playDamageSound();
      drawLives();
      if (lives <= 0){
        saveHighScore();
        inGame = false;
        gameOverScreen();
        return;
      }
      gameStartTime = now;
    }
  }

  // enemy lasers vs player
  for (uint8_t i=0;i<MAX_ENEMY_LASERS;i++){
    if (!enemyLasers[i].active) continue;
    if (playerVulnerable &&
        playerX < enemyLasers[i].x + enemyLasers[i].w &&
        playerX + playerW > enemyLasers[i].x &&
        playerY < enemyLasers[i].y + enemyLasers[i].h &&
        playerY + playerH > enemyLasers[i].y){
      eraseRect(enemyLasers[i].x, enemyLasers[i].y, enemyLasers[i].w, enemyLasers[i].h);
      enemyLasers[i].active = false;
      lives--;
      playDamageSound();
      drawLives();
      if (lives <= 0){
        saveHighScore();
        inGame = false;
        gameOverScreen();
        return;
      }
      gameStartTime = now;
    }
  }

  // powerups vs player
  for (uint8_t i=0;i<MAX_POWERUPS;i++){
    if (!powerups[i].active) continue;
    if (playerX < powerups[i].x + powerups[i].w &&
        playerX + playerW > powerups[i].x &&
        playerY < powerups[i].y + powerups[i].h &&
        playerY + playerH > powerups[i].y){
      // collected
      eraseRect(powerups[i].x-1, powerups[i].y-1, powerups[i].w+2, powerups[i].h+2);
      PowerUpType t = powerups[i].type;
      powerups[i].active = false;

      if (t == P_EXTRA_LIFE && lives < MAX_LIVES){
        lives++;
        playExtraLifeSound();
        drawLives();
      } else if (t == P_SHIELD){
        shieldActive = true;
        shieldStartTime = now;
        playShieldSound();
        // redraw player with shield outline
        eraseRect(playerX-1, playerY-1, playerW+2, playerH+2);
        drawPlayer(playerX, playerY);
      } else if (t == P_BIG_BANG){
        // wipe all enemies & enemy shots
        for (uint8_t j=0;j<MAX_ENEMIES;j++){
          if (enemies[j].active){
            eraseRect(enemies[j].x, enemies[j].y, enemies[j].w, enemies[j].h);
            enemies[j].active = false;
          }
        }
        for (uint8_t j=0;j<MAX_ENEMY_BULLETS;j++){
          if (enemyBullets[j].active){
            eraseRect(enemyBullets[j].x, enemyBullets[j].y, enemyBullets[j].w, enemyBullets[j].h);
            enemyBullets[j].active = false;
          }
        }
        for (uint8_t j=0;j<MAX_ENEMY_LASERS;j++){
          if (enemyLasers[j].active){
            eraseRect(enemyLasers[j].x, enemyLasers[j].y, enemyLasers[j].w, enemyLasers[j].h);
            enemyLasers[j].active = false;
          }
        }
        score += 5;
        playBigBangSound();
        saveHighScore();
        updateUIHeader();
      }
    }
  }

  delay(20); // steady pace without flicker
}
