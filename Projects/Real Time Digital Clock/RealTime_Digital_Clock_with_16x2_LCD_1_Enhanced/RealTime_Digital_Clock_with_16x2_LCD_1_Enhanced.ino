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
String prevTime = "", prevDate = "", prevDay = "", prevYY = "", prevMM = "", prevDD = "", prevHH = "", prevmm = "", prevSSS = "", prevdd = "";
String currTime = "", currDate = "", currDay = "", YY = "", MM = "", DD = "", HH = "", mm = "", SSS = "", dd = "";
String AMPM = "";
int hh;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(19200);
  rtc.begin();

  //  Uncomment These Lines only to set the time in RTC
  //  rtc.setDOW(WEDNESDAY);
  // rtc.setTime(17, 38, 4);
  //  rtc.setDate(31, 8, 2022);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  currTime = rtc.getTimeStr();
  currDate =  rtc.getDateStr();
  currDay = rtc.getDOWStr();

  lcd.setCursor(0, 0);
  lcd.print(currTime);
  lcd.setCursor(2, 0);
  lcd.print(" ");
  lcd.setCursor(5, 0);
  lcd.print(" ");
  lcd.setCursor(8, 0);
  lcd.print(" ----DAY");
  lcd.setCursor(0, 1);
  lcd.print("MONTH---");
  lcd.setCursor(8, 1);
  lcd.print(currDate);
  lcd.setCursor(10, 1);
  lcd.print(" ");
  lcd.setCursor(13, 1);
  lcd.print(" ");
  Serial.println("Current Time :  " + currTime + "---" + currDay + "---" + currDate);
  Serial.println("End Setup");
}

void loop() {
  currTime = rtc.getTimeStr();
  if (currTime != prevTime) {

    SSS = currTime.substring(6, 8);
    if (SSS != prevSSS) {
      //Serial.print(SSS[0]);
      lcd.setCursor(6, 0);
      lcd.print(SSS);
      prevSSS = SSS;

      MM = currTime.substring(3, 5);
      if (MM != prevMM) {
        Serial.println("\t\t Minute Change = " + MM);
        if (MM == "30") {
          digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(200);                       // wait for a second
          digitalWrite(LED_BUILTIN, LOW);
        }
        lcd.setCursor(3, 0);
        lcd.print(MM);
        prevMM = MM;

        HH = currTime.substring(0, 2);
        Serial.println("Hour = " + HH);
        if (prevHH != HH) {
          Serial.println("Hour Change");
          digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(200);                       // wait for a second
          digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
          delay(100);
          digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(200);                       // wait for a second
          digitalWrite(LED_BUILTIN, LOW);
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
          }

          lcd.setCursor(0, 0);
          lcd.print(HH);

          hh = HH.toInt();
          if (AMPM == "PM") {
            if (hh != 12) {
              hh = hh + 12;
              //Serial.println("Plus = ");
              //            Serial.println(hh);
              if (hh == 24) {
                hh = 0;
                Serial.println("0");
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
          Serial.println("After = " + HH);

          currDate = rtc.getDateStr();
          Serial.println("C Date = " + currDate);
          Serial.println("P Date = " + prevDate);
          if (currDate != prevDate) {

            if (currDate != prevDate) {
              //Serial.println("TEst");
              dd = rtc.getDOWStr();
              //Serial.println("Day = " + dd);
              lcd.setCursor(8, 0);
              if (dd == "Monday") {
                dd = " ----MON";      lcd.print(dd);
              }
              else if (dd == "Tuesday") {
                dd = " ---TUES";      lcd.print(dd);
              }
              else if (dd == "Wednesday") {
                dd = " -WEDNES";      lcd.print(dd);
              }
              else if (dd == "Thursday") {
                dd = " --THURS";      lcd.print(dd);
              }
              else if (dd == "Friday") {
                dd = " ----FRI";      lcd.print(dd);
              }
              else if (dd == "Saturday") {
                dd = " --SATUR";      lcd.print(dd);
              }
              else if (dd == "Sunday") {
                dd = " ----SUN";      lcd.print(dd);
              }
              //Serial.println("Month = " + mm);

            }
            prevTime = currTime;
          }

          DD = currDate.substring(0, 2);
          if (DD != prevDD) {
            Serial.println("Date Change");
            lcd.setCursor(8, 1);
            lcd.print(DD);

            mm = currDate.substring(3, 5);
            lcd.setCursor(0, 1);
            if (mm == "01")  {
              lcd.print("JAN---- ");
            }
            else if (mm == "02") {
              lcd.print("FEB---- ");
            }
            else if (mm == "03") {
              lcd.print("MARCH-- ");
            }
            else if (mm == "04") {
              lcd.print("APRIL-- ");
            }
            else if (mm == "05") {
              lcd.print("MAY---- ");
            }
            else if (mm == "06") {
              lcd.print("JUNE--- ");
            }
            else if (mm == "07") {
              lcd.print("JULY--- ");
            }
            else if (mm == "08") {
              lcd.print("AUGUST- ");
            }
            else if (mm == "09") {
              lcd.print("SEPT--- ");
            }
            else if (mm == "10") {
              lcd.print("OCT---- ");
            }
            else if (mm == "11") {
              lcd.print("NOV---- ");
            }
            else if (mm == "12") {
              lcd.print("DEC---- ");
            }
            if (mm != prevmm) {
              Serial.println("Month Change");
              lcd.setCursor(11, 1);
              lcd.print(mm);


              YY = currDate.substring(8, 10);
              if (YY != prevYY) {
                Serial.println("Year Change");
                lcd.setCursor(14, 1);
                lcd.print(YY);
                prevYY = YY;
              }
              prevmm = mm;
            }
            prevDD = DD;
          }
          prevDate = currDate;
        }
        prevHH = HH;
      }
      prevMM = MM;
    }
    prevSSS = SSS;
  }
  delay(200);
  //Serial.print("Current Time :  " + String(rtc.getTimeStr()) + " " + rtc.getDOWStr() + " " + rtc.getDateStr() + " " + rtc.getTimeStr());
}
