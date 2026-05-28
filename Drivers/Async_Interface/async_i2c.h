/*
 * async_i2c.h
 *
 *  Created on: 19 May 2026
 *      Author: vieth
 */

#ifndef ASYNC_INTERFACE_ASYNC_I2C_H_
#define ASYNC_INTERFACE_ASYNC_I2C_H_

#include <stdint.h>

typedef void (*SensorDoneCb_t)(uint8_t result, uint8_t *data, uint16_t len);

#define SENSOR_OK       0
#define SENSOR_BUSY     1
#define SENSOR_ERR      2

typedef enum {
	E_OK = 0,   /* send register address byte */
	E_BUSY,
	E_ERR,/* receive N data bytes       */
} Result_e;

uint8_t Sensor_ReadRegister(uint8_t regAddr, uint8_t *outBuf, uint16_t len,
                            SensorDoneCb_t doneCb);

/* Call these from STM32 HAL IRQ callbacks in stm32xx_it.c or main.c */
void Sensor_HAL_TxDoneCb(void);
void Sensor_HAL_RxDoneCb(void);


#endif /* ASYNC_INTERFACE_ASYNC_I2C_H_ */
