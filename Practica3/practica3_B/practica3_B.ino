#include <ESP8266WiFi.h>

char ssid[] = "Tesla WiFi"
char password[] = "12345678"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("WiFi access point test");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid,password);
  Serial.println("WiFi ON"); 
}

void loop() {
  // put your main code here, to run repeatedly:

}
