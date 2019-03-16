/*
 Name:		espresso.cpp
 Created:	3/14/2019 11:01:21 PM
 Author:	dss 
 */

#include "espresso.h"

ESP8266WebServer* server;
bool APmode;
String localIP;

void setupWebServer() {
	server = getWebServer();

	gpioController();
	webController();
	systemController();
	Serial.println("starting web service...");
	server->begin();
}

void handleWebModeClient() {
	server->handleClient();
	gpioUpdate();
}

void espresso_setup() {
	Serial.begin(115200);
	setAllPinsLow();

	localIP = connectWiFi();
	if (localIP != "") setupWebServer();
	else APmode = connectAPModeCaptiveNetworkServer();
}

void espresso_loop() {
	if (APmode) handleAPmodeClient();
	else handleWebModeClient();
}

