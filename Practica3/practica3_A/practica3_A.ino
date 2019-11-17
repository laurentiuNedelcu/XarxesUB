#include "ESP8266WiFi.h"
#include <math.h>

// Wi-Fi Settigs
String ssid, password, s;
String _disconnect;
char* c_ssid;
char* c_pwd;
int contador = 0, n;
boolean _connect;

WiFiClient client;

//ThigSpeak Settigs
int channelID = 912129;
String writeAPIKey = "3U1JOI6BCETBKJ24"; // write API key for your ThigSpeak Chael
const char* server = "api.thingspeak.com";
const int postingInterval = 20 * 1000; // post data every 20 secods

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  Serial.print("Setup done");
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    scanWiFi();
    wifiNetworkSelection();
    if (_connect)
      connectToWifi();
  } else {
    if (client.connect(server, 80)) {

      //Mesure Signal Strength (RSSI) of Wi-Fi connection
      long rssi = WiFi.RSSI();

      //Construct API request body
      String body = "field1=";
      body += String(rssi);

      Serial.print("RSSI: ");
      Serial.println(rssi);

      client.println("POST /update HTTP/1.1");
      client.println("Host: api.thingspeak.com");
      client.println("User-Agent: ESP8266 (nothans)/1.0");
      client.println("Connection: close");
      client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Content-Length: " + String(body.length()));
      client.println("");
      client.print(body);
    }
    client.stop();

    //wait and then post again
    delay(postingInterval);
  }
  Serial.println("****************");
  //Wait a bit before scaig agai
  delay(1000);

}

void scanWiFi() {
  Serial.println("Scan start");

  int n = WiFi.scanNetworks();
  Serial.println("Scan done");

  if (n == 0) {
    Serial.println("no networks found");
  } else {
    for (int i = 0; i < n; i++) {
      Serial.print(i + 1);
      Serial.print(" ");
      Serial.print(WiFi.SSID(i));
      Serial.print("(");
      int rssi = WiFi.RSSI(i);
      Serial.print(rssi);
      Serial.print(")");
      Serial.print((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      Serial.println();
      delay(10);
    }
  }
}

void connectToWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(c_ssid, c_pwd);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
      contador++;
      if (contador == 10) {
        contador = 0;
        break;
      }
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) { //Check: https://www.arduino.cc/en/Reference/WiFiStatus for more details
      Serial.print("WiFi local IP > ");
      Serial.println(WiFi.localIP());
    }
    else {
      Serial.println("ERROR: CAN NOT CONNECTED TO NETWORK");
    }
  }
}

void wifiNetworkSelection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("SSID? >");
    while (!Serial.available());
    ssid = Serial.readStringUntil('\n');
    c_ssid = strTochar(ssid, c_ssid);
    Serial.print("SSID selected > ");
    Serial.println(c_ssid);
    delay(1000);
    _connect = ssidAvailable();
    if (_connect) {
      Serial.println("Password? >");
      while (!Serial.available());
      password = Serial.readStringUntil('\n');
      c_pwd = strTochar(password, c_pwd);
      Serial.print("Password > ");
      Serial.println(c_pwd);
      delay(1000);
    }
  }
}

char* strTochar(String txt, char* c) {
  if (WiFi.status() != WL_CONNECTED) {
    int len = txt.length();
    c = (char*)malloc(len);
    strncpy(c, txt.c_str(), len);
    c[len] = '\0';

    return c;
  }
  return NULL;
}

boolean ssidAvailable() {
  Serial.println("Comproving if the SSID is available...");
  n = WiFi.scanNetworks();
  if (n == 0)
    Serial.println("No networks found!");
  else {
    for (int i = 0; i < n; i++) {
      s = WiFi.SSID(i);
      if (strncmp(s.c_str(), c_ssid, s.length()) == 0) {
        Serial.println("SSID available: OK");
        return true;
      }
    }
  }
  Serial.println("ERROR: SSID IS NOT AVAILABLE");
  return false;
}
