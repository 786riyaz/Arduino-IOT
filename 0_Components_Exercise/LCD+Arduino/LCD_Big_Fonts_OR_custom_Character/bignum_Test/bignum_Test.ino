#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x27 //0x3F for 16 x 2 units

LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

byte custChar[8][8] = {
  {31, 31, 31, 0, 0, 0, 0, 0},        // Small top line - 0
  {0, 0, 0, 0, 0, 31, 31, 31},       // Small bottom line - 1
  {31, 0, 0, 0, 0, 0, 0, 31},        // Small lines top and bottom -2
  {0, 0, 0, 0, 0, 0,  0, 31},       // Thin bottom line - 3
  {31, 31, 31, 31, 31, 31, 15, 7},  // Left bottom chamfer full - 4
  {28, 30, 31, 31, 31, 31, 31, 31}, // Right top chamfer full -5
  {31, 31, 31, 31, 31, 31, 30, 28}, // Right bottom chamfer full -6
  {7, 15, 31, 31, 31, 31, 31, 31},  // Left top chamfer full -7
};

// Define our numbers 0 thru 9
// 254 is blank and 255 is the "Full Block"
byte bigNums[10][6] = {
  {7, 0, 5, 4, 1, 6},         //0
  {0, 5, 254, 1, 255, 1},     //1
  {0, 2, 5, 7, 3, 1},         //2
  {0, 2, 5, 1, 3, 6},         //3
  {7, 3, 255, 254, 254, 255}, //4
  {7, 2, 0, 1, 3, 6},         //5
  {7, 2, 0, 4, 3, 6},         //6
  {0, 0, 5, 254, 7, 254},     //7
  {7, 2, 5, 4, 3, 6},         //8
  {7, 2, 5, 1, 3, 6},         //9
};
//-----------------------------------------------------
void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.home();
  lcd.print((char)255);
  lcd.print((char)bigNums[0][0]);
  lcd.print((char)bigNums[0][1]);
  lcd.print((char)bigNums[0][2]);
  lcd.print((char)bigNums[0][3]);
  lcd.print((char)bigNums[0][4]);
  lcd.print((char)bigNums[0][5]);
//  lcd.print((char)bigNums[0][6]);
   //lcd.write(1);
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print(1);
  lcd.print((char)custChar[0][0]);
  lcd.print((char)custChar[0][1]);
  lcd.print((char)custChar[0][2]);
  lcd.print((char)custChar[0][3]);
  lcd.print((char)custChar[0][4]);
  lcd.print((char)custChar[0][5]);
  char c = (char)custChar[0][5];
//  lcd.createChar(9,c);
  lcd.print((char)9);
}

void loop() {
  // put your main code here, to run repeatedly:
  //printBigNum(1, 0, 0);
}


void printBigNum(int number, int startCol, int startRow)//1
{
  lcd.setCursor(startCol, startRow);

  // Each number split over two lines, 3 chars per line. Retrieve character
  // from the main array to make working with it here a bit easier.
  byte thisNumber[6];
  for (int cnt = 0; cnt < 6; cnt++) {
    thisNumber[cnt] = bigNums[number][cnt];
  }
  //{0, 5, 254, 1, 255, 1}

  // First line (top half) of digit
  for (int cnt = 0; cnt < 3; cnt++) {
    //lcd.print((char)thisNumber[cnt]);
    lcd.createChar(1,custChar[thisNumber[cnt]]);
    lcd.write(1);
  }

  // Now position cursor to next line at same start column for digit
  lcd.setCursor(startCol, startRow + 1);

  // 2nd line (bottom half)
  for (int cnt = 3; cnt < 6; cnt++) {
    //lcd.print((char)thisNumber[cnt]);
    lcd.createChar(cnt,custChar[thisNumber[cnt]]);
    lcd.write(cnt);
  }
}
