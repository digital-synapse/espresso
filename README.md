# espresso
A simple bootstrap library for the esp8266 to skip the busy work. Usage is dead simple.

    #include <espresso.h>

    void setup() {
	    espresso_setup();
    }
    void loop() {
	    espresso_loop();
    }


This will handle configuration and storage of wifi credentials

* Setup WiFi in AP mode
* Setup captive network / wifi portal
* Host simple web form to setup wifi network / password
* Store network config in EEPROM

Exposes simple REST API to access GPIO and settings

* Reboot and connect to WiFi using stored credentials
* Host simple web server with rest API
* Basic API endpoints to read/write GPIO