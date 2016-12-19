/* Request the map data of the RFID read from BRC server.
 * Input 'q' to quit the server.
 */
#include <BRCClient.h>
#include <SPI.h>
#include <RFID.h>

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
#define MY_COMM_ID (char)0x20

// RFID setting
#define SPI_SS 10
#define MFRC522_RSTPD 9

RFID rfid(SPI_SS, MFRC522_RSTPD);

void setup()
{
	// Initialize the SPI and RFID
	SPI.begin();
	SPI.beginTransaction(SPISettings(10000000L, MSBFIRST, SPI_MODE3));
	rfid.begin();

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

// The length of serial number of the tag we use here is 4 bytes.
static uint8_t tagSN[4];

void loop()
{
	CommMsg msg;
	char buf[40];

	// If it read a serial number, reqeust the map data from server.
	if (readTagSN()) {
		brcClient.requestMapData(tagSN, "R");
	}

	if (brcClient.receiveMessage(&msg)) {
		sprintf(buf, "0x%02x, 0x%02x, %s", msg.type, msg.ID, msg.buffer);
		Serial.println(buf);

		if (msg.type == MSG_REQUEST_RFID) {
			// Use this function to convert the raw data to the map data.
			MapMsg map = rawDataToMapMsg(msg.buffer);

			// Display the converted data.
			sprintf(buf, "MAP: %02X%02X%02X%02X, (%02d, %02d), 0x%02X, %s",
					map.sn[0], map.sn[1], map.sn[2], map.sn[3],
					map.x, map.y, map.type, map.customTag);
			Serial.println(buf);
		}
	}

	// Input 'q' to quit the server.
	if (Serial.available() && Serial.read() == 'q') {
		brcClient.endBRCClient();
		while (1)
			;
	}

	delay(200);
}

/**
 * @brief Read the serial number of the RFID tag.
 *
 * This function will save the 4-byte serial number to the global variable _tagSN_.
 * Therefore, you can directly call the function _BRCClient::requestMapData()_ without
 * extracting first 4 bytes from _sn_.
 * Futhermore, you can check if the serial number read has been already known in
 * the function. For example, return false if the serial number has been already
 * known to avoid the program reqeusting the same data from the server.
 */
bool readTagSN()
{
	uint8_t status, snBytes, sn[MAXRLEN];
	uint16_t card_type;

	if ((status = rfid.findTag(&card_type)) == STATUS_OK &&
	    card_type == 1024) {
		if ((status = rfid.readTagSN(sn, &snBytes)) == STATUS_OK) {
			// Loop unrolling
			// The length of serial number of the tag we use here is 4 bytes.
			tagSN[0] = sn[0];
			tagSN[1] = sn[1];
			tagSN[2] = sn[2];
			tagSN[3] = sn[3];

			rfid.piccHalt();

			return true;
		}
	}

	return false;
}
