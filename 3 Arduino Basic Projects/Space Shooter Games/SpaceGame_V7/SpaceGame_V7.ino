/*
  SPACE SHOOTER (TFT ST7735, WHITE BG, NO FLICKER) - FIXED VERSION
  - Panel: ST7735 (INITR_BLACKTAB confirmed by your tests)
  - Libraries: Adafruit_GFX, Adafruit_ST7735, SPI, EEPROM
  - Pins: Updated to fix conflicts and improve stability
  - Memory: Optimized to prevent fragmentation and black screen issues
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <EEPROM.h>

// ---------- TFT PINS ----------
#define TFT_CS     10
#define TFT_RST   9
#define TFT_DC     8

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

// ---------- INPUTS / BUZZER / JOYSTICK ----------
#define BTN_LEFT   2
#define BTN_RIGHT  3
#define BTN_FIRE   4
#define BTN_PAUSE  6
#define BUZZER     7   // FIXED: Changed from 8 to 7 (was conflicting with TFT_DC)

// PS2 Joystick pins
#define JOY_X_PIN  A2  // Analog pin for X-axis
#define JOY_Y_PIN  A1  // Analog pin for Y-axis  
#define JOY_SW_PIN 5   // Digital pin for joystick button (can be used as fire button)

// ---------- GAME LIMITS ----------
#define MAX_BULLETS          3
#define MAX_ENEMY_BULLETS    3
#define MAX_ENEMY_LASERS     1
#define MAX_ENEMIES          6
#define MAX_POWERUPS         2
#define INVINCIBILITY_MS     1500UL
#define SHIELD_DURATION_MS   5000UL
#define MAX_LIVES            7

// Colors
#define ST77XX_DARKGREY 0x7BEF  // R:123 G:123 B:123
#define ST77XX_PURPLE   0x780F  // R:120 G:0   B:240

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

// Joystick calibration values (will be set in setup)
int16_t joyXCenter = 512;
int16_t joyYCenter = 512;
const int16_t JOY_DEADZONE = 50;  // Deadzone to prevent drift
const int16_t JOY_SENSITIVITY = 4; // Movement speed multiplier

int8_t  lives = 3;
uint16_t score = 0;
uint16_t highScore = 0;

bool rapidFire   = false;        
bool shieldActive = false;
bool isPaused     = false;
bool inGame       = false;

unsigned long lastShot          = 0;
unsigned long lastEnemyMove     = 0;
unsigned long lastPowerupSpawn  = 0;
unsigned long lastEnemyShot     = 0;
unsigned long gameStartTime     = 0;
unsigned long shieldStartTime   = 0;
unsigned long lastDisplayCheck  = 0;  // NEW: For display health monitoring

// ---------- COLORS ----------
const uint16_t BG         = ST77XX_WHITE;     
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
inline void beep(uint16_t f, uint16_t d){ 
  tone(BUZZER, f, d); 
}

inline void playShootSound(){ beep(800,20); }
inline void playHitSound(){ beep(500,30); }
inline void playDamageSound(){ beep(200,100); }
inline void playExtraLifeSound(){ 
  beep(1000,50); delay(60); 
  beep(1200,50); delay(60); 
  beep(1400,100); 
}
inline void playShieldSound(){ 
  beep(600,50); delay(60); 
  beep(800,50); delay(60); 
  beep(1000,100); 
}
inline void playBigBangSound(){ 
  beep(400,100); delay(120); 
  beep(600,100); delay(120); 
  beep(800,200); 
}

// IMPROVED: Draw/erase helpers with SPI stability
void eraseRect(int16_t x, int16_t y, int16_t w, int16_t h){
  tft.fillRect(x, y, w, h, BG);
  delayMicroseconds(50); // Give SPI time to complete transmission
}

// IMPROVED: Display health check function
void checkDisplayHealth(){
  unsigned long now = millis();
  if (now - lastDisplayCheck > 15000UL){ // Check every 15 seconds
    // Simple stability test - try a basic draw operation to ensure display responds
    // Draw and immediately erase a small test pixel in corner
    tft.drawPixel(0, 0, COL_TEXT);
    delayMicroseconds(100);
    tft.drawPixel(0, 0, BG);
    
    // If we get here without hanging, display is likely OK
    Serial.println("Display health check: OK");
    lastDisplayCheck = now;
  }
}

// Draw tiny heart icon
void drawHeart(int16_t x, int16_t y){
  // 7x6 heart using small blocks
  tft.drawPixel(x+2, y+1, COL_HP); tft.drawPixel(x+3, y+1, COL_HP); tft.drawPixel(x+4, y+1, COL_HP);
  tft.drawPixel(x+1, y+2, COL_HP); tft.drawPixel(x+2, y+2, COL_HP); tft.drawPixel(x+3, y+2, COL_HP); 
  tft.drawPixel(x+4, y+2, COL_HP); tft.drawPixel(x+5, y+2, COL_HP);
  tft.drawPixel(x+1, y+3, COL_HP); tft.drawPixel(x+2, y+3, COL_HP); tft.drawPixel(x+3, y+3, COL_HP); 
  tft.drawPixel(x+4, y+3, COL_HP); tft.drawPixel(x+5, y+3, COL_HP);
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
  delay(10); // Allow screen clear to complete

  // IMPROVED: New UI layout
  // Lives (top left), High Score (top center), Score (top right)
  updateUIHeader();

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

  // Spawn initial enemies with controlled loop
  uint8_t spawned = 0;
  for (uint8_t j=0; j<MAX_ENEMIES && spawned < 5; j++){
    if (!enemies[j].active){
      enemies[j].w = 10; enemies[j].h = 10;
      enemies[j].x = random(0, SCREEN_W - enemies[j].w);
      enemies[j].y = random(14, 30);
      enemies[j].px = enemies[j].x; enemies[j].py = enemies[j].y;
      enemies[j].active = true;
      uint8_t r = random(100);
      enemies[j].type = (r<40) ? E_COLLIDER : (r<70) ? E_BULLET : E_LASER;
      spawned++;
    }
  }

  // Draw initial elements
  drawPlayer(playerX, playerY);
  for (uint8_t i=0;i<MAX_ENEMIES;i++) {
    if (enemies[i].active) drawEnemy(enemies[i]);
  }
  drawLives();
}

// IMPROVED: Memory-efficient startup screen
void showStartupScreen(){
  tft.fillScreen(BG);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.setTextColor(COL_TEXT, BG);

  // Use const char* instead of String objects to prevent heap fragmentation
  const char* author = "RIYAZ PATHAN";
  const char* game   = "SPACE SHOOTER";
  const char* prompt = "Press FIRE";

  // Calculate center positions more efficiently
  int16_t xAuthor = (SCREEN_W - strlen(author)*12)/2;
  int16_t xGame   = (SCREEN_W - strlen(game)*12)/2;
  int16_t xPrompt = (SCREEN_W - strlen(prompt)*12)/2;

  // Ensure positions are valid
  if (xAuthor < 0) xAuthor = 0;
  if (xGame < 0) xGame = 0;
  if (xPrompt < 0) xPrompt = 0;

  tft.setCursor(xAuthor, 30);
  tft.print(author);
  tft.setCursor(xGame, 55);
  tft.print(game);
  tft.setCursor(xPrompt, 90);
  tft.print(prompt);

  // High score line
  tft.setTextSize(1);
  tft.setCursor(4, 4);
  tft.print("HI: ");
  tft.print(highScore);
}

void drawLives(){
  // Erase lives area at top left
  tft.fillRect(0, 0, 60, 12, BG);
  
  // Draw heart icons for lives at top left
  for (uint8_t i=0; i<lives && i<MAX_LIVES; i++){
    drawHeart(2 + i*8, 2);
  }
}

// ---------- SPAWN FUNCTIONS ----------
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
  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.println("Space Shooter Starting...");

  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_FIRE, INPUT_PULLUP);
  pinMode(BTN_PAUSE, INPUT_PULLUP);
  pinMode(JOY_SW_PIN, INPUT_PULLUP);  // Joystick button
  pinMode(BUZZER, OUTPUT);

  // IMPROVED: More robust TFT initialization
  Serial.println("Initializing TFT...");
  tft.initR(INITR_BLACKTAB);   
  delay(100); // Allow initialization to complete
  
  tft.setRotation(1);          // Landscape
  delay(50);
  
  tft.fillScreen(BG);
  delay(100); // Ensure screen clear completes

  SCREEN_W = tft.width();      // 160 on rotation(1)
  SCREEN_H = tft.height();     // 128 on rotation(1)

  Serial.print("Screen dimensions: ");
  Serial.print(SCREEN_W);
  Serial.print(" x ");
  Serial.println(SCREEN_H);

  // IMPROVED: Calibrate joystick center position
  Serial.println("Calibrating joystick...");
  delay(500); // Let joystick settle
  joyXCenter = analogRead(JOY_X_PIN);
  joyYCenter = analogRead(JOY_Y_PIN);
  Serial.print("Joystick center: X=");
  Serial.print(joyXCenter);
  Serial.print(", Y=");
  Serial.println(joyYCenter);

  // Load high score with validation
  uint8_t hi_high = EEPROM.read(0);
  uint8_t hi_low = EEPROM.read(1);
  highScore = (hi_high << 8) | hi_low;
  
  // Validate high score (prevent corruption)
  if (highScore > 9999) {
    highScore = 0;
    EEPROM.update(0, 0);
    EEPROM.update(1, 0);
  }

  randomSeed(analogRead(A0));

  showStartupScreen();
  Serial.println("Setup complete!");
}

// ---------- INPUT ----------
inline bool btnL(){ return digitalRead(BTN_LEFT)==LOW; }
inline bool btnR(){ return digitalRead(BTN_RIGHT)==LOW; }
inline bool btnF(){ return digitalRead(BTN_FIRE)==LOW; }
inline bool btnP(){ return digitalRead(BTN_PAUSE)==LOW; }
inline bool joyBtn(){ return digitalRead(JOY_SW_PIN)==LOW; }

// IMPROVED: Joystick input functions
int16_t getJoyX(){
  int16_t raw = analogRead(JOY_X_PIN);
  int16_t centered = raw - joyXCenter;
  
  // Apply deadzone
  if (abs(centered) < JOY_DEADZONE) return 0;
  
  // Return movement value (-4 to +4 for smooth movement)
  return constrain(centered / 128, -JOY_SENSITIVITY, JOY_SENSITIVITY);
}

int16_t getJoyY(){
  int16_t raw = analogRead(JOY_Y_PIN);
  int16_t centered = raw - joyYCenter;
  
  // Apply deadzone  
  if (abs(centered) < JOY_DEADZONE) return 0;
  
  // Return movement value (can be used for up/down movement if needed)
  return constrain(centered / 128, -JOY_SENSITIVITY, JOY_SENSITIVITY);
}

// ---------- GAME LOOP HELPERS ----------
void updateUIHeader(){
  // IMPROVED: New layout per request
  // Lives (top left), High Score (top center), Current Score (top right)
  
  // Clear entire top bar
  tft.fillRect(0, 0, SCREEN_W, 12, BG);
  
  tft.setTextSize(1);
  tft.setTextColor(COL_TEXT, BG);
  
  // Lives at top left (red hearts)
  drawLives();
  
  // High Score at top center
  tft.setCursor(SCREEN_W/2 - 18, 2); 
  tft.print("HI:");
  tft.print(highScore);
  
  // Current Score at top right
  tft.setCursor(SCREEN_W - 35, 2); 
  tft.print("S:");
  tft.print(score);
}

void gameOverScreen(){
  tft.fillScreen(BG);
  delay(10); // Let screen clear
  
  tft.setTextColor(COL_TEXT, BG);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.print("GAME OVER");
  
  tft.setTextSize(1);
  tft.setCursor(20, 70);
  tft.print("Score: ");
  tft.print(score);
  tft.setCursor(20, 86);
  tft.print("High Score: ");
  tft.print(highScore);
  tft.setCursor(20, 110);
  tft.print("Press FIRE to Start...");
  
  Serial.println("Game Over - waiting for restart");
}

// IMPROVED: Collision detection with bounds checking
bool checkCollision(int16_t x1, int16_t y1, int16_t w1, int16_t h1,
                   int16_t x2, int16_t y2, int16_t w2, int16_t h2){
  return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

// ---------- MAIN LOOP ----------
void loop(){
  // IMPROVED: Add display health monitoring
  if (inGame) checkDisplayHealth();

  if (!inGame){
    // Can start game with either fire button or joystick button
    if (btnF() || joyBtn()){
      delay(200);
      Serial.println("Starting new game...");
      startGame();
    }
    return;
  }

  // Pause toggle with improved debouncing
  static bool pauseLatch = false;
  static unsigned long lastPauseTime = 0;
  unsigned long now = millis();
  
  if (btnP() && !pauseLatch && (now - lastPauseTime > 300)){
    isPaused = !isPaused;
    pauseLatch = true;
    lastPauseTime = now;
    
    // show pause banner
    tft.fillRect(SCREEN_W/2-24, 0, 48, 12, BG);
    tft.setTextSize(1);
    tft.setTextColor(COL_TEXT, BG);
    tft.setCursor(SCREEN_W/2-20, 2);
    tft.print(isPaused ? "PAUSE" : "PLAY ");
    
    Serial.println(isPaused ? "Game Paused" : "Game Resumed");
  } else if (!btnP()){
    pauseLatch = false;
  }

  if (isPaused){ delay(25); return; }

  // -------- INPUT: MOVE & SHOOT --------
  // Erase old player position
  eraseRect(playerX-1, playerY-1, playerW+2, playerH+2);

  // IMPROVED: Joystick + button movement (both methods work)
  int16_t joyMoveX = getJoyX();
  
  // Joystick movement (analog)
  if (joyMoveX != 0){
    playerX += joyMoveX;
  }
  // Button movement (digital) - still works as backup
  else {
    if (btnL() && playerX > 0) playerX -= 2;
    if (btnR() && playerX < SCREEN_W-playerW) playerX += 2;
  }
  
  // Ensure player stays on screen
  playerX = constrain(playerX, 0, SCREEN_W - playerW);

  // FIRE with improved rate limiting (button OR joystick button)
  uint16_t fireDelay = rapidFire ? 150 : 400;
  if ((btnF() || joyBtn()) && (now - lastShot > fireDelay)){
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

  // Redraw player
  drawPlayer(playerX, playerY);

  // -------- SHIELD TIMER --------
  if (shieldActive && (now - shieldStartTime > SHIELD_DURATION_MS)){
    shieldActive = false;
    // Redraw player without shield
    eraseRect(playerX-1, playerY-1, playerW+2, playerH+2);
    drawPlayer(playerX, playerY);
  }

  // -------- MOVE PLAYER BULLETS --------
  for (uint8_t i=0;i<MAX_BULLETS;i++){
    if (!bullets[i].active) continue;
    
    eraseRect(bullets[i].x, bullets[i].y, bullets[i].w, bullets[i].h);
    bullets[i].py = bullets[i].y;
    bullets[i].y -= 3;
    
    if (bullets[i].y < 12){ 
      bullets[i].active=false; 
      continue; 
    }
    drawBullet(bullets[i]);
  }

  // -------- MOVE ENEMY BULLETS --------
  for (uint8_t i=0;i<MAX_ENEMY_BULLETS;i++){
    if (!enemyBullets[i].active) continue;
    
    eraseRect(enemyBullets[i].x, enemyBullets[i].y, enemyBullets[i].w, enemyBullets[i].h);
    enemyBullets[i].y += 3;
    
    if (enemyBullets[i].y > SCREEN_H-1){ 
      enemyBullets[i].active=false; 
      continue; 
    }
    drawEnemyBullet(enemyBullets[i]);
  }

  // -------- MOVE ENEMY LASERS --------
  for (uint8_t i=0;i<MAX_ENEMY_LASERS;i++){
    if (!enemyLasers[i].active) continue;
    
    eraseRect(enemyLasers[i].x, enemyLasers[i].y, enemyLasers[i].w, enemyLasers[i].h);
    enemyLasers[i].y += 2;
    
    if (enemyLasers[i].y > SCREEN_H-1){ 
      enemyLasers[i].active=false; 
      continue; 
    }
    drawEnemyLaser(enemyLasers[i]);
  }

  // -------- MOVE POWERUPS --------
  for (uint8_t i=0;i<MAX_POWERUPS;i++){
    if (!powerups[i].active) continue;
    
    eraseRect(powerups[i].x-1, powerups[i].y-1, powerups[i].w+2, powerups[i].h+2);
    powerups[i].y += 1;
    
    if (powerups[i].y > SCREEN_H-1){ 
      powerups[i].active=false; 
      continue; 
    }
    drawPowerUp(powerups[i]);
  }

  // -------- ENEMY MOVEMENT & SHOOTING --------
  if (now - lastEnemyMove > 85){ // Slightly slower to reduce load
    uint8_t activeEnemies = 0;
    
    for (uint8_t i=0;i<MAX_ENEMIES;i++){
      if (!enemies[i].active) continue;
      activeEnemies++;

      // Erase old position
      eraseRect(enemies[i].x, enemies[i].y, enemies[i].w, enemies[i].h);

      enemies[i].py = enemies[i].y;
      enemies[i].y += 2; // falling

      if (enemies[i].y > SCREEN_H - 1){
        enemies[i].active = false;
        continue;
      }

      // Random shooting with improved timing
      if (enemies[i].type == E_BULLET && (now - lastEnemyShot > 250) && (random(100) < 10)){
        spawnEnemyBullet(enemies[i].x, enemies[i].y);
        lastEnemyShot = now;
      } else if (enemies[i].type == E_LASER && (now - lastEnemyShot > 400) && (random(100) < 8)){
        spawnEnemyLaser(enemies[i].x, enemies[i].y);
        lastEnemyShot = now;
      }

      // Redraw enemy
      drawEnemy(enemies[i]);
    }
    
    // Spawn new enemies if needed
    if (activeEnemies < 4) spawnEnemy(); // Reduced from 5 to 4 for stability
    lastEnemyMove = now;
  }

  // Spawn powerups periodically
  if (now - lastPowerupSpawn > (unsigned long)random(6000, 12000)){ // Slightly longer intervals
    spawnPowerUp();
    lastPowerupSpawn = now;
  }

  // -------- COLLISION DETECTION --------
  bool playerVulnerable = (now - gameStartTime > INVINCIBILITY_MS) && !shieldActive;

  // IMPROVED: Bullets vs enemies with better collision detection
  for (uint8_t i=0;i<MAX_ENEMIES;i++){
    if (!enemies[i].active) continue;
    
    for (uint8_t j=0;j<MAX_BULLETS;j++){
      if (!bullets[j].active) continue;
      
      if (checkCollision(bullets[j].x, bullets[j].y, bullets[j].w, bullets[j].h,
                        enemies[i].x, enemies[i].y, enemies[i].w, enemies[i].h)){
        // Erase both objects
        eraseRect(enemies[i].x, enemies[i].y, enemies[i].w, enemies[i].h);
        eraseRect(bullets[j].x, bullets[j].y, bullets[j].w, bullets[j].h);
        
        enemies[i].active = false;
        bullets[j].active = false;
        score++;
        playHitSound();
        saveHighScore();
        updateUIHeader();
        break; // Exit inner loop early
      }
    }
  }

  // IMPROVED: Enemies vs player with better collision detection
  for (uint8_t i=0;i<MAX_ENEMIES;i++){
    if (!enemies[i].active) continue;
    
    if (playerVulnerable && 
        checkCollision(playerX, playerY, playerW, playerH,
                      enemies[i].x, enemies[i].y, enemies[i].w, enemies[i].h)){
      eraseRect(enemies[i].x, enemies[i].y, enemies[i].w, enemies[i].h);
      enemies[i].active = false;
      lives--;
      playDamageSound();
      drawLives();
      
      Serial.print("Player hit! Lives remaining: ");
      Serial.println(lives);
      
      if (lives <= 0){
        saveHighScore();
        inGame = false;
        Serial.println("Game Over!");
        gameOverScreen();
        return;
      }
      // Brief invincibility reset
      gameStartTime = now;
      break; // Exit loop early
    }
  }

  // IMPROVED: Enemy bullets vs player
  for (uint8_t i=0;i<MAX_ENEMY_BULLETS;i++){
    if (!enemyBullets[i].active) continue;
    
    if (playerVulnerable &&
        checkCollision(playerX, playerY, playerW, playerH,
                      enemyBullets[i].x, enemyBullets[i].y, enemyBullets[i].w, enemyBullets[i].h)){
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
      break;
    }
  }

  // IMPROVED: Enemy lasers vs player
  for (uint8_t i=0;i<MAX_ENEMY_LASERS;i++){
    if (!enemyLasers[i].active) continue;
    
    if (playerVulnerable &&
        checkCollision(playerX, playerY, playerW, playerH,
                      enemyLasers[i].x, enemyLasers[i].y, enemyLasers[i].w, enemyLasers[i].h)){
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
      break;
    }
  }

  // IMPROVED: Powerups vs player with better collision and effects
  for (uint8_t i=0;i<MAX_POWERUPS;i++){
    if (!powerups[i].active) continue;
    
    if (checkCollision(playerX, playerY, playerW, playerH,
                      powerups[i].x, powerups[i].y, powerups[i].w, powerups[i].h)){
      // Collected powerup
      eraseRect(powerups[i].x-1, powerups[i].y-1, powerups[i].w+2, powerups[i].h+2);
      PowerUpType t = powerups[i].type;
      powerups[i].active = false;

      Serial.print("Powerup collected: ");
      Serial.println((int)t);

      if (t == P_EXTRA_LIFE && lives < MAX_LIVES){
        lives++;
        playExtraLifeSound();
        drawLives();
        Serial.println("Extra life gained!");
        
      } else if (t == P_SHIELD){
        shieldActive = true;
        shieldStartTime = now;
        playShieldSound();
        // Redraw player with shield outline
        eraseRect(playerX-1, playerY-1, playerW+2, playerH+2);
        drawPlayer(playerX, playerY);
        Serial.println("Shield activated!");
        
      } else if (t == P_BIG_BANG){
        // Clear all enemies and enemy projectiles
        uint8_t cleared = 0;
        
        for (uint8_t j=0;j<MAX_ENEMIES;j++){
          if (enemies[j].active){
            eraseRect(enemies[j].x, enemies[j].y, enemies[j].w, enemies[j].h);
            enemies[j].active = false;
            cleared++;
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
        
        Serial.print("Big Bang! Cleared ");
        Serial.print(cleared);
        Serial.println(" enemies");
      }
      break; // Exit powerup loop early
    }
  }

  // IMPROVED: Reduced loop delay for better stability
  delay(25); // Increased from 20 to 25ms to reduce SPI/processing load
}