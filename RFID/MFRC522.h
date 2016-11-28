/*
 * Modified from repo https://code.google.com/archive/p/rpi-rc522/
 * to make it able to be used on Arduino.
 *
 * Modified by LanKuDot <airlanser@gmail.com>.
 * 8th Nov., 2016
 */
/* The library for communicating the MF-RC522 module via SPI.
 */
#ifndef _MFRCC522_H_
#define _MFRCC522_H_

#include <stdint.h>

#define FIFOLEN 64	// 64 bytes
#define MAXRLEN 18

/* Command of MFRC522 */
#define PCD_IDLE       0x00
#define PCD_CALCCRC    0x03
#define PCD_TRANSCEIVE 0x0C
#define PCD_AUTHENT    0x0E
#define PCD_SOFTRESET  0x0F

/* Command of Mifare One */
#define PICC_REQIDL      0x26
#define PICC_HALT        0x50
#define PICC_CASCADE_Lv1 0x93
#define PICC_CASCADE_Lv2 0x95
#define PICC_CASCADE_Lv3 0x97

/**
 * @name RC522 register addresses
 */
/** @{ */
#define CommandReg     0x01
#define ComIEnReg      0x02
#define ComIrqReg      0x04
#define DivIrqReg      0x05
#define ErrorReg       0x06
#define Status2Reg     0x08
#define FIFODataReg    0x09
#define FIFOLevelReg   0x0A
#define ControlReg     0x0C
#define CollReg        0x0E
#define BitFramingReg  0x0D
#define ModeReg        0x11
#define TxControlReg   0x14
#define TxASKReg       0x15
#define CRCResultRegM  0x21	// MSB
#define CRCResultRegL  0x22	// LSB
#define TModeReg       0x2A
#define TPrescalerReg  0x2B
#define TReloadReg_Hi  0x2C
#define TReloadReg_Lo  0x2D
/** @} */

/* Status of the communication to PCD */
#define STATUS_OK              0x00
#define STATUS_TIMEOUT         0x01
#define STATUS_ERROR           0x02
#define STATUS_COLLISION       0x03
#define STATUS_PCD_NO_RESPONSE 0x04

class MFRC522
{
	public:
		/**
		 * @name Constructor
		 */
		/** @{ */
		/**
		 * @brief Constructor of class MFRC522
		 *
		 * The constructor stores the select pin and reset\_power\_down pin
		 * of the MF-RC522 for futher usage.<br />
		 *
		 * @param selectPin Specify the pin number of Arduino which is connected to SS pin of the MF-RC522 module.
		 * @param resetPowerDownPin Specify the pin number of Arduino which is connected to the reset pin of the MF-RC522 module.
		 */
		MFRC522(int selectPin, int resetPowerDownPin) :
			_selectPin(selectPin), _resetPowerDownPin(resetPowerDownPin) {}
		/** @} */

		/**
		 * @brief Make the MF-RC522 start operating.
		 *
		 * The function will generate a positive edge at <tt>resetPowerDownPin</tt>
		 * and delay 100 ms to wake up the MF-RC522 module.<br />
		 * Then call <tt>pcdReset()</tt> and <tt>pcdInit()</tt> to set up the module.
		 *
		 * @sa MFRC522::pcdReset(), MFRC522::pcdInit().
		 */
		void begin(void);

		/**
		 * @brief Reset the RC522 by command (soft reset).
		 */
		void pcdReset(void);

		/**
		 * @brief Initialize and config the RC522.
		 * Set the reading timeout to 25 ms, enable 100% ASK (Amplitude Shift Keying),
		 * and select 6363h as CRC preset value.<br />
		 * Then, call <tt>pcdAntennaOn()</tt> to active the antenna.
		 *
		 * @sa MFRC522::pcdAntennaOn()
		 */
		void pcdInit(void);

		/**
		 * @brief Turn on the antenna of RC522.
		 * You have to enable the antenna to find and read a tag.
		 */
		void pcdAntennaOn(void);
		/**
		 * @brief Turn off the antenna of RC522.
		 */
		void pcdAntennaOff(void);

		/**
		 * @brief Send a request command from PCD to the PICC, and receive the 2-byte ATQA from the PICC.
		 * The function will call <tt>commWithPICC()</tt> to send the data.<br />
		 *
		 * The <tt>req_cmd</tt> could be PICC_REQIDL,
		 * which RC522 will get response from cards that are not in halt state, or,
		 * could be PICC_REQALL, which cards in halt state would also response to the command.
		 *
		 * @param req_cmd The request command to the PICC. Could be PICC_REQIDL or PICC_REQALL.
		 * @param ATQA [out] 2-byte Answer To Requset_A from the PICC
		 * @return The return value of <tt>commWithPICC()</tt>
		 *
		 * @sa MFRC522::commWithPICC()
		 */
		uint8_t piccRequest(uint8_t req_cmd, uint8_t *ATQA);

		/**
		 * @brief Get a byte of CLn (cascade level n) UID from one PICC.
		 *
		 * Note that this function should be called before <tt>piccSelect()</tt>.<br />
		 * The function will call <tt>commWithPICC()</tt> to send the data.
		 *
		 * @param cascadeLv The cascade level
		 * @param sn [out] The CLn read from PICC
		 * @return The return value of <tt>commWithPICC()</tt>
		 *
		 * @sa MFRC522::commWithPICC(), MFRC522::piccSelect()
		 */
		uint8_t piccAnticoll(uint8_t cascadeLv, uint8_t *sn);

		/**
		 * @brief Send the CLn read from a PICC, only matched PICC would response.
		 *
		 * The function will call <tt>commWithPICC()</tt> to send the data.
		 *
		 * @param cascadeLv The cascade level
		 * @param sn The CLn read from PICC by function <tt>piccAnticoll()</tt>
		 * @return The return value of <tt>commWithPICC()</tt>
		 *
		 * @sa MFRC522::commWithPICC(), MFRC522::piccAnticoll()
		 */
		uint8_t piccSelect(uint8_t cascadeLv, uint8_t *sn);

		/**
		 * @brief Make a PICC card (the last one responsed to the PCD) go into the halt state.
		 *
		 * If the command was successful, <tt>commWithPICC()</tt> would return STATUS_TIMEOUT.
		 *
		 * @return The return value of <tt>commWithPICC()</tt>
		 * @retval STATUS_TIMEOUT Command successfully sent.
		 */
		uint8_t piccHalt(void);

	private:
		/**
		 * @brief Communication with the PICC.
		 *
		 * @param cmd The command to be executed. Could be PCD_AUTHENT or PCD_TRANSCEIVE
		 * @param inBuf The pointer to the input buffer
		 * @param inBytes The size of the input buffer in bytes
		 * @param outBuf The pointer to the output buffer
		 * @param outBits The size of the vaild data in the output buffer in bits
		 *
		 * @return The status of the communication.
		 * @retval STATUS_OK No problem.
		 * @retval STATUS_TIMEOUT Timout in receiveing data from RC522, for example, no tag attached.
		 * @retval STATUS_ERROR Error on receiving data from PICC.
		 * @retval STATUS_COLLISION Collision detected.
		 * @retval STATUS_PCD_NO_RESPONSE Something went wrong on RC522.
		 */
		uint8_t commWithPICC(uint8_t cmd, uint8_t *inBuf, uint8_t inBytes, uint8_t *outBuf, uint8_t *outBits);

		/**
		 * @brief Ask PCD to calculate the CRC code.
		 *
		 * The PCD will calculate the CRC code from the <tt>inBuf</tt>, and
		 * the 2-byte result will be stored to <tt>outBuf</tt>.
		 *
		 * @param inBuf The pointer to a buffer storing the data to be calculated.
		 * @param inBytes The data length of <tt>inBuf</tt> in bytes.
		 * @param outBuf [out] The pointer to a buffer to store 2-byte CRC result.
		 */
		void calculateCRC(uint8_t *inBuf, uint8_t inBytes, uint8_t *outBuf);

		/**
		 * @name Register operations
		 * The operations of registers of RC522.
		 */
		/** @{ */
		/**
		 * @brief Set the specified bits of a register of RC522.
		 * @param regAddr Specify the address of a register.
		 * @param mask    Specify which bits will be set.
		 */
		void pcdSetBitMask (uint8_t regAddr, uint8_t mask);
		/**
		 * @brief Clear the specifed bits of a register of RC522.
		 * @param regAddr Specify the address of a register.
		 * @param mask    Specify which bits will be cleared.
		 */
		void pcdClearBitMask(uint8_t regAddr, uint8_t mask);

		/**
		 * @brief Read a register value of RC522.
		 * @param regAddr Specify the address of a register.
		 * @return The value in the specified register.
		 */
		uint8_t pcdReadReg(uint8_t regAddr);
		/**
		 * @brief Write a value to a register of RC522.
		 * @param regAddr Specify the address of a register.
		 * @param value   Specify the value to be written to the register.
		 */
		void pcdWriteReg(uint8_t regAddr, uint8_t value);
		/** @} */

		/**
		 * @brief The pin number which is connected to the SS pin of MF-RC522 module.
		 */
		int _selectPin;

		/**
		 * @brief The pin number which is connected to the reset pin of MF-RC522 module.
		 */
		int _resetPowerDownPin;
};

#endif // _MFRCC522_H_
