#include <WIFI.h>

#define UART_RX 10
#define UART_TX 2

#define origBaudrate 115200
#define newBaudrate 9600

WIFI wifi(UART_RX, UART_TX);

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
