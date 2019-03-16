
#include "espresso.h"
#include "base64.h"

const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
IPAddress         apIP(10, 10, 10, 1);    // Private network for server
DNSServer         dnsServer;              // Create the DNS object
ESP8266WebServer  webServer(80);          // HTTP server

/*
String loginHTML = ""
"<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
"<h1>Device Setup</h1><p>Please enter your wirless network info below."
"</p><form action=\"/\" method=\"post\">"
"<input type=\"text\" placeholder=\"Enter SSID\" name=\"ssid\" required>"
"<input type=\"text\" placeholder=\"Enter PASSWORD\" name=\"pass\" required>"
"<button type=\"submit\" value=\"Send\">Login</button>"
"</form></body></html>";
*/
String loginHTML = ""
"<!DOCTYPE html><html><head><title>CaptivePortal</title>"
"<style>"
"	div{ padding-bottom: 10px; }"
"	.tc_settings { display: none; required: false; }"
"	td, h1, h2, h3, h4, h5, p, ul, ol, li{page-break-inside: avoid;}body{font-size: 12px; font-family: Helvetica, Arial;}h1, h2{border-bottom: 1px solid #DDDDDD;}pre, code{font-family: Consolas, 'Liberation Mono', Courier; background-color: #F4F4F4; border: 1px solid #EAEAEA; border-radius: 2px; padding: 5px;}"
"	td { padding-right: 10px; vertical-align: top; }"
"	input[type='checkbox'] { vertical-align: middle; }"
"</style>"
"<script>"
"function show_tc_settings(){"
"	var settings =document.getElementsByClassName('tc_settings');"
"	for (var i=0; i<settings.length; i++){"
"		var setting = settings[i];"
"		if (document.getElementById('cb_tc').checked){"
"			setting.style.display='block';"
"			setting.required=true;"
"		}"
"		else{"
"			setting.style.display='none';"
"			setting.required=false;"
"		}"
"	}"
"}"
"</script>"
"</head><body>"
"<h1>Device Setup</h1><p>Please enter your wirless network info below."
"</p><small><form action='/' method='post'>"
"<div>Wifi SSID<br/><input type='text' name='ssid' required></div>"
"<div>Wifi Password<br/><input type='text' name='pass' required></div>"
"<br /><button type='submit' value='Send'>Login</button>"
"</form></small></body></html>";

String submitHTML = ""
"<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
"<h1>Thank You!</h1><p>Please wait while the device attempts to connect to the AP...</p>"
"</body></html>";

ESP8266WebServer* getWebServer() {
	return &webServer;
}

String connectWiFi() {

	//force AP mode (hold flash button during power on)
	pinMode(0, INPUT_PULLUP);
	bool flash = digitalRead(0)==0;
	String ssid = flash ? "" : readString(0);
	String pass = flash ? "" : readString(1);

	if (ssid == "" || pass == "") return "";

	WiFi.begin(ssid, pass);             // Connect to the network
	Serial.print("Connecting to ");
	Serial.print(ssid); Serial.println(" ...");
	//Serial.println(pass);

	// Wait 10 seconds for the Wi-Fi to connect
	int i = 0;
	while (WiFi.status() != WL_CONNECTED && i < 10) {
		delay(1000);
		Serial.print(++i); Serial.print(' ');
	}
	Serial.println('\n');
	if (WiFi.status() == WL_CONNECTED) {
		Serial.println("Connection established!");
		Serial.print("IP address:\t");
		Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
		return WiFi.localIP().toString();
	}

	Serial.println("WiFi connection failed...");
	WiFi.disconnect(true);
	WiFi.setAutoConnect(false);

	return "";
}

bool connectAPModeCaptiveNetworkServer() {
	// WIFI connection failed, start AP mode
	Serial.println("starting AP mode...");
	WiFi.mode(WIFI_AP);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	WiFi.softAP("ESP8266 WiFi Setup");

	// if DNSServer is started with "*" for domain name, it will reply with
	// provided IP to all DNS request
	Serial.println("starting dns server...");
	dnsServer.start(DNS_PORT, "*", apIP);

	webServer.on("/", HTTP_POST, []() {
		webServer.send(200, "text/html", submitHTML);

		String ssid = webServer.arg("ssid");
		String pass = webServer.arg("pass");
		Serial.println("ssid: " + ssid);
		Serial.println("pass: " + pass);
		writeString(ssid, 0);
		writeString(pass, 1);

		// get teamcity settings
		String tc_ip = webServer.arg("tc_ip");
		Serial.println("tc_ip: " + tc_ip);
		writeString(tc_ip, 2);
		if (tc_ip != "") {
			String tc_user = webServer.arg("tc_user");
			String tc_pass = webServer.arg("tc_pass");
			Serial.println("tc_user: " + tc_user);
			Serial.println("tc_pass: " + tc_pass);
			base64 b64;
			String tc_auth = b64.encode(tc_user + ":" + tc_pass, false);
			writeString(tc_auth, 3);
			String tc_project = webServer.arg("tc_project");
			Serial.println("tc_project: " + tc_project);
			writeString(tc_project, 4);

			int freq = webServer.arg("tc_frequency").toInt();
			if (freq == 0) freq = 60;
			Serial.println("tc_frequency: " + String(freq));
			writeInt(freq, 7);
			int timeout = webServer.arg("tc_timeout").toInt();
			Serial.println("tc_timeout: " + String(timeout));
			writeInt(timeout, 8);

			bool enableGreen = webServer.arg("tc_enable_green") == "on";
			Serial.println("tc_enable_green: " + String(enableGreen));
			bool enableYellow = webServer.arg("tc_enable_yellow") == "on";
			Serial.println("tc_enable_yellow: " + String(enableYellow));
			bool enableRed = webServer.arg("tc_enable_red") == "on";
			Serial.println("tc_enable_reed: " + String(enableRed));
			uint8 flags = 0;
			if (enableGreen) flags |= 0x1;
			if (enableYellow) flags |= 0x2;
			if (enableRed) flags |= 0x4;
			writeByte(flags, 0);
		}

		delay(2000);
		reboot();
	});

	// replay to all requests with same HTML
	webServer.onNotFound([]() {
		webServer.send(200, "text/html", loginHTML);
	});

	Serial.println("starting web server...");
	webServer.begin();

	return true;
}

void reboot() {
	WiFi.forceSleepBegin(); wdt_reset(); ESP.restart(); while (1)wdt_reset();
}

void handleAPmodeClient() {
	dnsServer.processNextRequest();
	webServer.handleClient();
}