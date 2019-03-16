#include "espresso.h"
unsigned long timeout[17];


void gpioController() {

	// advanced gpio
	server->on("/gpio", HTTP_GET, []() {

		uint8_t gpio;
		String value;
		for (int i = 1; i <= 16; i++)
		{
			// get gpioN
			if (getGpioArg("gpio" + String(i), gpio)) {
				if (gpio == READ) {
					pinMode(i, INPUT);
					int result = digitalRead(i);
					Serial.println("READ GPIO " + String(i) + " = " + String(result));
					server->send(200, "application/json", String(result));
				}
				else {
					pinMode(i, OUTPUT);
					digitalWrite(i, gpio);
					Serial.println("WRITE GPIO " + String(i) + " = " + String(gpio));

					// get timeN to shut off pin
					if (getArg("time" + String(i), value)) {
						timeout[i] = millis() + value.toInt() * 1000;
					}
				}
			}
		}
		server->send(200);
	});
}

bool getGpioArg(String name, uint8_t &value) {
	String val = server->arg(name);
	if (val == "") return false;
	if (val == "0" || val == "low" || val == "off" || val == "l") {
		value = LOW;
		return true;
	}
	if (val == "1" || val == "high" || val == "on" || val == "h") {
		value = HIGH;
		return true;
	}
	if (val == "-1" || val == "read" || val == "r") {
		value = READ;
		return true;

	}
	return false;
}
bool getArg(String name, String &value) {
	String val = server->arg(name);
	if (val == "") return false;
	else {
		value = val;
		return true;
	}
	return false;
}

void gpioUpdate() {
	for (int i = 0; i < 16; i++) {
		if (timeout[i] != 0) {
			if (millis() > timeout[i]) {
				timeout[i] = 0;

				int pin = digitalRead(i);
				if (pin == HIGH) digitalWrite(i, LOW);
				else digitalWrite(i, HIGH);
			}
		}
	}
}


void setAllPinsLow() {
	for (int i = 1; i < 17; i++)
	{
		pinMode(0, OUTPUT);
		digitalWrite(i, LOW);
	}
}