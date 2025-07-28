#include<ESP8266WiFi.h>
#include<ThingSpeak.h>
#include<SoftwareSerial.h>
SoftwareSerial s(D6,D5);

WiFiClient client;

long channelNumber = 1275348;
const char WriteAPI[] = "U0MDGBIHB1KO3XDV";
const char ReadAPI[] = "LG3RA15R63SZLCBJ";
int data = 50;

void setup() 
{
  Serial.begin(19200);
  //WiFi.begin("OPPO A5", "12345678");
  WiFi.begin("Shahin Foundation", "shahin.foundation");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected...");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);
  s.begin(9600);
}

void loop() 
{ 
  s.write("s");
  if(s.available() > 0)
  {
    data = s.read();
    Serial.println(data); 
    ThingSpeak.writeField(channelNumber, 1, data, WriteAPI);
    delay(1000*20);
  }
}
