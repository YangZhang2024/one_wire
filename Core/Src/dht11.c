#include "dht11.h"
#include "stdio.h"

unsigned char read_byte() {
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++) {
        while (!ONE_WIRE_DQ_IN);
        delay_us(30);
        dat = (dat << 1) | ONE_WIRE_DQ_IN;
        while (ONE_WIRE_DQ_IN);
    }
    return dat;
}


void get_humidity(double *temp, double *humidity) {
    ONE_WIRE_DQ_OUT(1);
    ONE_WIRE_DQ_OUT(0);
    delay_ms(20);
    ONE_WIRE_DQ_OUT(1);

    delay_us(13);
    while (!ONE_WIRE_DQ_IN); // 等低响应结束
    while (ONE_WIRE_DQ_IN);  // 等高响应结束

    uint8_t RH, RL, TH, TL, revise;
    RH = read_byte();    //接收湿度高八位
    RL = read_byte();    //接收湿度低八位
    TH = read_byte();    //接收温度高八位
    TL = read_byte();    //接收温度低八位
    revise = read_byte(); //接收校正位
    printf("RH: %c, RL: %c, TH: %c, TL: %c, revise: %c \r\n", RH, RL, TH, TL, revise);

    if ((RH + RL + TH + TL) == revise)      //校正
    {
        uint8_t is_negative = 0x80 & TL;

        if (TL >= 10) {
            *temp = (TH + TL * 0.01);
        } else {
            *temp = (TH + TL * 0.1);
        }

        if (is_negative) {
            *temp = -(*temp);
        }

        is_negative = 0x80 & RL;
        if (RH >= 10) {
            *humidity = (RH + RL * 0.01);
        } else {
            *humidity = (RH + RL * 0.1);
        }
        if (is_negative) {
            *humidity = -(*humidity);
        }

    } else {
        *temp = -1000;
        *humidity = -1000;
    }

}