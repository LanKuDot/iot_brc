#ifndef _KSM111_ESP8266_H_
#define _KSM111_ESP8266_H_

#include <stdint.h>
#include <SoftwareSerial.h>

/* The mode of wifi */
#define STATION 1
#define AP      2
#define BOTH    3	// STATION and AP

/* Encryption type */
#define OPEN         0
#define WEP          1
#define WPA_PSK      2
#define WPA2_PSK     3
#define WPA_WPA2_PSK 4

/* Connection status */
#define CONNECT_OK       0
#define CONNECT_ERROR   -1
#define ALREADY_CONNECT  1

/**
 * @struct AccessPointInfo
 * @brief A data structure for storing the information of AP.
 */
typedef struct AccessPointInfo {
	uint8_t encrypt;	///< The encryption type
	char    ssid[33];	///< Max 32 characters, 1 for null char.
	int     rssi;		///< The signal strength in dBm
	char    mac[18];	///< 6-byte hex addr with semicolon seperators, 1 for null char
	int     ch;			///< Channel
} APInfo;

class KSM111_ESP8266 {
	public:
		/**
		 * @brief Initialize the pins of <tt>SoftwareSerial</tt> and <tt>_resetPin</tt>
		 * @param rxPin The number of pin connected to the TXD pin of the module.
		 * @param txPin The number of pin connected to the RXD pin of the nodule.
		 * @param resetPin [optional] ]The number of pin connected to the RST pin of the module.
		 */
		KSM111_ESP8266(int rxPin, int txPin, int resetPin = -1)
			: _serial(rxPin, txPin), _resetPin(resetPin) {}

		/**
		 * @brief Set the buadrate of <tt>_serial</tt> to 115200.
		 *        It takes 5 seoncds to wake up the wifi module.
		 * @return true if the module is successfully waked up.
		 */
		bool begin();

		/**
		 * @brief Close the connection of SoftwareSerial.
		 */
		void end();

		/**
		 * @brief Restart the module by AT command.
		 *        It takes 5 seconds to wait for restarting.
		 * @return true if the module responses "OK"
		 */
		bool softReset();

		/**
		 * @brief Set the operating mode of the module. The method takes 0.5 seconds.
		 * @param mode The operating mode: STATION, AP, or BOTH
		 * @return true if the module responses "OK"
		 */
		bool setMode(uint8_t mode);

		/**
		 * @brief Get the operating mode of the module. The method takes 0.1 seconds.
		 * @return The operating mode
		 */
		uint8_t getMode();

		/**
		 * @name Access point operations
		 * The operations of accessing an AP.
		 */
		/** @{ */
		/**
		 * @brief List avalible access points. The method takes 5 seconds.
		 * @param apList [out] Store the information of access points
		 * @param count [in] The max amount of listing access points
		 * @param availCount [out] The number of vaild access points in <tt>apList</tt>.
		 * @return true if the responsing message contains "OK".
		 *         If the device is in AP mode, it will responses "ERROR".
		 */
		bool listAP(APInfo *apList, int count, int *vaildCount);
		/**
		 * @brief Join an AP. The method takes 8 seconds.
		 * @param ssid The ssid of the AP
		 * @param passed The password of the AP
		 */
		bool joinAP(const char *ssid, const char *passwd);
		/**
		 * @brief Quit from the joined AP.
		 */
		void quitAP();
		/** @} */

		/**
		 * @brief Enable multiple connections or not.
		 *
		 * Only when transparent transmission disabled (AT+CIPMODE=0) can enable multiple connections.
		 * Only when in non-AP mode and no active connection can be set to single connection.
		 *
		 * @param enable True to enable multiple connection. Otherwise, single connection.
		 * @return True, if the module responses "OK".
		 */
		bool multiConnect(bool enable);

		/**
		 * @brief Establish TCP conenction or register UDP port, and start connection.
		 * @param type "TCP" or "UDP"
		 * @param ip The ip of the server
		 * @param port The port number of the server
		 * @return The status of the connection
		 * @retval CONNECT_OK Success
		 * @retval CONNECT_ERROR Failed
		 * @retval ALREADY_CONNECT Already connect to this server
		 */
		uint8_t beginClient(const char *type, const char *ip, const int port);

		/**
		 * @brief Disconnect from the server but not quiting AP. Th method takes 0.25 seconds.
		 * @return True if successfully disconnected
		 */
		bool endClient();

		/**
		 * @brief Get the ip address of module station or module AP. This method takes 0.5 seconds.
		 * @param mode STATION or AP
		 * @param ip [out] The IP address
		 */
		void getIP(uint8_t mode, char *ip);

		/**
		 * @brief Send a message
		 * @param msg [input] The message wants to passed to AP
		 * @return True if it sends successfully
		 */
		 bool puts(const char *msg);

		 /* @brief Receive the message send from others
		  * @param msg [out] The buffer for receiving message
		  * @param buffLen [in] The max length of the buffer _msg_ including null character.
		  * @return The ID of the sender. In single conenction mode, always return 0.
		  * @retval -1 There is no message to read.
		  */
		 int8_t gets(char * const msg, unsigned int buffLen);

	private:
		/**
		 * @brief The interface for communicating with the module.
		 */
		SoftwareSerial _serial;

		/**
		 * @brief The number of pin which is connected to the RST pin of the module.
		 */
		int _resetPin;

		/**
		 * @brief The buffer for temporarily storing the message.
		 */
		char _buff[512];
};

#endif // _KSM111_ESP8266_H_
