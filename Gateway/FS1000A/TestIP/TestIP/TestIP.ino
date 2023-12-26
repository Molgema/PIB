#include <WiFi.h>

const char* ssid = "PIBTest";
const char* password = "PIB1234@";

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
