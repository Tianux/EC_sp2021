#ifndef __REFEREE_CRC_H
#define __REFEREE_CRC_H

#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"


//crc8 ���ɶ���ʽ:G(x)=x8+x5+x4+1
extern const unsigned char CRC8_INIT2;
extern const unsigned char CRC8_TAB[256];


//crc16 ���ɶ���ʽ: δ֪
extern uint16_t CRC_INIT;
extern const uint16_t wCRC_Table[256];


/*
** Descriptions: CRC8 У��������
** Input: ��Ϣ����, ��Ϣ���ݳ�, CRC8_INIT2(0xff)
** Output: CRC8У����
*/
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8);


/*
** Descriptions: CRC8 У�����
** Input: ��У������, ���ݳ���(= ��Ϣ���� + У���볤��)
** Output: True or False (CRC Verify Result)
*/
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);


/*
** Descriptions: ���� CRC8 ����Ϣ����β��
** Input: ��Ϣ����(β��������1λ�Լ���У����), ��Ϣ���ݳ���
** Output: ��У���������
*/
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);

/*
** Descriptions: CRC16 У��������
** Input: ��Ϣ����, ��Ϣ���ݳ�, CRC_INIT(0xffff)
** Output: CRC16У����
*/
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);


/*
** Descriptions: CRC16 У�����
** Input: ��У������, ���ݳ���(= ��Ϣ���� + У���볤��, ��������У����ĳ���)
** Output: True or False (CRC Verify Result)
*/
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);

/*
** Descriptions: ���� CRC16 ����Ϣ����β��
** Input: ��Ϣ����(β��������2λ�Լ���У����), ��Ϣ���ݳ���
** Output: ��У���������
*/
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);


// ���ֽ�����תu16(2�ֽ�), �ߵ�λ����
uint16_t _bytes2u16(uint8_t * chosen_Message);


// ���ֽ�����תu32(4�ֽ�), �ߵ�λδ֪
uint32_t _bytes4u32(uint8_t * chosen_Message);


// ���ֽ�����תfloat(4�ֽ�), �ߵ�λ����
float _bytes2float(uint8_t * chosen_Message);


// floatת4���ȵ��ֽ�����
void float2bytes(float chosen_value, uint8_t * res_message);

#endif // __REFEREE_CRC_H
