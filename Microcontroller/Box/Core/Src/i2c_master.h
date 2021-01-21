#ifndef __I2C_MASTER_H
#define __I2C_MASTER_H

#ifdef __cplusplus
extern "C" {
#endif
	#include "stm32l1xx_hal.h"
#ifdef __cplusplus
}
#endif

class I2C_Master {
 public:
	void init();
	void write(uint8_t addr, uint8_t* data, uint32_t len, bool repeated_start = false);
	void read(uint8_t addr, uint8_t* data, uint32_t len, bool stop_cond = false);
};

#endif
