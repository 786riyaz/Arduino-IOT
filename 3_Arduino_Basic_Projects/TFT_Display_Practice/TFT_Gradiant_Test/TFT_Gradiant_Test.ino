#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
void setup() {
  tft.initR(INITR_REDTAB); // Try others
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  // Draw color gradient
  for (int x = 0; x < 160; x++) {
    uint16_t color = (x < 53) ? (0xF800 >> (x / 3)) : // Red fade
                     (x < 106) ? (0x07E0 >> ((x - 53) / 3)) : // Green fade
                     (0x001F >> ((x - 106) / 3)); // Blue fade
    tft.drawFastVLine(x, 0, 128, color);
  }
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
  tft.print("Gradient Test");
}
void loop() {}