#ifndef __CAN_H
#define __CAN_H	 
#include "stm32f4xx.h"
//#include "sys.h"    
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//CAN���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.0 
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��
void continue_value(void);
void CAN1_Cmd(int16_t l_data1,int16_t l_data2,int16_t R_data1,int16_t R_data2);


volatile extern int16_t current_position_201; 
volatile extern int16_t current_speed_201;
volatile extern float continuous_current_position_201;
volatile extern int16_t current_position_202; 
volatile extern int16_t current_speed_202;
volatile extern float continuous_current_position_202;
volatile extern int16_t current_position_203; 
volatile extern int16_t current_speed_203;
volatile extern float continuous_current_position_203;
volatile extern int16_t current_position_204; 
volatile extern int16_t current_speed_204;
volatile extern float continuous_current_position_204;



#endif

















