/**
 * Modified from repo https://code.google.com/archive/p/rpi-rc522/
 * to make it able to be used on Arduino.
 *
 * Modified by LanKuDot <airlanser@gmail.com>.
 * 8th Nov., 2016
 */
#include <string.h>
#include "RFID.h"

uint8_t RFID::findTag(uint16_t *card_type)
{
	uint8_t status;
	if ((status = MFRC522::piccRequest(PICC_REQIDL, _buff)) == STATUS_OK) {
		*card_type = (uint16_t)(_buff[0] << 8 | _buff[1]);
	}
	return status;
}

uint8_t RFID::readTagSN(uint8_t *sn, uint8_t *snBytes)
{
	if (MFRC522::piccAnticoll(PICC_CASCADE_Lv1, _buff) != STATUS_OK) return STATUS_ERROR;
	if (MFRC522::piccSelect(PICC_CASCADE_Lv1, _buff) != STATUS_OK) return STATUS_ERROR;
	if (_buff[0] == 0x88) {
		memcpy(sn, &_buff[1], 3);
		if (MFRC522::piccAnticoll(PICC_CASCADE_Lv2, _buff) != STATUS_OK) return STATUS_ERROR;
		if (MFRC522::piccSelect(PICC_CASCADE_Lv2, _buff) != STATUS_OK) return STATUS_ERROR;
		if (_buff[0] = 0x88) {
			memcpy(sn + 3, &_buff[1], 3);
			if (MFRC522::piccAnticoll(PICC_CASCADE_Lv3, _buff) != STATUS_OK) return STATUS_ERROR;
			if (MFRC522::piccSelect(PICC_CASCADE_Lv3, _buff) != STATUS_OK) return STATUS_ERROR;
			memcpy(sn + 6, _buff, 3);
			*snBytes = 10;
		} else {
			memcpy(sn + 3, _buff, 4);
			*snBytes = 7;
		}
	} else {
		memcpy(sn, _buff, 4);
		*snBytes = 4;
	}

	return STATUS_OK;
}
