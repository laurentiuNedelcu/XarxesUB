#include <ESP8266WiFi.h>

char ssid[] = "Tesla WiFi";
char password[] = "12345678";
WiFiServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.println("WiFi access point test");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid,password);
  server.begin();
  Serial.begin(9600);
  Serial.println("WiFi ON");
  IPAddress IP = WiFi.softAPIP();
  //Serial.flush();
  Serial.println();
  Serial.print("Server IP is: ");
  Serial.println(IP);
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();

  if(client){
    Serial.println(client.readStringUntil('\n'));
    delay(100);
  }
}
