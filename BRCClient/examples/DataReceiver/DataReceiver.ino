/* Just receive the message from others.
 * If it received the MSG_ROUND_END, quit the server.
 */
#include <BRCClient.h>

// #define UNO

#ifdef UNO
 #define UART_RX 3
 #define UART_TX 2
#else
 #define UART_RX 10
 #define UART_TX 2
#endif

// You have to modify the corresponding parameter
#define AP_SSID    "AP_SSID"
#define AP_PASSWD  "AP_PASSWD"
#define TCP_IP     "TCP_IP"
#define TCP_PORT   5000
#define MY_COMM_ID 0x20

BRCClient brcClient(UART_RX, UART_TX);

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
	else
		Serial.println("ID register FAIL");
}

void loop()
{
	CommMsg msg;
	char buf[40];

	if (brcClient.receiveMessage(&msg)) {
		sprintf(buf, "0x%02x, 0x%02x, %s", msg.type, msg.ID, msg.buffer);
		Serial.println(buf);

		if (msg.type == MSG_ROUND_END) {
			brcClient.endBRCClient();
			while (1)
				;
		}
	}

	delay(100);
}
