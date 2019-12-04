#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

char ssid[] = "Teslawifi";
char password[] = "elonmusk";
WiFiServer server(80);
WiFiClient client;
char* json;
int rssi[5];
float avg;

void setup() {
  Serial.begin(9600);
  server.begin();
  Serial.println("WiFi access point test");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid,password);
  Serial.println("WiFi ON");
  Serial.print("SSID -> ");
  Serial.println(ssid);
  Serial.print("Password -> ");
  Serial.println(password);
  IPAddress IP = WiFi.softAPIP();
  //Serial.flush();
  Serial.println();
  Serial.print("Server IP is: ");
  Serial.println(IP); 
}

void loop() {
  client = server.available();
  if (client) {
    Serial.println("Reading...");
    String str_json = client.readStringUntil('\n');
    Serial.println(str_json);
    Serial.println("STR TO CHAR ...");
    json = strTochar(str_json, json);
    
    deserialize();
    delay(1000);
  }
  delay(50);
}

void deserialize() {
  Serial.println("Deserializing...");
  StaticJsonDocument<200> doc;

  deserializeJson(doc, json);

  Serial.println("  Deserializing... TEXT");
  const char* text = doc["text"]; // "The RSSI I receive from your WiFi is: "
  Serial.println("  Deserializing... AVG");
  const char* avg_client = doc["avg"]; // "float"
  avg_client = ("%.1f",avg_client);
  Serial.println(text);
  Serial.print("Average: ");
  Serial.println(avg_client);

  JsonArray data = doc["data"];
  
  Serial.println("RSSI mesured: ");
  Serial.print("[ ");
  avg = 0.0;
  for (int i = 0; i < 5; i++) {
    rssi[i] = data[i];
    avg += rssi[i];
    Serial.print(rssi[i]);
    Serial.print(", ");
  }
  Serial.println(" ]");
  avg /= 5;
  avg = ("%.1f",avg);
  Serial.println();
  Serial.print("The average mesure of the data recived is:");
  Serial.println(avg);
  if((avg-atof(avg_client))<0.01 || (avg-atof(avg_client)>0.01)){
    Serial.println("The mean recived and the one we calculated coincide");
  }else{
    Serial.println("The mean recived and the one we calculated do not coincide");
    Serial.println("Probably the data recived was incorrect. This iteration wont be saved on the DB.");
  }
  Serial.println();
  Serial.println("Waiting for new data");
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
