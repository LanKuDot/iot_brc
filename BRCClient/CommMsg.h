#ifndef _COMM_MSG_H_
#define _COMM_MSG_H_

/**
 * @name Communication data type
 */
/** @{ */
#define MSG_REGISTER         (char)0x01
#define MSG_REQUEST_RFID     (char)0x10
#define MSG_ROUND_COMPLETE   (char)0x11
#define MSG_ROUND_START      (char)0x20
#define MSG_ROUND_END        (char)0x21
#define MSG_CUSTOM           (char)0x70
#define MSG_CUSTOM_BROADCAST (char)0x71
/** @} */

#define COMM_MSG_BUF_LEN 30

/**
 * @struct COMM_MESSAGE BRCClient/CommMsg.h "CommMsg.h"
 * @brief The data structure for communicating with the central terminal.
 */
typedef struct COMM_MESSAGE {
	char type;           ///< The type of the message
	char ID;             ///< The ID of the sender or receiver
	char buffer[COMM_MSG_BUF_LEN]; ///< The message buffer. Reserve 1 byte for null character.
} CommMsg;

#endif //_COMM_MSG_H_
