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

//IP server setings
const char* server = "192.168.4.1"; //IP
const int postingInterval = 1000; // post data every 20 secods

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  Serial.print("Setup done");
}

void loop() {
  // If not connected to WiFi connect
  if (WiFi.status() != WL_CONNECTED) {
    scanWiFi();
    wifiNetworkSelection();
    if (_connect)
      connectToWifi();
  //If already connected
  } else {
    // If there is connection with server
    if (client.connect(server, 80)) {

      //Mesure Signal Strength (RSSI) of Wi-Fi connection
      long rssi = WiFi.RSSI();

      //Info sending
      Serial.println("Connected");
      Serial.println(server);
      for(int i=0; i<10;i++){
        client.write("Hola",sizeof(char)*4);
        delay(100);
      }  
    }
    client.stop();

    //wait and then post again
    delay(postingInterval);
  }
  Serial.println("****************");
  //Wait a bit before scaning again
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
      Serial.println("ERROR: CAN NOT CONNECT TO NETWORK");
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
