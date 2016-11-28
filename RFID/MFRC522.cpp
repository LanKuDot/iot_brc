/*
 * Modified from repo https://code.google.com/archive/p/rpi-rc522/
 * to make it able to be used on Arduino.
 *
 * Modified by LanKuDot <airlanser@gmail.com>.
 * 8th Nov., 2016
 */
#include <Arduino.h>
#include <SPI.h>

#include "MFRC522.h"

uint8_t MFRC522::pcdReadReg(uint8_t regAddr)
{
	unsigned char buff[2];
	buff[0] = ((regAddr << 1) & 0x7E) | 0x80;	// Set bit 7 for reading
	buff[1] = 0x00;

	digitalWrite(_selectPin, LOW);
	SPI.transfer(buff, 2);
	digitalWrite(_selectPin, HIGH);

	return (uint8_t)buff[1];
}

void MFRC522::pcdWriteReg(uint8_t regAddr, uint8_t value)
{
	unsigned char buff[2];
	buff[0] = ((regAddr << 1) & 0x7E);	// Clear bit 7 for write
	buff[1] = (unsigned char)value;

	digitalWrite(_selectPin, LOW);
	SPI.transfer(buff, 2);
	digitalWrite(_selectPin, HIGH);
}

void MFRC522::pcdSetBitMask(uint8_t regAddr, uint8_t mask)
{
	uint8_t tmp = pcdReadReg(regAddr);
	pcdWriteReg(regAddr, tmp | mask);
}

void MFRC522::pcdClearBitMask(uint8_t regAddr, uint8_t mask)
{
	uint8_t tmp = pcdReadReg(regAddr);
	pcdWriteReg(regAddr, tmp & ~mask);
}

/* Soft reset on the MFRC522 module.
 */
void MFRC522::pcdReset(void)
{
	pcdWriteReg(CommandReg, PCD_SOFTRESET);
}

void MFRC522::pcdInit(void)
{
	// Time out setting: 25 ms
	pcdWriteReg(TModeReg, 0x80);
	pcdWriteReg(TPrescalerReg, 0xA5);	// Prescaler: 165 (0x0A5)
	pcdWriteReg(TReloadReg_Hi, 0x03);
	pcdWriteReg(TReloadReg_Lo, 0xFF);	// Reload val: 1023 (0x3FF)
	pcdWriteReg(TxASKReg, 0x40);	// 100% ASK (Amplitude Shift Keying)
	pcdWriteReg(ModeReg, 0x3D);	// 6363h for CRC

	pcdAntennaOn();
}

void MFRC522::begin(void)
{
	// Pin initialization
	pinMode(_selectPin, OUTPUT);
	pinMode(_resetPowerDownPin, OUTPUT);

	// Reset
	digitalWrite(_resetPowerDownPin, HIGH); // Generate positive edge
	delay(100);	// Wait for waking up RC522
	pcdReset();
	pcdInit();
}

uint8_t MFRC522::commWithPICC(uint8_t cmd, uint8_t *inBuf, uint8_t inBytes, uint8_t *outBuf, uint8_t *outBits)
{
	uint8_t waitFor, status;

	switch(cmd) {
	case PCD_AUTHENT:
		waitFor = 0x10;
		break;
	// Command: Transmit data from FIFO buffer to antenna and
	// automatically activates the receiver after transmission.
	case PCD_TRANSCEIVE:
		waitFor = 0x30;
		break;
	}

	pcdWriteReg(CommandReg, PCD_IDLE);	// Stop the active commands
	pcdClearBitMask(ComIrqReg, 0x80);	// Clear all irq bits
	pcdSetBitMask(FIFOLevelReg, 0x80);	// Flush the FIFO buffer
	// Write data to FIFO buffer
	for (int in = 0; in < inBytes; ++in)
		pcdWriteReg(FIFODataReg, inBuf[in]);
	pcdWriteReg(CommandReg, cmd);	// Execute the command
	if (cmd == PCD_TRANSCEIVE)
		// Start the transmission of data
		pcdSetBitMask(BitFramingReg, 0x80);

	// Wait for the interrupt for 30 ms.
	uint8_t i = 150, irq;
	do {
		// Check the irq every 200us.
		delayMicroseconds(200);
		irq = pcdReadReg(ComIrqReg);
		--i;
	} while ((i != 0) && (!(irq & 0x01)) && (!(irq & waitFor)));

	// Stop the transmission of data
	pcdClearBitMask(BitFramingReg, 0x80);

	if ((i != 0) && (!(irq & 0x01))) {
		uint8_t err = pcdReadReg(ErrorReg);
		if (!(err & 0x11)) {
			status = STATUS_OK;
			if (cmd == PCD_TRANSCEIVE) {
				uint8_t fifoBytes, lastBits;
				fifoBytes = pcdReadReg(FIFOLevelReg);
				lastBits = pcdReadReg(ControlReg) & 0x07;	// Get the number of vaild bits in the last received byte.
				// Calculate the num of the vaild bits.
				if (lastBits)
					*outBits = (fifoBytes - 1) * 8 + lastBits;
				else	// If lastBits is 0, the whole byte is vaild.
					*outBits = fifoBytes * 8;

				if (fifoBytes == 0)      fifoBytes = 1;
				if (fifoBytes > MAXRLEN) fifoBytes = MAXRLEN;

				// Read data from FIFO buffer
				for (int out = 0; out < fifoBytes; ++out)
					outBuf[out] = pcdReadReg(FIFODataReg);
			}
		} else
			status = STATUS_ERROR;

		if (err & 0x08)
			status = STATUS_COLLISION;
	} else {
		if (i == 0) status = STATUS_PCD_NO_RESPONSE;
		else        status = STATUS_TIMEOUT;
	}

	return status;
}

uint8_t MFRC522::piccRequest(uint8_t req_cmd, uint8_t *ATQA)
{
	uint8_t status, receiveBits, buff[MAXRLEN];

	// PICC request command has 7 bits in a short frame.
	pcdWriteReg(BitFramingReg, 0x07);
	buff[0] = req_cmd;

	status = commWithPICC(PCD_TRANSCEIVE, buff, 1, buff, &receiveBits);
	if ((status == STATUS_OK) && (receiveBits == 16)) {
		ATQA[0] = buff[0];
		ATQA[1] = buff[1];
	}

	return status;
}

uint8_t MFRC522::piccAnticoll(uint8_t cascadeLv, uint8_t *sn)
{
	uint8_t status, buff[MAXRLEN], collbits = 0, i = 0, receiveBits, sn_BCC = 0;
	uint8_t loop = 32;	// The maximum number of loops is 32.

	pcdWriteReg(BitFramingReg, 0x00);	// The whole bits in the last byte are vaild.
	do {
		buff[0] = cascadeLv;	// SEL with anticollision type and cascade level
		buff[1] = 0x20 + collbits;	// NVB (0x20)
		status = commWithPICC(PCD_TRANSCEIVE, buff, 2+i, buff, &receiveBits);
		// Collision occured
		if (status == STATUS_COLLISION) {
			collbits = pcdReadReg(CollReg) & 0x1F;	// Get the bit pos of first detected collision
			if (collbits == 0) collbits = 32;
			i = (collbits - 1) / 8 + 1;
			buff[i-1] |= (1 << ((collbits - 1) % 8));
			buff[5] = buff[3];
			buff[4] = buff[2];
			buff[3] = buff[1];
			buff[2] = buff[0];
			pcdWriteReg(BitFramingReg, (collbits % 8));
		}
	} while (((--loop) > 0) && (status == STATUS_COLLISION));

	if (status == STATUS_OK) {
		for (int out = 0; out < 4; ++out) {
			sn[out] = buff[out];
			sn_BCC ^= buff[out];
		}
		if (sn_BCC != buff[4])
			status = STATUS_ERROR;
	}

	return status;
}

uint8_t MFRC522::piccSelect(uint8_t cascadeLv, uint8_t *sn)
{
	uint8_t status, receivedBits, buf[MAXRLEN];

	// Generate bytes for select a PICC
	buf[0] = cascadeLv;	// SEL with anticollision type and cascade level
	buf[1] = 0x70;	// NVB is 0x70
	// buf[2..5] All bits of UID CLn
	buf[6] = 0;	// BCC (UIF CLn check byte)
	for (int i = 0; i < 4; ++i) {
		buf[i+2] = sn[i];
		buf[6]  ^= sn[i];
	}
	calculateCRC(buf, 7, &buf[7]);	// Attach CRC_A at byte 7 and 8
	pcdClearBitMask(Status2Reg, 0x80); // Disable encrypted communication

	status = commWithPICC(PCD_TRANSCEIVE, buf, 9, buf, &receivedBits);

	// PICC responses with 1-byte SAK and 2-byte CRC_A
	if ((status == STATUS_OK) && (receivedBits == 24))
		status = STATUS_OK;
	else
		status = STATUS_ERROR;

	return status;
}

uint8_t MFRC522::piccHalt()
{
	uint8_t buff[2], outBits;

	buff[0] = PICC_HALT;
	buff[1] = 0x00;
	return commWithPICC(PCD_TRANSCEIVE, buff, 2, buff, &outBits);
}

void MFRC522::calculateCRC(uint8_t *inBuf, uint8_t inBytes, uint8_t *CRCBuf)
{
	uint8_t irq, i;
	pcdClearBitMask(DivIrqReg, 0x04);	// Clear CRCIRq bit
	pcdWriteReg(CommandReg, PCD_IDLE);	// Stop all active command
	pcdSetBitMask(FIFOLevelReg, 0x80);	// Flush FIFO buffer
	for (i = 0; i < inBytes; ++i)
		pcdWriteReg(FIFODataReg, inBuf[i]);
	pcdWriteReg(CommandReg, PCD_CALCCRC);	// Calcuate CRC
	// Wait for PCD
	i = 0xFF;
	do {
		irq = pcdReadReg(DivIrqReg);
		--i;
	} while ((i != 0) && !(irq & 0x04));
	// Get the result of CRC
	CRCBuf[0] = pcdReadReg(CRCResultRegL);
	CRCBuf[1] = pcdReadReg(CRCResultRegM);
}

/* Switch on the antenna on the MFRC522 module.
 * By setting Tx1RFEn and Tx2RFEn in TxControlReg.(8.6.3)
 */
void MFRC522::pcdAntennaOn()
{
	uint8_t tmp = pcdReadReg(TxControlReg);
	if (!(tmp & 0x03))
		pcdSetBitMask(TxControlReg, 0x03);
}

/* Switch off the antenna off the MFRC522 module
 * By clearing Tx1RFEn and Tx2RFEn in TxControlReg.(8.6.3)
 */
void MFRC522::pcdAntennaOff()
{
	pcdClearBitMask(TxControlReg, 0x03);
}
