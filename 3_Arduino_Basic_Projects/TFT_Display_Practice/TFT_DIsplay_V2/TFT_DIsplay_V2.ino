#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ---- Pin configuration (adjust if needed) ----
#define TFT_CS     10
#define TFT_RST     9
#define TFT_DC      8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// ---- Panel dimensions for ST7735 Red Tab ----
#define PANEL_W 128
#define PANEL_H 160

// ---- Text scrolling variables ----
int16_t textX = PANEL_W;  // Start at right edge
int16_t textY = 70;       // Vertical position
const char* scrollText = " Hello from ST7735! ";

// Convert RGB888 to RGB565
uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Gradient color function
uint16_t Color565Gradient(int x, int y) {
  uint8_t r = map(x, 0, PANEL_W - 1, 0, 255);   // horizontal red gradient
  uint8_t g = map(y, 0, PANEL_H - 1, 0, 255);   // vertical green gradient
  uint8_t b = 128;                              // fixed blue tone
  return Color565(r, g, b);
}

// Draw full gradient background once
void drawFullGradient() {
  for (int y = 0; y < PANEL_H; y++) {
    tft.startWrite();
    tft.setAddrWindow(0, y, PANEL_W, 1);
    for (int x = 0; x < PANEL_W; x++) {
      tft.pushColor(Color565Gradient(x, y));
    }
    tft.endWrite();
  }
}

// Draw scrolling text with background restore
void scrollTextWithGradient() {
  int16_t textW = strlen(scrollText) * 6; // 6px width per char (size=1)
  int16_t textH = 8;                      // 8px height per char

  // restore gradient strip behind text
  for (int gy = textY; gy < textY + textH; gy++) {
    tft.startWrite();
    tft.setAddrWindow(0, gy, PANEL_W, 1);
    for (int gx = 0; gx < PANEL_W; gx++) {
      tft.pushColor(Color565Gradient(gx, gy));
    }
    tft.endWrite();
  }

  // draw text
  tft.setCursor(textX, textY);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(scrollText);

  // move text left
  textX--;
  if (textX < -textW) textX = PANEL_W; // reset after fully leaving screen
}

void setup() {
  tft.initR(INITR_REDTAB);   // Init ST7735 Red Tab
  tft.setRotation(1);        // Landscape (try 0/1/2/3 if needed)
  tft.fillScreen(ST77XX_BLACK);
  drawFullGradient();
}

void loop() {
  scrollTextWithGradient();
  delay(30);  // speed control
}
