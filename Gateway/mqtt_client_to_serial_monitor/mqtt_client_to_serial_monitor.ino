#include <WiFi.h>
#include <MQTTPubSubClient.h>

const char* ssid = "superdepuper2.4G";
const char* pass = "password_not1found!";
const char* address = "PibNV/Somfy";

const int ledPin = 2;
WiFiClient client;
MQTTPubSubClient mqtt;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, pass);

    pinMode(ledPin, OUTPUT);

    Serial.print("connecting to wifi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" connected!");

    Serial.print("connecting to host...");
    while (!client.connect("192.168.1.58", 1883)) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" connected!");

    mqtt.begin(client);

    Serial.print("connecting to mqtt broker...");
    while (!mqtt.connect("ESP32", "MQTT_User", "ashu3312mqtt")) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" connected!");
   

    mqtt.subscribe(address, [](const String& payload, const size_t size) {
        Serial.print(address);
        Serial.print(": ");
        Serial.println(payload);
                
        if (payload == "on") {
          Serial.println("De LED wordt op ON gezet.");
          digitalWrite(ledPin, HIGH);
        } else if (payload == "off") {
          Serial.println("De LED wordt op ON gezet.");
          digitalWrite(ledPin, LOW);
       }
     }
    );
 }

void loop() {
     mqtt.update();  // Print de input
     delay(10);
} 
    