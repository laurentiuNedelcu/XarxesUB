#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <math.h>

String ssid, password, s, option, _exit_;
char* c_ssid;
char* c_pwd;
int contador = 0, n;
boolean _connect, check;
int rssi[5];
float avg;

WiFiClient _client;

//IP server setings
//const char* server = "192.168.4.2"; //IP
IPAddress server(192, 168, 4, 1);


void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  delay(2000);
  Serial.println("Setup done");
}

void loop() {
  // put your main code here, to run repeatedly:
  menu();
  while (!Serial.available());
  option = Serial.readStringUntil('\n');
  switch (atoi(option.c_str()))
  {
    case 1:
      if (WiFi.status() != WL_CONNECTED) {
        wifiNetworkSelection();
        if (_connect)
          connectToWifi();
      } else {
        Serial.println("You are already connected to Network!");
      }
      break;
    case 2:
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Disconnecting...");
        WiFi.disconnect();
        free(c_ssid);
        free(c_pwd);
        delay(2000);
        Serial.println("You have disconnected.");
      } else {
        Serial.println("You are already disconnected!");
      }
      break;
    case 3:
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Taking measurements...");
        avg = 0.0;
        for (int i = 0; i < 5; i++) {
          rssi[i] = WiFi.RSSI();
          avg = avg + rssi[i];
          delay(1000);
        }
        avg = avg / 5;
        Serial.println("Measurements taken.");
        Serial.print("RSSI -> ");
        for (int i = 0; i < 5; i++) {
          Serial.print(rssi[i]);
          Serial.print("db ");
        }
        Serial.println();
        Serial.print("Average: ");
        Serial.println(avg);
      } else {
        Serial.println("Please, first connect to Network.");
      }
      break;
    case 4:
      if (WiFi.status() == WL_CONNECTED) {
        check = false;
        while (!check) {
          serializable();
          Serial.println("Do you want to exit? Press (y) to exit. Press any other button to not.");
          while (!Serial.available());
          _exit_ = Serial.readStringUntil('\n');
          if (_exit_ == "Y" or _exit_ == "y")
            check = true;
        }
      } else {
        Serial.println("Please, first connect to Network.");
      }
      break;
    default:
      Serial.println("Invalid option. Please, select a number.");
      break;
  }
}

void menu() {
  Serial.println();
  Serial.println("-------------- MENU ------------");
  Serial.println("1. Connect to Network.");
  Serial.println("2. Disconnect to Network.");
  Serial.println("3. Measure RSSI from Network.");
  Serial.println("4. Send data to server.");
  Serial.println();
}

void serializable() {
  Serial.println(_client.connect(server, 80));
  if (_client.connect(server, 80)) {

    StaticJsonDocument<128> doc;
    char buf[100];
    gcvt(avg, 4, buf);

    doc["text"] = "The RSSI I receive from your WiFi is: ";
    doc["avg"] = buf;

    JsonArray data = doc.createNestedArray("data");
    for (int i = 0; i < 5; i++)
    {
      data.add(rssi[i]);
    }

    serializeJson(doc, (Client&)_client);
    //client.write(doc);
  } else {
    Serial.println("CANNOT CONNECTED TO SERVER!");
  }
  _client.stop();
  delay(1000);
}

void connectToWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(c_ssid, c_pwd);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
      contador++;
      if (contador == 15) {
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
