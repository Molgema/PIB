#include <EEPROM.h>
#include <EEPROMRollingCodeStorage.h>
#include <SomfyRemote.h>

#define EMITTER_GPIO 4
#define EEPROM_ADDRESS 0
#define REMOTE 0x5122b0

EEPROMRollingCodeStorage rollingCodeStorage(EEPROM_ADDRESS);
SomfyRemote somfyRemote(EMITTER_GPIO, REMOTE, &rollingCodeStorage);

void setup() {
	Serial.begin(115200);

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

void loop() {
	if (Serial.available() > 0) {
		const String string = Serial.readStringUntil('\n');
		const Command command = getSomfyCommand(string);
    Serial.println(string); 
		somfyRemote.sendCommand(command);
#ifdef DEBUG
		Serial.println("finished sending");
#endif
	}
}