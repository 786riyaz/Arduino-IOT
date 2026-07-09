#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Hexadecimal address of the LCD unit
#define I2C_ADDR 0x27 //0x3F for 16 x 2 units

// Tell the LCD what type we have (eg 16 x 2 or 20 x 4)
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

// Define the bit patters for each of our custom chars. These
// are 5 bits wide and 8 dots deep
uint8_t custChar[8][8] = {
  {31, 31, 31, 0, 0, 0, 0, 0},      // Small top line - 0
  {0, 0, 0, 0, 0, 31, 31, 31},      // Small bottom line - 1
  {31, 0, 0, 0, 0, 0, 0, 31},      // Small lines top and bottom -2
  {0, 0, 0, 0, 0, 0,  0, 31},       // Thin bottom line - 3
  {31, 31, 31, 31, 31, 31, 15, 7},  // Left bottom chamfer full - 4
  {28, 30, 31, 31, 31, 31, 31, 31}, // Right top chamfer full -5
  {31, 31, 31, 31, 31, 31, 30, 28}, // Right bottom chamfer full -6
  {7, 15, 31, 31, 31, 31, 31, 31},  // Left top chamfer full -7
};

// Define our numbers 0 thru 9 // 254 is blank and 255 is the "Full Block"
uint8_t bigNums[10][6] = {
  {7, 0, 5, 4, 1, 6},         //0
  {0, 5, 254, 1, 255, 1},     //1
  {0, 2, 5, 7, 3, 1},         //2
  {0, 2, 5, 1, 3, 6},         //3
  {7, 3, 255, 254, 254, 255}, //4
  {7, 2, 0, 1, 3, 6},         //5
  {7, 2, 0, 4, 3, 6},         //6
  {0, 0, 5, 254, 7, 254},      //7
  {7, 2, 5, 4, 3, 6},         //8
  {7, 2, 5, 1, 3, 6},         //9
};
int var = 0;
// ------------------------------------------------------
// SETUP    SETUP    SETUP    SETUP    SETUP    SETUP
// ------------------------------------------------------
void setup()
{
  // Serial monitoring on PC (debug only)
  Serial.begin(9600);

  // Initialise the LCD: turn on backlight and ensure cursor top left
  lcd.init();
  lcd.backlight();
  delay(500);
  lcd.home();
  lcd.clear();

  // Print all the special chars out
  //    for (int cnt = 0; cnt < 8; cnt++) {
  //      lcd.print((char)cnt);
  //    }

  // Demo Part 1 - the custom characters
  // Create custom character map (8 characters only!)
  for (int cnt = 0; cnt < sizeof(custChar) / 8; cnt++) {
    lcd.createChar(cnt, custChar[cnt]);
  }

  // Now print the numbers 0 to 9    // Format is: number2print, startCol, startRow
  printBigNum(1, 0, 0);
  printBigNum(2, 4, 0);
  printBigNum(3, 8, 0);
  printBigNum(4, 12, 0);
  delay(1000);
}


// Print big number over 2 lines, 3 colums per half digit
void printBigNum(int number, int startCol, int startRow) {

  // Position cursor to requested position (each char takes 3 cols plus a space col)
  lcd.setCursor(startCol, startRow);

  // Each number split over two lines, 3 chars per line. Retrieve character
  // from the main array to make working with it here a bit easier.
  uint8_t thisNumber[6];
  for (int cnt = 0; cnt < 6; cnt++) {
    thisNumber[cnt] = bigNums[number][cnt];
  }

  for (int cnt = 0; cnt < 3; cnt++) {       // First line (top half) of digit
    lcd.print((char)thisNumber[cnt]);
  }
  lcd.setCursor(startCol, startRow + 1);    // Now position cursor to next line at same start column for digit
  for (int cnt = 3; cnt < 6; cnt++) {       // 2nd line (bottom half)
    lcd.print((char)thisNumber[cnt]);
  }
}

void loop() {
  printBigNum(var, 12, 0);
  delay(1000);
  var++;
  if (var > 9) {
    var = 0 ;
  }
}
