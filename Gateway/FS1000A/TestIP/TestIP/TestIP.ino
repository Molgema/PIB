#include <WiFi.h>

const char* ssid = "HotspotTest";
const char* password = "8c196696f872";

void initWifi();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initWifi();
}

void initWifi() {
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); 
  Serial.print("Connecting to WiFi .."); 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
}
