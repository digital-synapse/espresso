
#include "espresso.h"
#include "base64.h"

const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
IPAddress         apIP(10, 10, 10, 1);    // Private network for server
DNSServer         dnsServer;              // Create the DNS object
ESP8266WebServer  webServer(80);          // HTTP server

String loginHTML1 = "<!DOCTYPE html><html><head><title>CaptivePortal</title>"
"<style> div{ padding-bottom: 10px; } .tc_settings { display: none; required: false; } td, h1, h2, h3, h4, h5, p, ul, ol, li{page-break-inside: avoid;}body{font-size: 12px; font-family: Helvetica, Arial;}h1, h2{border-bottom: 1px solid #DDDDDD;}pre, code{font-family: Consolas, 'Liberation Mono', Courier; background-color: #F4F4F4; border: 1px solid #EAEAEA; border-radius: 2px; padding: 5px;} td { padding-right: 10px; vertical-align: top; } input[type='checkbox'] { vertical-align: middle; }</style>"
"<script> (function() {function getSelectedOption(sel) { var opt; for ( var i = 0, len = sel.options.length; i < len; i++ ) { opt = sel.options[i]; if ( opt.selected === true ) { break; }} return opt; } document.addEventListener('DOMContentLoaded', function(event) { var ssids =document.getElementById('ssids'); var ssid =document.getElementById('ssid'); ssid.style.display='none'; ssids.onchange = function () { var opt = getSelectedOption(ssids); if (opt.value == 'custom') ssid.style.display='block'; else { ssid.style.display='none'; ssid.value = opt.value; } }; ssids.onchange(); }); })(); </script>"
"</head><body><h1>Device Setup</h1><p>Please enter your wirless network info below.</p><small><form action='/' method='post'><div>Wifi SSID<br/><select id='ssids' name='ssids' required>";
String loginHTML2 = "<option value='custom'>Enter Custom</option></select></div><div><input type='text' id='ssid' name='ssid' required></div><div>Wifi Password<br/><input type='text' name='pass' required></div><br /><button type='submit' value='Send'>Login</button></form></small></body></html>";
String submitHTML = "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body><h1>Thank You!</h1><p>Please wait while the device attempts to connect to the AP...</p></body></html>";

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

String options = "";
bool connectAPModeCaptiveNetworkServer() {
	
	// WIFI onnection failed, scan for SSIDs	
	Serial.println("scanning networks...");		
	int numberOfNetworks = WiFi.scanNetworks();		
	int maxSignal = -100;
	String option;
	for (int i = 0; i < numberOfNetworks; i++) {
		int signal = WiFi.RSSI(i);
		option = "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) +/*" " + String(signal) + "dBm*/"</option>";
		if (signal > maxSignal) {
			options =  option + options;
			maxSignal = signal;
		}
		else options = options + option;
	}	

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
		
		delay(2000);
		reboot();
	});

	// replay to all requests with same HTML
	webServer.onNotFound([]() {
		webServer.send(200, "text/html", loginHTML1 + options + loginHTML2);
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