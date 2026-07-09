#include<ESP8266WiFi.h>
#define led D4

WiFiClient myClient;
WiFiServer myServer(80);

void setup() 
{
  WiFi.begin("Shahin Foundation","shahin.foundation");
  //WiFi.begin("OPPO A5","12345678");
  Serial.begin(9600);
  
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("..");
    delay(200);
  }

  Serial.println();
  Serial.println("MCU Connected!!!");
  Serial.println(WiFi.localIP());
  myServer.begin();

  pinMode(led,OUTPUT);
}

void loop() 
{
   myClient = myServer.available();
   if (myClient == 1)
   {
      String request = myClient.readStringUntil('\n');
      Serial.println(request);  
      request.trim(); // to remove grabage value

      if(request == "GET /ON HTTP/1.1")
      {
        Serial.println("ON");
        digitalWrite(led,HIGH);
      }
      
      if(request == "GET /OFF HTTP/1.1")
      {
         Serial.println("OFF");
        digitalWrite(led,LOW);
      }
   }
}
