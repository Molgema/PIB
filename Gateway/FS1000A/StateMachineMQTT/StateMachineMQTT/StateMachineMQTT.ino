#include <WiFi.h>
#include <MQTTPubSubClient.h>
#include <EEPROM.h>
#include <EEPROMRollingCodeStorage.h>
#include <SomfyRemote.h>  

#define EMITTER_GPIO 2
#define EEPROM_ADDRESS 0
#define REMOTE 0x5182b0
#define SW1 5
#define SW2 18
#define SW3 19
#define ledPin 13

//Home Assistant Initializations
const char* ssid = "PIBTest";
const char* pass = "PIB1234@";
const char* address = "PibNV/Somfy";

//State Machine Initializations
typedef enum Enum_State {Wait, My, Up, MyUp, Down, MyDown, UpDown, Prog} states;
states Current_State = Wait;
int mqttReceive = 0; 
int conString = 0;
String payload = ""; 
String inputProg = ""; 

WiFiClient client;
MQTTPubSubClient mqtt;

//Function prototypes
EEPROMRollingCodeStorage rollingCodeStorage(EEPROM_ADDRESS);
SomfyRemote somfyRemote(EMITTER_GPIO, REMOTE, &rollingCodeStorage);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  pinMode(ledPin, OUTPUT);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);

  somfyRemote.setup();

#if defined(ESP32)
	if (!EEPROM.begin(4)) {
		Serial.println("failed to initialise EEPROM");
		delay(1000);
	}
#elif defined(ESP8266)
	EEPROM.begin(4);
#endif

  Serial.print("connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" connected!");

  Serial.print("connecting to host...");
  while (!client.connect("192.168.1.104", 1883)) {
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
                
      ConvertString(payload, &mqttReceive, &conString);
  
      if (mqttReceive) {
        Serial.println("Het ingevoerde commando is " + payload);
        CallStateMachine(payload, conString, Current_State); 
        //Serial.println(conString);
        //Serial.println(Current_State);
        //Serial.println(string);
        //StateMachine(conString, string);

        conString = 0;
        Current_State = Wait;  
        //payload = "";
        mqttReceive = 0; 

        Serial.println(conString); 
        Serial.println(Current_State); 
      }
    }
  );
 }

 void ActionWhileInState (states Current_State, String string) {
  const Command command = getSomfyCommand(string);

  switch (Current_State) {
    case My: 
      Serial.println("Nu " + string);
      somfyRemote.sendCommand(command);
      break;
    case Up: 
      Serial.println("Nu " + string);
      somfyRemote.sendCommand(command);
      break;
    case MyUp: 
      Serial.println("Nu " + string);
      somfyRemote.sendCommand(command);
      break;
    case Down: 
      Serial.println("Nu " + string);
      somfyRemote.sendCommand(command);
      break; 
    case MyDown: 
      Serial.println("Nu " + string);
      somfyRemote.sendCommand(command);
      break;
    case UpDown: 
      Serial.println("Nu " + string);
      somfyRemote.sendCommand(command);
      break; 
    case Prog: 
      Serial.println("Nu " + string);
      somfyRemote.sendCommand(command);
      break;   
    default: 
      break; 
  }
}

void StateMachine (int event, String string) {
  states Next_State = Current_State; 

  switch (event) {
    case 0:
      Next_State =  Wait;
      break;
    case 1:
      Next_State =  My;
      break; 
    case 2:
      Next_State =  Up;
      break;
    case 3:
      Next_State =  MyUp;
      break; 
    case 4:
      Next_State =  Down;
      break; 
    case 5:
      Next_State =  MyDown;
      break; 
    case 6:
      Next_State =  UpDown;
      break; 
    case 7:
      Next_State =  Prog;
      break;
    default:
      break;         
  }

  if (Current_State != Next_State) {
    Current_State = Next_State;
  }

  ActionWhileInState(Current_State, string);
}

void ConvertString(String string, int* receiveFlag, int* convertedString) {
  //convertedString = 0; 
  //receiveFlag = 0; 

  if (string == "Wait") {
    *convertedString = 0;
    *receiveFlag = 1;
  }
  else if (string == "My") {
    *convertedString = 1;
    *receiveFlag = 1;
  }
  else if (string == "Up") {
    *convertedString = 2;
    *receiveFlag = 1;
  }
  else if (string == "MyUp") {
    *convertedString = 3;
    *receiveFlag = 1;
  }
  else if (string == "Down") {
    *convertedString = 4;
    *receiveFlag = 1;
  }
  else if (string == "MyDown") {
    *convertedString = 5;
    *receiveFlag = 1;
  }
  else if (string == "UpDown") {
    *convertedString = 6;
    *receiveFlag = 1;
  }
  else if (string == "Prog") {
    *convertedString = 7;
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
  mqtt.update();  // Print de input

  if (!digitalRead(SW2)) {
    inputProg = "Prog";
    Serial.println("Switch Prog pressed");
  }
  else {
    inputProg = ""; 
  }

  ConvertString(inputProg, &mqttReceive, &conString);

  if (mqttReceive) {
    Serial.println("Het ingevoerde commando is " + inputProg);
    CallStateMachine(inputProg, conString, Current_State); 
    //Serial.println(conString);
    //Serial.println(Current_State);
    //Serial.println(string);
    //StateMachine(conString, string);

    conString = 0;
    Current_State = Wait;  
    inputProg = ""; 
    mqttReceive = 0; 

    Serial.println(conString); 
    Serial.println(Current_State); 

    while(!digitalRead(SW2)) {};
  }

  delay(10);
} 
    