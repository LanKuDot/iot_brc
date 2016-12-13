#ifndef _COMM_MSG_H_
#define _COMM_MSG_H_

/**
 * @name Communication data type
 */
/** @{ */
#define MSG_REGISTER         0x01
#define MSG_REQUEST_RFID     0x10
#define MSG_ROUND_COMPELETE  0x11
#define MSG_ROUND_START      0x20
#define MSG_ROUND_END        0x21
#define MSG_CUSTOM           0x70
#define MSG_CUSTOM_BROADCAST 0x71
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
