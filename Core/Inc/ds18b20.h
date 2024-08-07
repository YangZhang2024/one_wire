
#ifndef __DS18B20_H
#define __DS18B20_H

#include "stdint.h"
#include "one_wire.h"

uint8_t ds18b20_init(void);         /* 初始化DS18B20 */
uint8_t ds18b20_check(void);        /* 检测是否存在DS18B20 */
short ds18b20_get_temperature(void);/* 获取温度 */

#endif















