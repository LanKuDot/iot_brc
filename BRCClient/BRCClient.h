#ifndef _BRC_CLIENT_H_
#define _BRC_CLIENT_H_

#include <KSM111_ESP8266.h>
#include <HardwareSerial.h>
#include "CommMsg.h"

class BRCClient : public KSM111_ESP8266
{
	public:
		BRCClient(int rxPin, int txPin, int resetPin = -1)
			: KSM111_ESP8266(rxPin, txPin, resetPin), _myID(0xFF) {}

		BRCClient(HardwareSerial *hws, int resetPin = -1)
			: KSM111_ESP8266(hws, resetPin) {}

		/**
		 * @brief Join AP and connect to the TCP server.
		 *
		 * The module would set to STATION mode and disable the multiple connections.
		 * Quit any joined AP, and then call <tt>joinAP()</tt> and <tt>beginClienet()</tt>
		 * to join new AP and connect to the TCP server.
		 *
		 * @param ssid The ssid of AP.
		 * @param passwd The password of AP.
		 * @param serverIP The IP of the TCP server.
		 * @param port The port of the TCP srever.
		 * @return true if the module successfully connects to the TCP server.
		 */
		bool beginBRCClient(const char *ssid, const char *passwd, const char *serverIP, const int port);

		/**
		 * @brief Disconnect from the TCP server and quit from AP.
		 *
		 * This function will call <tt>endClient()</tt> and <tt>quitAP()</tt> in sequence.
		 *
		 * @return true if the module successfully quits from AP.
		 */
		bool endBRCClient();

		/**
		 * @brief Send a message to the server.
		 *
		 * It's highly recommended that add a null character at theend of the message in the buffer.
		 * Therefore, the max number of vaild characters is COMM_MSG_BUF_LEN - 1,
		 * which reserving 1 byte for a null-charater.
		 *
		 * @param msg The pointer to the container of the message.
		 * @return true if the message is successfuly sent.
		 */
		bool sendMessage(CommMsg *msg);

		/**
		 * @brief Receive a message from the server.
		 * @param msg The pointer to the container of the message,
		 * @return true if there is an incoming message.
		 */
		bool receiveMessage(CommMsg *msg);

		/**
		 * @brief Register an ID representing itself on BRC server.
		 *
		 * Note that the _ID_ can't be 0xFF, or from 0x00 to 0x0F.
		 *
		 * @param ID The ID representing the module.
		 * @return true if the ID is successfully registered.
		 */
		bool registerID(const uint8_t ID);

	private:
		/**
		 * @brief The ID representing itself in the BRC server.
		 */
		uint8_t _myID;
};

#endif
