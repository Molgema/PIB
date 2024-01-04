#include <WiFi.h>
#include <MQTTPubSubClient.h>
#include <EEPROM.h>
#include <EEPROMRollingCodeStorage.h>
#include <SomfyRemote.h>  

#define EMITTER_GPIO 4
#define LED_GPIO 1
#define EEPROM_ADDRESS 0
#define REMOTE1 0x5181b0
#define REMOTE2 0x62dc00
#define REMOTE3 0x22b5d5
#define REMOTE4 0xc5c78f
#define REMOTE5 0x59614b

EEPROMRollingCodeStorage rollingCodeStorage1(0);
EEPROMRollingCodeStorage rollingCodeStorage2(2);
EEPROMRollingCodeStorage rollingCodeStorage3(4);
EEPROMRollingCodeStorage rollingCodeStorage4(6);
EEPROMRollingCodeStorage rollingCodeStorage5(8);

SomfyRemote somfyRemote1(EMITTER_GPIO, REMOTE1, &rollingCodeStorage1);
SomfyRemote somfyRemote2(EMITTER_GPIO, REMOTE2, &rollingCodeStorage2);
SomfyRemote somfyRemote3(EMITTER_GPIO, REMOTE3, &rollingCodeStorage3);
SomfyRemote somfyRemote4(EMITTER_GPIO, REMOTE4, &rollingCodeStorage4);
SomfyRemote somfyRemote5(EMITTER_GPIO, REMOTE5, &rollingCodeStorage5);

//Home Assistant Initializations
const char* ssid = SSID;
const char* pass = SSID_PW;
const char* address = MQTT_BROKER_ADDRESS;

//State Machine Initializations
typedef enum Enum_Command {Idle, Shutter1, Shutter2, Shutter3, Shutter4, Shutter5} states;
states Current_State = Idle;
int mqttReceive = 0; 
int conShutter = 0; 
String payload = "";
String payloadShutter = "";
String payloadCommand = "";  

WiFiClient client;
MQTTPubSubClient mqtt;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, pass);

  pinMode(EMITTER_GPIO, OUTPUT);
  pinMode(LED_GPIO, OUTPUT); 
	digitalWrite(EMITTER_GPIO, LOW);
  digitalWrite(LED_GPIO, HIGH);

#if defined(ESP32)
	if (!EEPROM.begin(10)) {
		Serial.println("failed to initialise EEPROM");
		delay(1000);
	}
#elif defined(ESP8266)
	EEPROM.begin(10);
#endif

  Serial.print("connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  Serial.print("connecting to host...");
  while (!client.connect(HOST_ADDRESS, 1883)) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  mqtt.begin(client);

  Serial.print("connecting to mqtt broker...");
  while (!mqtt.connect("ESP32", MQTT_BROKER, MQTT_BROKER_PW)) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  mqtt.subscribe(address, [](const String& payload, const size_t size) {
      Serial.print(address);
      Serial.print(": ");
      Serial.println(payload);

      String payloadShutter = payload.substring(0,1);
      String payloadCommand = payload.substring(2);

      ConvertStringShutter(payloadShutter, &mqttReceive, &conShutter);

      if (mqttReceive) {
        Serial.println("Het ingevoerde commando is " + payloadCommand);

        CallStateMachine(payloadCommand, conShutter, Current_State);

        conShutter = 0; 
        Current_State = Idle; 
        mqttReceive = 0;

        Serial.println(conShutter); 
        Serial.println(Current_State); 
      }
    }
  );
}

 void ActionWhileInState (states Current_State, String string) {
  const Command command = getSomfyCommand(string);

  switch (Current_State) {
    case Shutter1: 
      Serial.println("Nu Shutter 1 " + string);
      somfyRemote1.sendCommand(command);
      break;
    case Shutter2: 
      Serial.println("Nu Shutter 2 " + string);
      somfyRemote2.sendCommand(command);
      break;
    case Shutter3: 
      Serial.println("Nu Shutter 3 " + string);
      somfyRemote3.sendCommand(command);
      break;
    case Shutter4: 
      Serial.println("Nu Shutter 4 " + string);
      somfyRemote4.sendCommand(command);
      break; 
    case Shutter5: 
      Serial.println("Nu Shutter 5 " + string);
      somfyRemote5.sendCommand(command);
      break;   
    default: 
      break; 
  }
}

void StateMachine (int event, String string) {
  states Next_State = Current_State; 

  switch (event) {
    case 0:
      Next_State =  Idle;
      break;
    case 1:
      Next_State = Shutter1;
      break; 
    case 2:
      Next_State =  Shutter2;
      break;
    case 3:
      Next_State =  Shutter3;
      break; 
    case 4:
      Next_State =  Shutter4;
      break;
    case 5:
      Next_State =  Shutter5;
      break;  
    default:
      break;         
  }

  if (Current_State != Next_State) {
    Current_State = Next_State;
  }

  ActionWhileInState(Current_State, string);
}

void ConvertStringShutter(String string, int* receiveFlag, int* convertedString) {
  if (string == "1") {
    *convertedString = 1;
    *receiveFlag = 1;
  }
  else if (string == "2") {
    *convertedString = 2;
    *receiveFlag = 1;
  }
  else if (string == "3") {
    *convertedString = 3;
    *receiveFlag = 1;
  }
  else if (string == "4") {
    *convertedString = 4;
    *receiveFlag = 1;
  }
  else if (string == "5") {
    *convertedString = 5;
    *receiveFlag = 1;
  }
  else {
    *convertedString = 0; 
    *receiveFlag = 0;
  }
}

void CallStateMachine(String inputString, int conversionOfString, states Current_State) {
  Serial.println(conversionOfString);
  Serial.println(Current_State);
  Serial.println(inputString);
  StateMachine(conversionOfString, inputString);
    
  Serial.println(conversionOfString); 
  Serial.println(Current_State);
}

void loop() {
  mqtt.update();
  delay(10);
} 
    