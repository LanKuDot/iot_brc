#include <SPI.h>
#include <RFID.h>

// #define UNO

/* The pin mapping of SPI */
#ifdef UNO
 #define SPI_MOSI 11
 #define SPI_MISO 12
 #define SPI_SCLK 13
#else
 #define SPI_MOSI 51
 #define SPI_MISO 50
 #define SPI_SCLK 52
#endif

// SPI_SS pin can be chosen by yourself
// becasue we use SPI in master mode.
#define SPI_SS   10
#define MFRC522_RSTPD 9

RFID rfid(SPI_SS, MFRC522_RSTPD);

void setup()
{
	SPI.begin();
	SPI.beginTransaction(SPISettings(10000000L, MSBFIRST, SPI_MODE3));
	rfid.begin();

	Serial.begin(9600);
	while (!Serial)
		;
}

static uint8_t status;
static uint16_t card_type;
static uint8_t sn[MAXRLEN], snBytes;

void loop()
{
	delay(200);
	if ((status = rfid.findTag(&card_type)) == STATUS_OK &&
	    card_type == 1024) {
		Serial.print("Tag SN: ");
		if ((status = rfid.readTagSN(sn, &snBytes)) == STATUS_OK) {
			for (int i = 0; i < snBytes; ++i)
				Serial.print(sn[i], HEX);
			Serial.println();
			rfid.piccHalt();
		}
	} else
		Serial.println("No tag.");
}
