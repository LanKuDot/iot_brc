/**
 * @file BRCClient/BRCClient.h
 * @brief The header file of class BRCClient
 */
#ifndef _BRC_CLIENT_H_
#define _BRC_CLIENT_H_

#include <KSM111_ESP8266.h>
#include "CommMsg.h"

/**
 * @class BRCClient BRCClient.h <BRCClient.h>
 * @brief The API for using KSM111_ESP8266 module to communicate with BRC server.
 */
class BRCClient : public KSM111_ESP8266
{
	public:
		/**
		 * @brief Use <tt>SoftwareSerial</tt> to communicate with the module.
		 */
		BRCClient(int rxPin, int txPin, int resetPin = -1)
			: KSM111_ESP8266(rxPin, txPin, resetPin), _myID(0xFF) {}

		/**
		 * @brief For MEGA board, use <tt>HardwareSerial</tt> to communicate with the module.
		 */
		BRCClient(HardwareSerial *hws, int resetPin = -1)
			: KSM111_ESP8266(hws, resetPin), _myID(0xFF) {}

		/**
		 * @brief Join AP and connect to the BRC server.
		 *
		 * The module would be set to STATION mode, disable the multiple connections,
		 * and quit any joined AP. Then call <tt>joinAP()</tt> and <tt>beginClienet()</tt>
		 * to join new AP and connect to the BRC server.
		 *
		 * @param ssid The ssid of AP.
		 * @param passwd The password of AP.
		 * @param serverIP The IP of the BRC server.
		 * @param port The port of the BRC srever.
		 * @return true if the module successfully connects to the BRC server.
		 */
		bool beginBRCClient(const char *ssid, const char *passwd, const char *serverIP, const int port);

		/**
		 * @brief Disconnect from the BRC server and quit from AP.
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

		/**
		 * @brief Send the message to the specified BRC client.
		 *
		 * Note that the length of <tt>message</tt> can't be more than
		 * COMM_MSG_BUF_LEN - 1, you have to reserve 1 byte for null-character.
		 *
		 * @param ID The ID of the client who will receive the message.
		 * @param message The buffer of the message
		 */
		bool sendToClient(const uint8_t ID, const char *message);

		/**
		 * @brief Broadcast the message to all the BRC clients.
		 *
		 * Note that the length of <tt>message</tt> can't be more than
		 * COMM_MSG_BUF_LEN - 1, you have to reserve 1 byte for null-character.
		 *
		 * @param message The buffer of the message
		 */
		bool broadcast(const char *message);

	private:
		/**
		 * @brief The ID representing itself in the BRC server.
		 */
		uint8_t _myID;
};

#endif
