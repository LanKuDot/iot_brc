/* Change the default baudrate of the module.
 * For the first time changing to the different baudrate
 * may get "Failed" because of garbled characters.
 * Change the "origBaudrate" to the same as "newBaudrate"
 * and then run again. You would receive "OK" message.
 */
#include <KSM111_ESP8266.h>

#define UNO

#ifdef UNO
 #define UART_RX 3
 #define UART_TX 2
#else
 #define UART_RX 10
 #define UART_TX 2
#endif

#define origBaudrate 115200
#define newBaudrate 9600

KSM111_ESP8266 wifi(UART_RX, UART_TX);

void setup()
{
	Serial.begin(9600);
	while (!Serial)
		;

	wifi.begin(origBaudrate);
	Serial.print("Set baudrate to ");
	Serial.println(newBaudrate);
	if (wifi.setBaudrate(newBaudrate)) {
		Serial.println("Done!");
	} else
		Serial.println("Failed!");
}

void loop()
{
}
