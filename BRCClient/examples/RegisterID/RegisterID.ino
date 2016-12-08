/* Register an ID representing itself on the BRC server.
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

// if you're using software serial, uncomment this line.
// BRCClient brcClient(UART_RX, UART_TX);

// if you're using hardware serial, uncomment this line.
// BRCClient brcClient(&Serial3); //use "TX3" and "RX3" on arduino mega

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

	// Leave the BRC server after 5 seconds.
	delay(5000);
	brcClient.endBRCClient();
}

void loop()
{
}
