#include "main.h"


void BSP_Init(void)	//ע���ʼ����˳��
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(180);
	Power_Configuration();
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_2tq,CAN_BS1_6tq,5,CAN_Mode_Normal);	
	TIM6_Init();
	Pid_M201_Init();
	delay_ms(1000);

	
}

