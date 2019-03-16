#include "espresso.h"

// [0-6] allows 7 strings max, 64 characters each
bool writeString(String data, int position) {		
	if (position < 0 || position > 6) return false;
	byte len = byte(data.length());
	if (len > 64) return false;
	EEPROM.begin(512);
	//EEPROM.write(position, len);
	for (int n = 0; n < len; n++) {
		EEPROM.write(n+(position*64), data[n]);
	}
	for (int n = len; n < 64; n++) {
		EEPROM.write(n + (position * 64), ' ');
	}
	EEPROM.commit();
	return true;
}

// [0-6] allows 7 strings max, 64 characters each
String readString(int position) {
	if (position < 0 || position > 6) return "";
	EEPROM.begin(512);	
	String temp;
	for (int n = 0; n < 64; n++) {
		temp = temp + char(EEPROM.read(n + (position * 64)));
	}
	temp.trim();
	return temp;
}

// [7-8] allow 2 16 bit integers
bool writeInt(int value, int position) {
	if (position < 7 || position > 8) return false;
	position -= 7;
	EEPROM.begin(512);		
	EEPROM.write(448 + (position * 2), value & 0xFF);	
	EEPROM.write(449 + (position * 2), (value >> 8) & 0xFF);
	EEPROM.commit();
	return true;
}
// [7-8] allow 2 16 bit integers
int readInt(int position) {
	if (position < 7 || position > 8) return false;
	position -= 7;
	EEPROM.begin(512);
	uint value;
	value = EEPROM.read(448 + (position * 2));
	value |= ((uint)EEPROM.read(449 + (position * 2)) << 8);
	return (int)value; // dont think i need the case but just in case
}

// [9-12] allow 4 bytes
bool writeByte(uint8 value, int position) {
	if (position < 9 || position > 12) return false;
	position -= 9;
	EEPROM.begin(512);
	EEPROM.write(480 + position, value);
	EEPROM.commit();
	return true;
}
// [9-12] allow 4 bytes
uint8 readByte(int position) {
	if (position < 9 || position > 12) return false;
	position -= 9;
	EEPROM.begin(512);
	uint value;
	value = EEPROM.read(480 + position);
	return (int)value; // dont think i need the case but just in case
}