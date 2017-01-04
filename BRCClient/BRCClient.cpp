#include <Arduino.h>
#include <string.h>

#include "BRCClient.h"

bool BRCClient::beginBRCClient(const char *ssid, const char *passwd, const char *serverIP, const int port)
{
	char joinedSSID[32];
	memset(joinedSSID, 0, 32);

	// Check if the module joined an AP.
	if (!joinedAP(joinedSSID) ||
	    strcmp(joinedSSID, ssid) != 0) {
		quitAP();
		setMode(STATION);
		multiConnect(false);
		if (joinAP(ssid, passwd) < 0)
			return false;
	}
	if (beginClient("TCP", serverIP, port) != CONNECT_ERROR)
		return true;

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

		case MSG_ROUND_COMPLETE:
			// No additional message
			break;

		case MSG_CUSTOM:
			*ch++ = msg->ID;
			memcpy(ch, msg->buffer, COMM_MSG_BUF_LEN);
			break;

		case MSG_REQUEST_RFID:
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
			msg->ID = 0x01;	// Data is sent from server.
			memcpy(msg->buffer, ch, COMM_MSG_BUF_LEN);
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

bool BRCClient::registerID(const uint8_t ID)
{
	// Invaild register ID
	if (ID == (uint8_t)0xFF ||
	    (ID >= (uint8_t)0x00 && ID < (uint8_t)0x10)) {
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
		_myID = ID;
		return true;
	} else
		return false;
}

bool BRCClient::sendToClient(const uint8_t ID, const char *message)
{
	CommMsg msg = {
		.type = MSG_CUSTOM,
		.ID = ID
	};
	strncpy(msg.buffer, message, COMM_MSG_BUF_LEN);
	sendMessage(&msg);

	// Delay for a while to receive the response
	delay(1);
	if (!receiveMessage(&msg))
		return false;

	if (msg.ID == _myID &&
		strcmp(msg.buffer, "OK") == 0)
		return true;
	else
		return false;
}

bool BRCClient::broadcast(const char *message)
{
	CommMsg msg = {
		.type = MSG_CUSTOM_BROADCAST
	};
	strncpy(msg.buffer, message, COMM_MSG_BUF_LEN);
	sendMessage(&msg);

	// Delay for a while to receive the response
	delay(1);
	if (!receiveMessage(&msg))
		return false;

	if (msg.ID == _myID &&
		strcmp(msg.buffer, "OK") == 0)
		return true;
	else
		return false;
}

void BRCClient::requestMapData(const uint8_t *sn)
{
	CommMsg msg = {
		.type = MSG_REQUEST_RFID
	};
	memcpy(msg.buffer, sn, 4);
	msg.buffer[4] = '\0';

	sendMessage(&msg);
	delay(1);
}
