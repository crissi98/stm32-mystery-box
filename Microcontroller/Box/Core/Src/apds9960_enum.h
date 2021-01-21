/*
 * apds9960_enum.h
 *
 *  Created on: Jan 8, 2021
 *      Author: ap7u5
 */

#ifndef SRC_APDS9960_ENUM_H_
#define SRC_APDS9960_ENUM_H_

enum REGISTERS {
	ENABLE = 0x80,
	PROXIMITY_DATA = 0x9C,
	CLEAR_DATA_LOW = 0x94,
	CLEAR_DATA_HIGH = 0x94
};

enum CONTROL {
	POWER_ON = 0x01,
	PROXIMITY_ON = 0x04,
	ALS_ON = 0x02
};

#endif /* SRC_APDS9960_ENUM_H */
