/**
 * @file KSM111_ESP8266/KSM111_ESP8266.h
 * @brief The header file of class KSM111_ESP8266.
 */
#ifndef _KSM111_ESP8266_H_
#define _KSM111_ESP8266_H_

#include <stdint.h>
#include <Stream.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>

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

/* Error Code from joining AP */
#define JAP_OK 1
#define ERR_JAP_TIMEOUT       -1	// Connection timeout
#define ERR_JAP_WRONG_PASSWD  -2	// Wrong password
#define ERR_JAP_AP_NOT_FOUND  -3	// Can not found target AP
#define ERR_JAP_CONNECT_FAIL  -4	// Connect fail

/* Serial type tag */
enum {HARD, SOFT};

/**
 * @struct AccessPointInfo KSM111_ESP8266/KSM111_ESP8266.h <KSM111_ESP8266.h>
 * @brief A data structure for storing the information of AP.
 */
typedef struct AccessPointInfo {
	uint8_t encrypt;	///< The encryption type
	char    ssid[33];	///< Max 32 characters, 1 for null char.
	int     rssi;		///< The signal strength in dBm
	char    mac[18];	///< 6-byte hex addr with semicolon seperators, 1 for null char
	int     ch;			///< Channel
} APInfo;

/**
 * @class KSM111_ESP8266 KSM111_ESP8266.h <KSM111_ESP8266.h>
 * @brief The basic class which directly communicating with the KSM111_ESP8266 module.
 */
class KSM111_ESP8266 {
	public:
		/**
		 * @brief Initialize the pins of <tt>SoftwareSerial</tt> and <tt>_resetPin</tt>
		 * @param rxPin The number of pin connected to the TXD pin of the module.
		 * @param txPin The number of pin connected to the RXD pin of the nodule.
		 * @param resetPin [optional] ]The number of pin connected to the RST pin of the module.
		 */
		KSM111_ESP8266(int rxPin, int txPin, int resetPin = -1)
			: _serial(new SoftwareSerial(rxPin, txPin)), _resetPin(resetPin), _serialType(SOFT) {}

		/**
		 * @brief Constructor for using <tt>HardwareSerial</tt> to communicate with module.
		 */
		KSM111_ESP8266(HardwareSerial *hws, int resetPin = -1)
			: _serial(hws), _resetPin(resetPin), _serialType(HARD) {}

		/**
		 * @brief Set the buadrate of <tt>_serial</tt> and begin it
		 * @param baudrate Specify the baudrate of <tt>_serial</tt>.
		 * @return true if the module is successfully waked up.
		 */
		bool begin(long baudrate);

		/**
		 * @brief Close the connection of <tt>_serial</tt>.
		 */
		void end();

		/**
		 * @brief Restart the module by AT command.
		 *        It takes 5 seconds to wait for restarting.
		 * @return true if the module responses "OK"
		 */
		bool softReset();

		/**
		 * @brief Set the operating mode of the module. The method spends 0.5 seconds.
		 * @param mode The operating mode: STATION, AP, or BOTH
		 * @return true if the module responses "OK"
		 */
		bool setMode(uint8_t mode);

		/**
		 * @brief Get the operating mode of the module. The method spends 0.1 seconds.
		 * @return The operating mode
		 */
		uint8_t getMode();

		/**
		 * @brief Set the default UART baudrate of the module.
		 *
		 * Note that resetting the module cannot reset the baudrate.<br />
		 * After the new baudrate has been successfully set, the baudrate of <tt>_serial</tt>
		 * will also be set to the new baudrate.
		 *
		 * @param baudrate The baudrate to be set.
		 * @return true if the module responses "OK".
		 */
		bool setBaudrate(long baudrate);

		/**
		 * @name Access point operations
		 * The operations of accessing an AP.
		 */
		/** @{ */
		/**
		 * @brief List avalible access points. The method spends 5 seconds.
		 * @param apList [out] Store the information of access points
		 * @param count [in] The max amount of listing access points
		 * @param vaildCount [out] The number of vaild access points in <tt>apList</tt>.
		 * @return true if the responsing message contains "OK".
		 *         If the device is in AP mode, it will responses "ERROR".
		 */
		bool listAP(APInfo *apList, int count, int *vaildCount);
		/**
		 * @brief Join an AP. The method spends 8 seconds.
		 * @param ssid The ssid of the AP
		 * @param passwd The password of the AP
		 * @return The connection status of joining AP
		 * @retval JAP_OK Success
		 * @retval ERR_JAP_TIMEOUT Connecting timeout
		 * @retval ERR_JAP_WRONG_PASSWD Wrong passwrod
		 * @retval ERR_JAP_AP_NOT_FOUND Can not found target AP
		 * @retval ERR_JAP_CONNECT_FAIL Connect fail
		 */
		int8_t joinAP(const char *ssid, const char *passwd);
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
		 * @brief Disconnect from the server but not quiting AP. Th method spends 0.25 seconds.
		 * @return True if successfully disconnected
		 */
		bool endClient();

		/**
		 * @brief Get the IP address of the station or softAP. This method spends 0.5 seconds.
		 * @param mode STATION or AP (softAP) mode
		 * @param ip [out] The IP address
		 */
		void getIP(uint8_t mode, char *ip);

		/**
		 * @brief Send a message to server.
		 *
		 * Note that this function can be only used in the client.
		 *
		 * @param msg [input] The message wants to passed to AP
		 * @return True if it sends successfully
		 */
		 bool puts(const char *msg);

		 /**
		  * @brief Receive the message sent from the server.
		  * @param msg [out] The buffer for receiving message
		  * @param buffLen [in] The max length of the buffer _msg_ including null character.
		  * @return The ID of the sender. In single conenction mode, it always returns 0.
		  * @retval -1 There is no incoming message.
		  */
		 int8_t gets(char * const msg, unsigned int buffLen);

	private:
		/**
		 * @brief The interface for communicating with the module.
		 */
		Stream *_serial;

		/**
		 * @brief Record either SoftwareSerial or HarewareSerial is in use.
		 */
		int _serialType;

		/**
		 * @brief The number of pin which is connected to the RST pin of the module.
		 */
		int _resetPin;

		/**
		 * @brief The buffer for temporarily storing the message.
		 */
		char _buff[128];
};

#endif // _KSM111_ESP8266_H_
