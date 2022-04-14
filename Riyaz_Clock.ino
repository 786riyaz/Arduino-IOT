#include <RTClib.h>
#include <Wire.h>

RTC_DS3231 rtc;

char t[32];

const int clockPin = 13;

const int latchPin1 = 11;
const int dataPin1 = 12;

const int latchPin2 = 9;
const int dataPin2 = 10;

const int latchPin3 = 7;
const int dataPin3 = 8;

const int hourpin = 6;
//int datArray[16] = {B11111100, B01100000, B11011010, B11110010, B01100110, B10110110, B10111110, B11100000, B11111110, B11110110};
//                  0           1         2           3           4         5           6         7         8           9
int datArray[16] = {B00000011, B10011111, B00100101, B00001101, B10011001, B01001001, B01000001, B00011111, B00000001, B00001001};
//                  0'           1'         2'           3'        4'         5'          6'         7'         8'           9'

int second0, second1, minute0, minute1, hour0, hour1;
void setup()
{
  Serial.begin(9600);
  Serial.println("Start");
  Wire.begin();

  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //rtc.adjust(DateTime(2022, 4, 12, 11, 30, 0));
  //rtc.adjust(DateTime(year, month, date, hour24, minute, seconds));

  pinMode(clockPin, OUTPUT);

  pinMode(latchPin1, OUTPUT);
  pinMode(dataPin1, OUTPUT);

  pinMode(latchPin2, OUTPUT);
  pinMode(dataPin2, OUTPUT);

  pinMode(latchPin3, OUTPUT);
  pinMode(dataPin3, OUTPUT);
}

void loop()
{
  DateTime now = rtc.now();

  //temperature = DS3231_get_treg(); //Get temperature
  //dtostrf(temperature, 5, 1, tempF);

  hour0 = (now.hour() % 12) % 10;
  hour1 = (now.hour() % 12) / 10;
  Serial.print(String(hour1) + " " + String(hour0) + "\t");

  minute0 = now.minute() % 10;
  minute1 = now.minute() / 10;
  Serial.print(String(minute1) + " " + String(minute0) + "\t");

  second0 = now.second() % 10;
  second1 = now.second() / 10;
  Serial.println(String(second1) + " " + String(second0) + "");

  //Serial.println("Hour : "+String(now.hour())+"\tMinute : "+String(now.minute())+"\tSecond : "+String(now.second()));
  //Serial.println(String(datArray[second0]));

  if (hour1 == 1) {
    digitalWrite(hourpin, HIGH);
  } else {
    digitalWrite(hourpin, LOW);
  }

  digitalWrite(latchPin1, LOW);
  shiftOut(dataPin1, clockPin, MSBFIRST, datArray[hour0]);//1
  digitalWrite(latchPin1, HIGH);

  digitalWrite(latchPin2, LOW);
  shiftOut(dataPin2, clockPin, MSBFIRST, datArray[minute1]);
  digitalWrite(latchPin2, HIGH);

  digitalWrite(latchPin3, LOW);
  shiftOut(dataPin3, clockPin, MSBFIRST, datArray[minute0]);
  digitalWrite(latchPin3, HIGH);



  Serial.println();
  delay(500);
  //rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
}
