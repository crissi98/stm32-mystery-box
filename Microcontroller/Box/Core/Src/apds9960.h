/*
 * apds9960.h
 *
 *  Created on: Jan 8, 2021
 *      Author: ap7u5
 */

#ifndef SRC_APDS9960_H_
#define SRC_APDS9960_H_

#include <cstdint>
#include "apds9960_enum.h"


template <int (*i2c_write)(uint8_t* buf, uint32_t len), int (*i2c_read)(uint8_t* buf, uint32_t len)>
class APDS9960 {
private:
	uint8_t readBuffer[1];
	uint8_t writeBuffer[2];

	uint8_t getRegisterValue(uint8_t addr) {
		// Zu lesendes Register setzen
		readBuffer[0] = addr;
		i2c_write(readBuffer, 1);
		// Wert aus Register lesen
		i2c_read(readBuffer, 1);
		return *readBuffer;
	}

	void setRegister(uint8_t addr, uint8_t value) {
		writeBuffer[0] = addr;
		writeBuffer[1] = value;

		i2c_write(writeBuffer, 2);
	}


public:
	APDS9960(): readBuffer{0}, writeBuffer{0} {
	}

	void init() {
		setRegister(APDS9960RegisterEnable, 0);
	}

	void powerOn() {
		uint8_t enableValue = getRegisterValue(REGISTERS::ENABLE);
		enableValue |= CONTROL::POWER_ON;
		setRegister(REGISTERS::ENABLE, enableValue);
	}

	void proximityOn() {
		uint8_t enableValue = getRegisterValue(REGISTERS::ENABLE);
		enableValue |= CONTROL::PROXIMITY_ON;
		setRegister(REGISTERS::ENABLE, enableValue);
	}

	void alsOn() {
		uint8_t enableValue = getRegisterValue(REGISTERS::ENABLE);
		enableValue |= CONTROL::ALS_ON;
		setRegister(REGISTERS::ENABLE, enableValue);
	}

	uint8_t getProximityData() {
		return getRegisterValue(REGISTERS::PROXIMITY_DATA);
	}

	uint16_t getAlsClearData() {
		uint16_t result = getRegisterValue(REGISTERS::CLEAR_DATA_LOW);
		result |= getRegisterValue(REGISTERS::CLEAR_DATA_HIGH) << 8;
		return result;
	}

};


#endif /* SRC_APDS9960_H_ */
