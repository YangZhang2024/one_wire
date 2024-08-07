#include "ds18b20.h"

/**
 * @brief       开始温度转换
 * @param       无
 * @retval      无
 */
static void ds18b20_start(void) {
    one_wire_reset();
    one_wire_check();
    one_wire_write_byte(0xcc);   /*  skip rom */
    one_wire_write_byte(0x44);   /*  convert */
}

/**
 * @brief       初始化DS18B20的IO口 DQ 同时检测DS18B20的存在
 * @param       无
 * @retval      0, 正常
 *              1, 不存在/不正常
 */
uint8_t ds18b20_init(void) {
    one_wire_init();
    one_wire_reset();
    return one_wire_check();
}

/**
 * @brief       从ds18b20得到温度值(精度：0.1C)
 * @param       无
 * @retval      温度值 （-550~1250）
 *   @note      返回的温度值放大了10倍.
 *              实际使用的时候,要除以10才是实际温度.
 */
short ds18b20_get_temperature(void) {
    uint8_t flag = 1;           /* 默认温度为正数 */
    uint8_t TL, TH;
    short temp;

    ds18b20_start();            /*  ds1820 start convert */
    one_wire_reset();
    one_wire_check();
    one_wire_write_byte(0xcc);   /*  skip rom */
    one_wire_write_byte(0xbe);   /*  convert */
    TL = one_wire_read_byte();   /*  LSB */
    TH = one_wire_read_byte();   /*  MSB */

    if (TH > 7) {
        /* 温度为负，查看DS18B20的温度表示法与计算机存储正负数据的原理一致：
            正数补码为寄存器存储的数据自身，负数补码为寄存器存储值按位取反后+1
            所以我们直接取它实际的负数部分，但负数的补码为取反后加一，但考虑到低位可能+1后有进位和代码冗余，
            我们这里先暂时没有作+1的处理，这里需要留意  */
        TH = ~TH;
        TL = ~TL;
        flag = 0;   /* 温度为负 */
    }

    temp = TH;      /* 获得高八位 */
    temp <<= 8;
    temp += TL;     /* 获得底八位 */

    /* 转换成实际温度 */
    if (flag == 0) {   /* 将温度转换成负温度，这里的+1参考前面的说明 */
        temp = (double) (temp + 1) * 0.625;
        temp = -temp;
    } else {
        temp = (double) temp * 0.625;
    }

    return temp;
}
















