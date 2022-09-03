#include<LiquidCrystal_I2C.h>
#include<DS3231.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // SDA=A4 SCL=A5
DS3231  rtc(SDA, SCL);
Time t;
String prevTime, HH, MM,  DD, mm, YY, prevDate, DateString, dd, TimeString, SSS, currTime, currDate;
int hh;

void setup() {
  Serial.begin(19200);
  rtc.begin();

  // Uncomment These Lines only to set the time in RTC
  //rtc.setDOW(SATURDAY);
  //rtc.setTime(00, 15, 0);
  //rtc.setDate(27, 8, 2022);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  // lcd.setCursor(0, 0);
  // lcd.print("   RIYAZ KHAN  ");
  // lcd.setCursor(0, 1);
  // lcd.print("-REALTIME CLOCK-");
  // delay(200);

  Serial.print("Current Time :  " + String(rtc.getTimeStr()) + " " + rtc.getDOWStr() + " " + rtc.getDateStr() + " " + rtc.getTimeStr());
}

void loop() {
  lcd.clear();
  currTime = rtc.getTimeStr();
  prevTime = currTime;
  HH = prevTime.substring(0, 2);
  hh = HH.toInt() % 12;
  HH = String(hh);
  if (HH.length() == 1)
  {
    //HH = "0" + HH;
    HH = " " + HH;
  }
  MM = prevTime.substring(3, 5);
  SSS = prevTime.substring(6, 8);
  TimeString = HH + ":" + MM + ":" + SSS + "--";

  currDate = rtc.getDateStr(5, 7);
  prevDate = currDate;
  DD = prevDate.substring(0, 2);
  mm = prevDate.substring(3, 5);
  YY = prevDate.substring(8, 10);
  DateString =  DD + "/" + mm + "/" + YY;

  dd = rtc.getDOWStr();
  //Serial.println("Day = " + dd);
  lcd.setCursor(10, 0);
  if (dd == "Monday") {
    dd = "---MON";      lcd.print(dd);
  }
  else if (dd == "Tuesday") {
    dd = "--TUES";      lcd.print(dd);
  }
  else if (dd == "Wednesday") {
    dd = "WEDNES";      lcd.print(dd);
  }
  else if (dd == "Thursday") {
    dd = "-THURS";      lcd.print(dd);
  }
  else if (dd == "Friday") {
    dd = "---FRI";      lcd.print(dd);
  }
  else if (dd == "Saturday") {
    dd = "-SATUR";      lcd.print(dd);
  }
  else if (dd == "Sunday") {
    dd = "---SUN";      lcd.print(dd);
  }


  //Serial.println("Month = " + mm);
  lcd.setCursor(0, 1);
  if (mm == "01")  {
    lcd.print("JAN-----");
  }
  else if (mm == "02") {
    lcd.print("FEB-----");
  }
  else if (mm == "03") {
    lcd.print("MARCH---");
  }
  else if (mm == "04") {
    lcd.print("APRIL---");
  }
  else if (mm == "05") {
    lcd.print("MAY-----");
  }
  else if (mm == "06") {
    lcd.print("JUNE----");
  }
  else if (mm == "07") {
    lcd.print("JULY----");
  }
  else if (mm == "08") {
    lcd.print("AUGUST--");
  }
  else if (mm == "09") {
    lcd.print("SEPT----");
  }
  else if (mm == "10") {
    lcd.print("OCT-----");
  }
  else if (mm == "11") {
    lcd.print("NOV-----");
  }
  else if (mm == "12") {
    lcd.print("DEC-----");
  }

  lcd.setCursor(0, 0);
  lcd.print(TimeString);
  lcd.setCursor(8, 1);
  lcd.print(DateString);
  delay(200);
}
