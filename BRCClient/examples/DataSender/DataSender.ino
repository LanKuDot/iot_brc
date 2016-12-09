/* Periodically send or broadcast the custom message
 * until the user stops it.
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
#define MY_COMM_ID 0x24

#define PARTNER_COMM_ID 0x20

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

void loop()
{
	sendToClient(PARTNER_COMM_ID, "Hello");
	delay(2000);
	broadcast("World");
	delay(2000);
	// Enter 'q' from serial monitor to stop the application.
	if (Serial.available()) {
		char ch = Serial.read();
		if (ch == 'q') {
			brcClient.endBRCClient();
			while (1)
				;
		}
	}
}

// Note that the length of message can't be more than
// COMM_MSG_BUF_LEN - 1, which reserving 1 byte for null-
// character.
bool sendToClient(uint8_t ID, char *message)
{
	CommMsg msg = {
		.type = MSG_CUSTOM,
		.ID = PARTNER_COMM_ID
	};
	strncpy(msg.buffer, message, COMM_MSG_BUF_LEN);
	brcClient.sendMessage(&msg);

	// Delay for a while to receive the response
	delay(1);
	if (!brcClient.receiveMessage(&msg))
		return false;

	if (msg.ID == MY_COMM_ID &&
		strcmp(msg.buffer, "OK") == 0)
		return true;
	else
		return false;
}

bool broadcast(char *message)
{
	CommMsg msg = {
		.type = MSG_CUSTOM_BROADCAST
	};
	strncpy(msg.buffer, message, COMM_MSG_BUF_LEN);
	brcClient.sendMessage(&msg);

	// Delay for a while to receive the response
	delay(1);
	if (!brcClient.receiveMessage(&msg))
		return false;

	if (msg.ID == MY_COMM_ID &&
		strcmp(msg.buffer, "OK") == 0)
		return true;
	else
		return false;
}
