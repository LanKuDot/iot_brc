#include <Arduino.h>
#include <string.h>

#include "BRCClient.h"

bool BRCClient::beginBRCClient(const char *ssid, const char *passwd, const char *serverIP, const int port)
{
	setMode(STATION);
	multiConnect(false);
	quitAP();
	if (joinAP(ssid, passwd) && beginClient("TCP", serverIP, port))
		return true;

	return false;
}

bool BRCClient::registerID(const uint8_t ID)
{
	// Invaild register ID
	if (ID == 0xFF ||
	    (ID >= 0x00 && ID < 0x10)) {
		return false;
	}

	CommMsg requestMsg = {
		.type = MSG_REGISTER,
		.ID = ID
	};

	// Send the request
	if (!sendMessage(&requestMsg))
		return false;

	// Wait for a moment and receive the reply from server
	delay(10);
	if (receiveMessage(&requestMsg) &&
	    strcmp(requestMsg.buffer, "OK") == 0) {
		return true;
	} else
		return false;
}

bool BRCClient::endBRCClient()
{
	if (!endClient())
		return false;
	
	quitAP();
	return true;
}

bool BRCClient::sendMessage(CommMsg *msg)
{
	char buffer[COMM_MSG_BUF_LEN + 2];
	char *ch = buffer;

	// Add the type first.
	*ch = msg->type;

	switch (*ch++) {
		case MSG_REGISTER:
			*ch++ = msg->ID;
			*ch = '\0';
			break;

		case MSG_REQUEST_RFID:
			// 4-byte serial number.
			memcpy(ch, msg->buffer, 4);
			*(ch+4) = '\0';
			break;

		case MSG_ROUND_COMPELETE:
			// No additional message
			break;

		case MSG_CUSTOM:
			*ch++ = msg->ID;
			memcpy(ch, msg->buffer, COMM_MSG_BUF_LEN);
			break;

		case MSG_CUSTOM_BROADCAST:
			memcpy(ch, msg->buffer, COMM_MSG_BUF_LEN);
			break;

		default:	// Invaild data type
			return false;
	}

	return puts(buffer);
}

bool BRCClient::receiveMessage(CommMsg *msg)
{
	char buffer[COMM_MSG_BUF_LEN + 2];
	char *ch = buffer;

	if (gets(buffer, COMM_MSG_BUF_LEN + 2) == -1)
		return false;

	msg->type = *ch;
	switch (*ch++) {
		case MSG_REGISTER:
			msg->ID = *ch++;
			memcpy(msg->buffer, ch, COMM_MSG_BUF_LEN);
			break;

		case MSG_REQUEST_RFID:
			// 2-byte coordination (x, y)
			memcpy(msg->buffer, ch, 2);
			(msg->buffer)[2] = '\0';
			break;

		case MSG_ROUND_START:
		case MSG_ROUND_END:
			msg->ID = *ch++;
			break;

		case MSG_CUSTOM:
		case MSG_CUSTOM_BROADCAST:
			msg->ID = *ch++;
			memcpy(msg->buffer, ch, COMM_MSG_BUF_LEN);
			break;

		default:	// Invaild data type
			return false;
	}

	return true;
}
