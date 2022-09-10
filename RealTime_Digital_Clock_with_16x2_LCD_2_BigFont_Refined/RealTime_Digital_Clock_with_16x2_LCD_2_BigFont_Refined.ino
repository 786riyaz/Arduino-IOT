#include<LiquidCrystal_I2C.h>
#include<DS3231.h>

/* Cable Connections
  I2C Module
  SCL = A5
  SDA = A4

  DS3231 Module
  SCL = A5
  SDA = A4
*/

LiquidCrystal_I2C lcd(0x27, 16, 2); // SDA=A4 SCL=A5
DS3231  rtc(SDA, SCL);
Time t;
String     prevHH = "";
String currTime = "", MM = "",  HH = "", mm = "", ss = "";
String AMPM = "";
int hh;

uint8_t custChar[8][8] = {
  {31, 31, 31, 0, 0, 0, 0, 0},      // Small top line - 0
  {0, 0, 0, 0, 0, 0, 0, 31},        // Small bottom line - 1
  {31, 0, 0, 0, 0, 0, 0, 31},       // Small lines top and bottom -2
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
  {0, 2, 5, 0, 3, 6},         //3
  {7, 3, 255, 254, 254, 255}, //4
  {7, 2, 0, 1, 3, 6},         //5
  {7, 2, 0, 4, 3, 6},         //6
  {0, 0, 5, 254, 7, 254},     //7
  {7, 2, 5, 4, 3, 6},         //8
  {7, 2, 5, 1, 3, 6},         //9
};
void setup() {

  rtc.begin();
  //  Uncomment These Lines only to set the time in RTC
  //  rtc.setDOW(WEDNESDAY);
  //  rtc.setTime(8, 45, 00);
  //  rtc.setDate(31, 8, 2022);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.begin(19200);

  for (int cnt = 0; cnt < sizeof(custChar) / 8; cnt++) {
    lcd.createChar(cnt, custChar[cnt]);
  }
}

void printBigNum(int number, int startCol, int startRow) {

  // Position cursor to requested position (each char takes 3 cols plus a space col)
  lcd.setCursor(startCol, startRow);

  // Each number split over two lines, 3 chars per line. Retrieve character
  // from the main array to make working with it here a bit easier.
  uint8_t thisNumber[6];
  for (int cnt = 0; cnt < 6; cnt++) {
    thisNumber[cnt] = bigNums[number][cnt];
    //Serial.println("------------------- "+ String(cnt));
    //Serial.println("------------------------------ " + String(thisNumber[cnt]));
  }

  for (int cnt = 0; cnt < 3; cnt++) {       // First line (top half) of digit
    lcd.print((char)thisNumber[cnt]);
  }
  lcd.setCursor(startCol, startRow + 1);    // Now position cursor to next line at same start column for digit
  for (int cnt = 3; cnt < 6; cnt++) {       // 2nd line (bottom half)
    lcd.print((char)thisNumber[cnt]);
  }
}

void loop()
{
  currTime = rtc.getTimeStr();
  //Serial.println(currTime);
  MM = currTime.substring(3, 5);
  int MMM = MM.toInt();

  printBigNum(MMM / 10, 10, 0);
  printBigNum(MMM % 10, 13, 0);

  ss = currTime.substring(6, 8);
  lcd.setCursor(7, 0);
  lcd.print(ss[0]);
  lcd.setCursor(7, 1);
  lcd.print(ss[1]);


  HH = currTime.substring(0, 2);
  if (prevHH != HH) {
    hh = HH.toInt();
    if (hh >= 12) {
      AMPM = "PM";
      //Serial.println("PM");
    } else {
      AMPM = "AM";
    }
    hh = hh % 12;

    HH = String(hh);
    if (HH.length() == 1) {
      if (hh == 0) {
        HH = "12";
      } else {
        HH = " " + HH;
      }
      //Clear First Charcter
      lcd.clear();
    }

    hh = HH.toInt();
    //  printBigNum(1, 0, 0);
    if (hh / 10 == 1) {
      printBigNum(hh / 10, 0, 0);
    }
    printBigNum(hh % 10, 2, 0);
    if (AMPM == "PM") {
      if (hh != 12) {
        hh = hh + 12;
        //Serial.println("Plus = ");
        //            Serial.println(hh);
        if (hh == 24) {
          hh = 0;
          // Serial.println("0");
        }
      }
    } else {
      if (hh == 12) {
        hh = 0;
      }
    }
    HH = String(hh);
    if (HH.length() == 1) {
      if (hh == 0) {
        HH = "00";
      } else {
        HH = "0" + HH;
      }
    }
    prevHH = HH;
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(250);
}
