#ifndef __SENSOR_H
#define __SENSOR_H
#include "stm32f4xx.h"
#include "sys.h"
#include "i2c.h"
#include "sp_conf.h"

uint8_t SensorWrite_PF(uint8_t Slave_addr,uint8_t addr,uint16_t* databuffer);
uint8_t SensorRead_PF(uint8_t Slave_addr,uint8_t addr,uint8_t cnt,uint16_t* requestdata);
uint8_t SensorWrite_PE(uint8_t Slave_addr,uint8_t addr,uint16_t* databuffer);
uint8_t SensorRead_PE(uint8_t Slave_addr,uint8_t addr,uint8_t cnt,uint16_t* requestdata);
uint16_t distance(uint8_t Slave_addr,uint8_t addr,uint8_t cnt,uint16_t* requestdata);
//uint16_t GY56_distance(void);
uint16_t GY56_1_distance(void);
uint16_t GY56_2_distance(void);
uint16_t GY56ChangeSlaveAddress(uint8_t NewSlaveAddress);
uint16_t TOF10120_1distance(void);
uint16_t TOF10120_2distance(void);
uint16_t TOF10120_3distance(void);
uint16_t TOF10120_4distance(void);
uint16_t TOF10120_5distance(void);
uint16_t TOF10120_6distance(void);
uint16_t TOF10120_7distance(void);
uint16_t GP2Y0E03_distance(void);

uint16_t TOF10120_1distance_PE(void);
uint16_t TOF10120_2distance_PE(void);
uint16_t TOF10120_3distance_PE(void);

#endif
