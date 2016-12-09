/* Register an ID representing itself on the BRC server.
 */

#include <BRCClient.h>

// #define UNO
// #define USE_HARDWARE_SERIAL

#ifdef UNO
 #ifdef USE_HARDWARE_SERIAL
  #error Hardware Serial can't use for UNO
 #endif

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
#define HW_SERIAL  Serial3

#ifdef USE_HARDWARE_SERIAL
 BRCClient brcClient(&HW_SERIAL);
#else
 BRCClient brcClient(UART_RX, UART_TX);
#endif


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
