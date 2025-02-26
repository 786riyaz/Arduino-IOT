#include <ESP8266WiFi.h>

WiFiClient client;
WiFiServer server(80);

#define led1 D5
#define led2 D4

void setup() {
  Serial.begin(9600);
  WiFi.begin("OPPO A5", "12345678");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }
  Serial.println();
  Serial.println("Connected...");
  Serial.println(WiFi.localIP());
  server.begin();
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);

}

void loop() {
  client = server.available();
  if (client == 1)
  {
    String request = client.readStringUntil('\n');
    Serial.println(request);
    request.trim();

    if(request == "GET /led1on HTTP/1.1")
    {
      digitalWrite(led1, HIGH);
    }
    if(request == "GET /led1off HTTP/1.1")
    {
      digitalWrite(led1, LOW);
    }
    if(request == "GET /led2on HTTP/1.1")
    {
      digitalWrite(led2, HIGH);
    }
    if(request == "GET /led2off HTTP/1.1")
    {
      digitalWrite(led2, LOW);
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
  client.println("<a href=\"/led2on\"\"><button>LED 2 ON</button></a>");
  client.println("<a href=\"/led2off\"\"><button>LED 2 OFF</button></a>");
  client.println("</br>");
  
  client.println("</html>");
  
}
