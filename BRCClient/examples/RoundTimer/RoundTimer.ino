/* Restart the timer when receive the MSG_ROUND_START.
 * Stop the timer when receive the MSG_ROUND_END.
 *
 * If the program reveived a 'e' from Serial,
 * send MSG_ROUND_COMPELETE to the BRC server.
 * If the program received a 'q' from Serial.
 * quit from the BRC server and go into a infinite loop.
 */

#include <BRCClient.h>

/* If you are using UNO, uncomment the next line. */
// #define UNO
/* If you are using MEGA and want to use HardwareSerial,
 * umcomment the next 2 lines. */
// #define USE_HARDWARE_SERIAL
// #define HW_SERIAL Serial3

#ifdef UNO
 #define UART_RX 3
 #define UART_TX 2
#else
 #define UART_RX 10
 #define UART_TX 2
#endif

#if !defined(UNO) && defined(USE_HARDWARE_SERIAL)
 BRCClient brcClient(&HW_SERIAL);
#else
 BRCClient brcClient(UART_RX, UART_TX);
#endif

// You have to modify the corresponding parameter
#define AP_SSID    "AP_SSID"
#define AP_PASSWD  "AP_PASSWD"
#define TCP_IP     "TCP_IP"
#define TCP_PORT   5000
#define MY_COMM_ID 0x20

void setup()
{
	Serial.begin(9600);
	while (!Serial)
		;

	brcClient.begin(9600);
	brcClient.beginBRCClient(AP_SSID, AP_PASSWD, TCP_IP, TCP_PORT);

	delay(2000);
	if (brcClient.registerID(MY_COMM_ID))
		Serial.println("ID register OK");
	else {
		Serial.println("ID register FAIL");
		brcClient.endBRCClient();

		while (1)
			;
	}

	// Wait for 5 seconds.
	delay(5000);
}

static long startFromMillis = 0L;
static long lastSecondMillis = 0L;
static bool timing = false;

void loop()
{
	CommMsg msg;

	if (timing && (millis() - lastSecondMillis) > 1000L) {
		lastSecondMillis = millis();
		Serial.print((lastSecondMillis - startFromMillis) / 1000L);
		Serial.println(" sec.");
	}

	if (brcClient.receiveMessage(&msg)) {
		switch (msg.type) {
			case MSG_ROUND_START:
				startFromMillis = millis();
				lastSecondMillis = startFromMillis;
				timing = true;
				Serial.println("0 sec.");
				break;

			case MSG_ROUND_END:
				timing = false;
				Serial.print((millis() - startFromMillis) / 1000L);
				Serial.print(".");
				Serial.print(((millis() - startFromMillis) % 1000L / 100L));
				Serial.println(" sec.");
				break;
		}
	}

	if (Serial.available()) {
		char ch = Serial.read();
		if (ch == 'e') {
			msg.type = MSG_ROUND_COMPELETE;
			brcClient.sendMessage(&msg);
		} else if (ch == 'q') {
			brcClient.endBRCClient();
			while (1)
				;
		}
	}
}
