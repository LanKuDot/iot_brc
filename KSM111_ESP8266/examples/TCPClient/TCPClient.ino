/* A TCP client.
 */
#include <KSM111_ESP8266.h>

#define UNO

#ifdef UNO
 KSM111_ESP8266 wifi(3, 2);	// USe SoftwareSerial: Rx pin 3, Tx pin 2.
#else
 KSM111_ESP8266 wifi(&Serial1);	// Use HardwareSerial.
#endif

 // You have to modify the following parameters.
#define AP_SSID         "AP_SSID"
#define AP_PASSWD       "AP_PASSWD"
#define TCP_SERVER_IP   "SERVER_IP"
#define TCP_SERVER_PORT 8888

void setup()
{
	char joinedSSID[32];

	// Initialize the UART
	Serial.begin(9600);
	while (!Serial)
		;
	wifi.begin(9600);

	Serial.print("Connecting to the AP... ");
	// Check if the module joined an AP.
	if (!wifi.joinedAP(joinedSSID) ||
	    strcmp(joinedSSID, AP_SSID) != 0) {
		// Quit the current connected AP and connect to the new one.
		wifi.quitAP();
		wifi.setMode(STATION);
		wifi.multiConnect(false);
		if (wifi.joinAP(AP_SSID, AP_PASSWD) < 0) {
			// Cannot connect to the AP, stop.
			Serial.println("Fail");
			while (1)
				;
		}
	}

	Serial.print("OK\nJoining to the TCP server... ");
	if (wifi.beginClient("TCP", TCP_SERVER_IP, TCP_SERVER_PORT) != CONNECT_ERROR)
		Serial.println("OK");
	else {
		// Cannot join to the TCP server, stop.
		Serial.println("Fail");
		while (1)
			;
	}
}

static char buffer[32];

void loop()
{
	int charAvail;

	if ((charAvail = Serial.available()) > 0) {
		for (int i = 0; i < charAvail; ++i)
			buffer[i] = Serial.read();
		buffer[charAvail] = '\0';

		wifi.puts(buffer);
	}

	// gets() would return the ID of the sender.
	if (wifi.gets(buffer, 32) >= 0) {
		Serial.println(buffer);

		if (strcmp(buffer, "end") == 0) {
			Serial.println("Quit from the TCP server.");
			wifi.endClient();
			while (1)
				;
		}
	}
}
