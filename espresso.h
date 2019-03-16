/*
 Name:		espresso.h
 Created:	3/14/2019 11:01:21 PM
 Author:	dss
*/

#ifndef _espresso_h
#define _espresso_h

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "arduino.h"
	#else
		#include "WProgram.h"
	#endif

	/*** Espresso ***********************************************************/
	void espresso_setup();
	void espresso_loop();

	#include <EEPROM.h>
	#include <ESP8266WiFi.h>
	#include <ESP8266WebServer.h>
	#include <WiFiClient.h>
	
	
	/*** EEPROM Storage *****************************************************/	
	bool writeString(String data, int position);
	String readString(int position);
	bool writeInt(int value, int position);
	int readInt(int position);
	bool writeByte(uint8 value, int position);
	uint8 readByte(int position);


	/*** GPIO Controller ****************************************************/	
	#define READ 0x2
	void gpioController();
	bool getGpioArg(String name, uint8_t &value);
	bool getArg(String name, String &value);
	void gpioUpdate();
	void setAllPinsLow();
	
	/*** System Controller **************************************************/	
	void systemController();
	
	/*** Web Controller *****************************************************/	
	void webController();	
	
	extern ESP8266WebServer *server;
	
	/*** WiFi Setup *********************************************************/	
	String connectWiFi();
	bool connectAPModeCaptiveNetworkServer();
	void handleAPmodeClient();
	void reboot();
	ESP8266WebServer* getWebServer();
	
	/*** DNS Server *********************************************************/
	
	#include <WiFiUdp.h>

	#define DNS_QR_QUERY 0
	#define DNS_QR_RESPONSE 1
	#define DNS_OPCODE_QUERY 0

	enum class DNSReplyCode
	{
		NoError = 0,
		FormError = 1,
		ServerFailure = 2,
		NonExistentDomain = 3,
		NotImplemented = 4,
		Refused = 5,
		YXDomain = 6,
		YXRRSet = 7,
		NXRRSet = 8
	};

	struct DNSHeader
	{
		uint16_t ID;               // identification number
		unsigned char RD : 1;      // recursion desired
		unsigned char TC : 1;      // truncated message
		unsigned char AA : 1;      // authoritive answer
		unsigned char OPCode : 4;  // message_type
		unsigned char QR : 1;      // query/response flag
		unsigned char RCode : 4;   // response code
		unsigned char Z : 3;       // its z! reserved
		unsigned char RA : 1;      // recursion available
		uint16_t QDCount;          // number of question entries
		uint16_t ANCount;          // number of answer entries
		uint16_t NSCount;          // number of authority entries
		uint16_t ARCount;          // number of resource entries
	};

	class DNSServer
	{
	public:
		DNSServer();
		void processNextRequest();
		void setErrorReplyCode(const DNSReplyCode &replyCode);
		void setTTL(const uint32_t &ttl);

		// Returns true if successful, false if there are no sockets available
		bool start(const uint16_t &port,
			const String &domainName,
			const IPAddress &resolvedIP);
		// stops the DNS server
		void stop();

	private:
		WiFiUDP _udp;
		uint16_t _port;
		String _domainName;
		unsigned char _resolvedIP[4];
		int _currentPacketSize;
		unsigned char* _buffer;
		DNSHeader* _dnsHeader;
		uint32_t _ttl;
		DNSReplyCode _errorReplyCode;

		void downcaseAndRemoveWwwPrefix(String &domainName);
		String getDomainNameWithoutWwwPrefix();
		bool requestIncludesOnlyOneQuestion();
		void replyWithIP();
		void replyWithCustomCode();
	};	
#endif

