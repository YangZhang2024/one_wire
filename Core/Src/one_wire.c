#include "one_wire.h"
#include "stm32f1xx_hal.h"
#include "delay.h"

void one_wire_init(void) {
    GPIO_InitTypeDef gpio_init_struct;
    DS18B20_DQ_GPIO_CLK_ENABLE();   /* 开启DQ引脚时钟 */
    gpio_init_struct.Pin = ONE_WIRE_DQ_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;            /* 开漏输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(ONE_WIRE_DQ_GPIO_PORT, &gpio_init_struct); /* 初始化DS18B20_DQ引脚 */
}

void one_wire_reset(void) {
    ONE_WIRE_DQ_OUT(0);  /* 拉低DQ,复位 */
    delay_us(750);      /* 拉低750us */
    ONE_WIRE_DQ_OUT(1);  /* DQ=1, 释放复位 */
    delay_us(15);       /* 延迟15US */
}


/**
 * @brief       等待DS18B20的回应
 * @param       无
 * @retval      0, 正常
 *              1, 异常/不存在
 */
uint8_t one_wire_check(void) {
    uint8_t retry = 0;
    uint8_t rval = 0;

    while (ONE_WIRE_DQ_IN && retry < 200)    /* 等待DQ变低, 等待200us */
    {
        retry++;
        delay_us(1);
    }

    if (retry >= 200) {
        rval = 1;
    } else {
        retry = 0;

        while (!ONE_WIRE_DQ_IN && retry < 240)   /* 等待DQ变高, 等待240us */
        {
            retry++;
            delay_us(1);
        }

        if (retry >= 240) rval = 1;
    }

    return rval;
}


uint8_t one_wire_read_bit(void) {
    uint8_t data = 0;
    ONE_WIRE_DQ_OUT(0);
    delay_us(2);
    ONE_WIRE_DQ_OUT(1);
    delay_us(12);

    if (ONE_WIRE_DQ_IN) {
        data = 1;
    }

    delay_us(50);
    return data;
}

 uint8_t one_wire_read_byte(void) {
    uint8_t i, b, data = 0;

    for (i = 0; i < 8; i++) {
        b = one_wire_read_bit(); /* DS18B20先输出低位数据 ,高位数据后输出 */

        data |= b << i;         /* 填充data的每一位 */
    }

    return data;
}

 void one_wire_write_byte(uint8_t data) {
    uint8_t j;

    for (j = 1; j <= 8; j++) {
        if (data & 0x01) {
            ONE_WIRE_DQ_OUT(0);  /*  Write 1 */
            delay_us(2);
            ONE_WIRE_DQ_OUT(1);
            delay_us(60);
        } else {
            ONE_WIRE_DQ_OUT(0);  /*  Write 0 */
            delay_us(60);
            ONE_WIRE_DQ_OUT(1);
            delay_us(2);
        }

        data >>= 1;             /* 右移,获取高一位数据 */
    }
}