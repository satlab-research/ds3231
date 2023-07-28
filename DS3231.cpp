#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "DS3231.h"


DS3231::DS3231(/*i2c_inst_t *inst, int sda, int scl*/)
{
    // this->inst = inst;
    // this->sda = sda;
    // this->scl = scl;

    i2c_init(this->inst, 400*1000); // 100 Khz.

    gpio_set_function(this->sda, GPIO_FUNC_I2C);
    gpio_set_function(this->scl, GPIO_FUNC_I2C);
    gpio_pull_up(this->sda);
    gpio_pull_up(this->scl);

}

DS3231::~DS3231()
{
}



uint8_t DS3231::dectobcd(const uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

uint8_t DS3231::bcdtodec(const uint8_t val)
{
    return ((val / 16 * 10) + (val % 16));
}


void DS3231::set_date(uint8_t date, uint8_t month, uint8_t year, uint8_t day)
{

    uint8_t sYbuf[2];
    sYbuf[0] = REG_YEAR;
    sYbuf[1] = ((year / 10) << 4) | (year % 10);
    i2c_write_blocking( inst, DEVICE_ADDR, sYbuf, 2, false);

    uint8_t sMbuf[2];
    sMbuf[0] = REG_MON;
    sMbuf[1] = ((month / 10) << 4) | (month % 10);
    i2c_write_blocking( inst, DEVICE_ADDR, sMbuf, 2, false);

    uint8_t sDbuf[2];
    sDbuf[0] = REG_DATE;
    sDbuf[1] = dectobcd(date);
    i2c_write_blocking( inst, DEVICE_ADDR, sDbuf, 2, false);

    uint8_t sDwbuf[2];
    sDwbuf[0] = REG_DOW;
    sDwbuf[1] = dectobcd(day);
    i2c_write_blocking( inst, DEVICE_ADDR, sDwbuf, 2, false);


}

void DS3231 :: read_date(uint8_t *date, uint8_t *month, uint8_t *year, uint8_t *day)
{
    uint8_t buf[1] = { REG_DOW };
    uint8_t result[3] = { 0x00 };
    i2c_write_blocking( inst, DEVICE_ADDR, buf, 1, true);
    i2c_read_blocking( inst, DEVICE_ADDR, result, 4, false);

    *day = result[0];

    *date = ( 10*(result[1]>>4) ) + ( result[1] & (0x0F) ) ;

    *month =( 10*(result[2]>>4) ) + ( result[2] & (0x0F) ) ;;

    *year = ( 10*(result[3]>>4) ) + ( result[3] & (0x0F) ) ;
    
}



void DS3231::set_time(uint8_t hour, uint8_t min, uint8_t sec)
{

    uint8_t setSbuf[2];
    setSbuf[0] = REG_SEC;
    setSbuf[1] = dectobcd(sec);
    i2c_write_blocking( inst, DEVICE_ADDR, setSbuf, 2, false);


    uint8_t setMbuf[2];
    setMbuf[0] = REG_MIN;
    setMbuf[1] = dectobcd(min);
    i2c_write_blocking( inst, DEVICE_ADDR, setMbuf, 2, false);


    uint8_t setHbuf[2];
    setHbuf[0] = REG_HOUR;
    setHbuf[1] = dectobcd(hour);
    i2c_write_blocking( inst, DEVICE_ADDR, setHbuf, 2, false);

    // printf("Written time\n");

}

void DS3231::read_time(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint8_t buf[1] = { REG_SEC };
    uint8_t result[3] = { 0x00 };
    i2c_write_blocking( inst, DEVICE_ADDR, buf, 1, true);
    i2c_read_blocking( inst, DEVICE_ADDR, result, 3, false);

    uint8_t data[3];

    *sec =  ( 10*(result[0]>>4) ) + ( result[0] & (0x0f) ) ;
    *min =  ( 10*(result[1]>>4) ) + ( result[1] & (0x0f) ) ;

    *hour =  (result[2] & 15) + (10 * ((result[2] & 48) >> 4));

}
