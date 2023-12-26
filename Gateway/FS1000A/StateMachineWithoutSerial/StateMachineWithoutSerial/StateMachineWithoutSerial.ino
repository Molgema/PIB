#include <EEPROM.h>
#include <EEPROMRollingCodeStorage.h>
#include <SomfyRemote.h>

#define EMITTER_GPIO 2
#define EEPROM_ADDRESS 0
#define REMOTE 0x5182b0
#define SW1 5
#define SW2 18
#define SW3 19

EEPROMRollingCodeStorage rollingCodeStorage(EEPROM_ADDRESS);
SomfyRemote somfyRemote(EMITTER_GPIO, REMOTE, &rollingCodeStorage);

typedef enum Enum_State {Wait, My, Up, MyUp, Down, MyDown, UpDown, Prog} states;
states Current_State = Wait;
int mqttReceive = 0; 
int conString = 0;
String payload = ""; 

void setup() {
  Serial.begin(115200); 

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
  if (!digitalRead(SW1)) {
    payload = "Down";
    Serial.println("Switch Up pressed"); 
  }
  else if (!digitalRead(SW2)) {
    payload = "Prog";
    Serial.println("Switch Prog pressed"); 
  }
  else if (!digitalRead(SW3)) {
    payload = "Up";
    Serial.println("Switch Down pressed"); 
  }
  else {
    payload = ""; 
  }

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
    payload = "";
    mqttReceive = 0; 

    Serial.println(conString); 
    Serial.println(Current_State); 

    while (!digitalRead(SW1) || !digitalRead(SW2) || !digitalRead(SW3)) {};
  }   
}
