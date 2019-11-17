#include "ESP8266WiFi.h"
String ssid, password, s;
String _disconnect;
char* c_ssid;
char* c_pwd;
int contador = 0, n;
boolean check, _connect;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  Serial.println("Setup done");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiNetworkSelection();
    if (_connect)
      connectToWifi();
  } else {
    check = false;
    while (!check) {
      /*Serial.println("Do you want do disconnect? (y/n)");
      while (!Serial.available());
      _disconnect = Serial.readStringUntil('\n');
      if (_disconnect != "y" and _disconnect != "Y" and _disconnect != "n" and _disconnect != "N") {
        Serial.println("ERROR: Please write 'y' or 'n'");
      } else {
        if (_disconnect == "y" or  _disconnect == "Y") {
          WiFi.disconnect();
          free(c_ssid);
          free(c_pwd);
          delay(2000);
        } else {
          delay(5000);
        }
        check = true;
      }*/
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
