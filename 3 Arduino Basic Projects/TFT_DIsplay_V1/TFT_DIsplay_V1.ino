#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

// Pin definitions for the TFT display
#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

// Initialize the display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Array of colors for the text
uint16_t colors[] = {
  ST7735_RED, ST7735_GREEN, ST7735_BLUE, ST7735_YELLOW, ST7735_CYAN,
  ST7735_MAGENTA, ST7735_WHITE, 0xFD20, 0xF81F, 0x07E0 // Custom colors
};

void setup() {
  // Initialize the TFT display
  tft.initR(INITR_BLACKTAB); // Use this for 1.8" TFT with ST7735
  tft.setRotation(1);        // Rotate display (adjust 0-3 as needed)
  tft.fillScreen(ST7735_BLACK); // Clear screen with black background

  // Set text size and font properties
  tft.setTextSize(2); // Adjust size (1, 2, 3, etc. for larger text)
  tft.setTextWrap(false);

  // Center the text "RIYAZ PATHAN"
  String text = "RIYAZ PATHAN";
  int16_t x = (tft.width() - text.length() * 12) / 2; // Approximate width per character (6 pixels * text size)
  int16_t y = (tft.height() - 16) / 2; // Approximate height per character (8 pixels * text size)

  // Display each character with a random color
  for (int i = 0; i < text.length(); i++) {
    tft.setCursor(x + i * 12, y); // Move cursor for each character
    tft.setTextColor(colors[random(0, sizeof(colors)/sizeof(colors[0]))]); // Random color
    tft.print(text[i]);
  }
}

void loop() {
  // No action needed in loop; text stays on screen
}   