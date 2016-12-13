/*
 * Modified from repo https://code.google.com/archive/p/rpi-rc522/
 * to make it able to be used on Arduino.
 *
 * Modified by LanKuDot <airlanser@gmail.com>.
 * 8th Nov., 2016
 */
/**
 * @file RFID/RFID.h
 * @brief The header file of class RFID
 */
#ifndef _RFID_H_
#define _RFID_H_

#include "MFRC522.h"

/**
 * @class RFID RFID/RFID.h <RFID.h>
 * @brief The class for accessing RFID tag through MF-RC522.
 */
class RFID : public MFRC522
{
	public:
		/**
		 * @name Constructor
		 */
		/** @{ */
		/**
		 * @brief Call the contrsutor of the base class MFRC522
		 * @sa MFRC522::MFRC522()
		 */
		RFID(int selectPin, int resetPowerDownPin) :
			MFRC522(selectPin, resetPowerDownPin) {}
		/** @} */

		/**
		 * @brief Check if there is any tag existing.
		 *
		 * Here are the bits in card_type:
		 * [15:12] Reserved
		 * [11: 8] Properietary coding
		 * [ 7: 6] UID size bit frame
		 * [ 5]    Reserved
		 * [ 4: 0] Bit frame anticollision
		 *
		 * @param card_type [out] The reponse of the PICC
		 * @return The status of the finding a tag.
		 * @retval STATUS_OK        A Tag found
		 * @retval STATUS_TIMEOUT   No tag there
		 * @retval STATUS_ERROR     Error on reading data from PICC
		 * @retval STATUS_COLLISION More than one tag found
		 * @retval STATUS_PCD_NO_RESPONSE Internal error on the PCD
		 */
		uint8_t findTag(uint16_t *card_type);

		/**
		 * @brief Read the serial number of the tag
		 * @param sn [out] The buffer for storing the serial number. At least 4 bytes.
		 * @param snBytes [out] The vaild bytes in the <tt>sn</tt>.
		 * @return STATUS_OK, if successfully read the serial number.
		 */
		uint8_t readTagSN(uint8_t *sn, uint8_t *snBytes);

	private:
		uint8_t _buff[MAXRLEN];
};

#endif // _RFID_H_
