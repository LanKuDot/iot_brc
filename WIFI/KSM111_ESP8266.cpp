#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

#include "KSM111_ESP8266.h"

#define DEBUG
#ifdef DEBUG
 #define DEBUG_STR(x) Serial.print("# "); Serial.println(x)
#else
 #define DEBUG_STR(X)
#endif

bool KSM111_ESP8266::begin(long baudrate)
{
	char *ch = _buff;

	// Set _resetPin to HIGH to avoid resetting the module.
	if (_resetPin > 0) {
		pinMode(_resetPin, OUTPUT);
		digitalWrite(_resetPin, HIGH);
	}

	// Initialize the SoftwareSerial
	_serial.begin(baudrate);
	while (!_serial)
		;

	DEBUG_STR("AT");
	// Wake up the wifi module.
	_serial.println("AT");
	delay(5000);	// It takes some time to get out of bed
	while(_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);

	/* Response: "OK"
	 */
	if (strstr(_buff, "OK"))
		return true;

	return false;
}

void KSM111_ESP8266::end()
{
	_serial.end();
}

bool KSM111_ESP8266::softReset()
{
	char *ch = _buff;

	DEBUG_STR("AT+RST");
	_serial.println("AT+RST");
	delay(5000);
	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);

	/* Response: "AT+RST
	 *          \nOK
	 *            <Tons of message>
	 *            ready"
	 */
	if (strstr(_buff, "OK"))
		return true;

	return false;
}

bool KSM111_ESP8266::setMode(uint8_t mode)
{
	char *ch = _buff;

	sprintf(ch, "AT+CWMODE=%d", mode);
	DEBUG_STR(_buff);
	_serial.println(ch);
	delay(500);
	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);

	/* Response "AT+CWMODE=<mode>
	 *         \nOK" */
	if (strstr(_buff, "OK"))
		return true;

	return false;
}

uint8_t KSM111_ESP8266::getMode()
{
	char *ch = _buff;

	DEBUG_STR("AT+CWMODE?");
	_serial.println("AT+CWMODE?");
	delay(100);
	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);

	/* Response: "AT+CWMODE?
	 *            +CWMODE:<mode>
	 *          \nOK"
	 */
	if ((ch = strstr(_buff, ":")) != NULL) {
		switch (*++ch) {
			case '1':
				return STATION;
			case '2':
				return AP;
			case '3':
				return BOTH;
		}
	}

	return -1;
}

bool KSM111_ESP8266::setBaudrate(long baudrate)
{
	char *ch = _buff;

	sprintf(ch, "AT+CIOBAUD=%ld", baudrate);
	DEBUG_STR(ch);
	_serial.println(ch);
	delay(100);
	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);

	if (strstr(_buff, "OK")) {
		_serial.begin(baudrate);
		return true;
	}

	return false;
}

bool KSM111_ESP8266::listAP(APInfo *apList, int count, int *vaildCount)
{
	char *ch = _buff;
	uint8_t status;
	int i = 0;
	APInfo apInfo;

	if (apList == NULL || count < 1)
		return false;

	DEBUG_STR("AT+CWLAP");
	_serial.println("AT+CWLAP");
	delay(5000);	// Wait for searching

	// Wait until responsing with OK or ERROR
	while (1) {
		// Read the response line by line
		while (_serial.available()) {
			*ch++ = _serial.read();
			if (*(ch-1) == '\n') {
				*(ch-1) = '\0';
				DEBUG_STR(_buff);
				break;
			}
		}

		if (strstr(_buff, "OK")) {
			status = true;
			break;
		} else if (strstr(_buff, "ERROR")) {
			status = false;
			break;
		} else if (strstr(_buff, "+CWLAP:(")) {
			strtok(_buff, "(),\"");	// Ignore +CWLAP:
			apInfo.encrypt = atoi(strtok(NULL, "(),\""));
			strcpy(apInfo.ssid, strtok(NULL, "(),\""));
			apInfo.rssi = atoi(strtok(NULL, "(),\""));
			strcpy(apInfo.mac, strtok(NULL, "(),\""));
			apInfo.ch = atoi(strtok(NULL, "(),\""));

			if (i < count)
				apList[i++] = apInfo;
		}

		delay(5);	// Delay for a moment
		ch = _buff;	// Reset buffer
	}

	// Flush the buffer
	while (_serial.available())
		;

	if (vaildCount != NULL)
		*vaildCount = i;
	return status;
}

bool KSM111_ESP8266::joinAP(const char *ssid, const char *passwd)
{
	char *ch = _buff;

	sprintf(ch, "AT+CWJAP=\"%s\",\"%s\"", ssid, passwd);
	DEBUG_STR(ch);
	_serial.println(ch);
	delay(8000);

	// Wait until responsing with OK or FAIL
	while (1) {
		while (_serial.available()) {
			*ch++ = _serial.read();
		}
		*ch = '\0';
		DEBUG_STR(_buff);

		if (strstr(_buff, "OK"))
			return true;
		else if (strstr(_buff, "FAIL"))
			return false;

		ch = _buff;
	}
}

void KSM111_ESP8266::quitAP()
{
	char *ch = _buff;

	_serial.println("AT+CWQAP");
	DEBUG_STR("AT+CWQAP");
	delay(100);

	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);
}

bool KSM111_ESP8266::multiConnect(bool mode)
{
	char *ch = _buff;

	sprintf(ch, "AT+CIPMUX=%d", mode ? 1 : 0 );
	_serial.println(ch);
	DEBUG_STR(ch);
	delay(100);

	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);

	if (strstr(_buff, "OK"))
		return true;
	else
		return false;
}

uint8_t KSM111_ESP8266::beginClient(const char *type, const char *ip, const int port)
{
	char *ch = _buff;

	sprintf(ch, "AT+CIPSTART=\"%s\",\"%s\",%d", type, ip, port);
	_serial.println(ch);
	DEBUG_STR(ch);
	delay(100);

	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);

	if (strstr(_buff, "OK"))
		return CONNECT_OK;
	else if (strstr(_buff, "ALREADY CONNECT"))
		return ALREADY_CONNECT;
	else
		return CONNECT_ERROR;
}

bool KSM111_ESP8266::endClient()
{
	char *ch = _buff;

	_serial.println("AT+CIPCLOSE");
	DEBUG_STR("AT+CIPCLOSE");
	delay(250);

	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);

	if (strstr(_buff, "CLOSED"))
		return true;
	else
		return false;
}

void KSM111_ESP8266::getIP(uint8_t mode, char *ip)
{
	char *ch = _buff;

	switch (mode) {
		case STATION:
			_serial.println("AT+CIPSTA?");
			break;
		case AP:
			_serial.println("AT+CIPAP?");
			break;
		default:
			return;
	}
	DEBUG_STR("GET IP");
	delay(500);

	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);

	// Parse IP
	if (strstr(_buff, "OK")) {
		char *secondQoute;
		ch = strchr(_buff, '\"');
		secondQoute = strchr(ch+1, '\"');
		*secondQoute = '\0';
		strcpy(ip, ch+1);
	}
}

bool KSM111_ESP8266::puts(const char *msg)
{
	char *ch = _buff;
	int msgLen = strlen(msg);

	sprintf(ch, "AT+CIPSEND=%d", msgLen);
	_serial.println(ch);
	DEBUG_STR(ch);
	delay(50);

	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	if (!strstr(_buff, ">")) {
		return false;
	}

	_serial.println(msg);
	while (1) {
		ch = _buff;

		while (_serial.available()) {
			*ch++ = _serial.read();
			if (*(ch-1) == '\n') {
				*ch = '\0';
				DEBUG_STR(_buff);

				if (strstr(_buff, "SEND OK"))
					return true;
				else if (strstr(_buff, "ERROR"))
					return false;

				break;
			}
		}
	}
}

int8_t KSM111_ESP8266::gets(char * const msg, unsigned int buffLen)
{
	char *ch = _buff, *end;
	int sendID = 0, msgLen = 0;

	if (!_serial.available())
		return -1;

	// +IPD,<msgLen>:<data> in SINGLE mode
	// +IPD,<id>,<msgLen>:<data> in MULTIPLE mode
	while (_serial.available()) {
		*ch++ = _serial.read();
	}
	*ch = '\0';
	DEBUG_STR(_buff);

	// Not the vaild received message
	if (!strstr(_buff, "+IPD"))
		return -1;

	ch = strchr(_buff, ','); ++ch;	// Ignore +IPD
	if ((end = strchr(ch, ',')) != NULL) {	// Get sendID
		*end = '\0';
		sendID = atoi(ch);
		ch = end + 1;
	}
	end = strchr(ch, ':');	// Get message length
	*end = '\0';
	msgLen = atoi(ch);
	ch = end + 1;

	memset(msg, 0, buffLen);
	--buffLen;	// 1 for null character
	end = msg;
	while (buffLen != 0 && msgLen != 0) {
		*end++ = *ch++;
		--buffLen; --msgLen;
	}

	return sendID;
}
