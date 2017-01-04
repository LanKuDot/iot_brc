#ifndef _MAP_MSG_H_
#define _MAP_MSG_H_

#include <stdint.h>
#include <string.h>

/**
 * @name Map type
 */
/** @{ */
#define MAP_NORMAL   (char)0x01
#define MAP_TREASURE (char)0x02
#define MAP_PARK_1   (char)0x21
#define MAP_PARK_2   (char)0x22
#define MAP_PARK_3   (char)0x23
#define MAP_PARK_4   (char)0x24
#define MAP_INVAILD  (char)0xFF
/** @} */

#define CUSTOM_TAG_LEN 23

/**
 * @struct MAP_MESSAGE BRCClient/MapMsg.h "MapMsg.h"
 * @brief The data structure for the information of a map block.
 *
 * The coordination of an invaild map block will be (-1, -1), and
 * its type will be MAP_INVAILD.
 */
typedef struct MAP_MESSAGE {
	uint8_t sn[4];	///< The RFID serial number of a map block
	int8_t x;		///< The x coordinate of a map block.
	int8_t y;		///< The y coordinate of a map block.
	char type;		///< The type of a map block.
} MapMsg;

/**
 * @brief Convert the raw message to the MapMsg.
 *
 * The length of <tt>rawData</tt> must be less than 30 (null character included).
 *
 * @rawData The pointer to the buffer of raw data. Must be null terminated.
 */
static MapMsg rawDataToMapMsg(const char * const rawData)
{
	const char *ch = rawData;
	MapMsg mapMsg;

	mapMsg.sn[0] = (uint8_t)ch[0];
	mapMsg.sn[1] = (uint8_t)ch[1];
	mapMsg.sn[2] = (uint8_t)ch[2];
	mapMsg.sn[3] = (uint8_t)ch[3];
	mapMsg.x = ch[4];
	mapMsg.y = ch[5];
	mapMsg.type = ch[6];

	return mapMsg;
}

#endif // _MAP_MSG_H_
