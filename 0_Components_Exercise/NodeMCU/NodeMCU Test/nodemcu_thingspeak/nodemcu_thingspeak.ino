#include<ESP8266WiFi.h>
#include<ThingSpeak.h>

WiFiClient client;
WiFiServer server(80);

long channelNumber = 1275348;
const char WriteAPI[] = "U0MDGBIHB1KO3XDV";

//LG3RA15R63SZLCBJ

#define led D5

void setup() {
  Serial.begin(9600);
  WiFi.begin("OPPO A5", "12345678");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected...");
  Serial.println(WiFi.localIP());
  pinMode(D4,OUTPUT);
  digitalWrite(D4,LOW);
  server.begin();
  ThingSpeak.begin(client);
  pinMode(led, OUTPUT);

  
}

void loop() {
  client = server.available();
  if (client == 1)
  {
    String request = client.readStringUntil('\n');
    Serial.println(request);
    request.trim();

    if (request == "GET /led1on HTTP/1.1")
    {
      digitalWrite(led, HIGH);
      Serial.println("LED ON");
      ThingSpeak.writeField(channelNumber, 1, 1, WriteAPI);
    }
    if (request == "GET /led1off HTTP/1.1")
    {
      digitalWrite(led, LOW);
      Serial.println("LED OFF");
      ThingSpeak.writeField(channelNumber, 1, 0, WriteAPI);
    }
  }
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");

  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.println("<h1> Welcome</h1>");
  client.println("<br>");
  client.println("<a href=\"/led1on\"\"><button>LED 1 ON</button></a>");
  client.println("<a href=\"/led1off\"\"><button>LED 1 OFF</button></a>");
  //client.println("<a href=\"/led2on\"\"><button>LED 2 ON</button></a>");
  //client.println("<a href=\"/led2off\"\"><button>LED 2 OFF</button></a>");
  client.println("</br>");
  client.println("</html>");
  

}
