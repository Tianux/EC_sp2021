/************************************************************
 *File		:	can1.c
 *Author	:  @YangTianhao ,490999282@qq.com，@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.12.11
 *Description: 	Use CAN1 to control CM motor
								bound 1Mbps
								
								CMcontrol and GimbalControl Use Can1 together!   2017.2.23
 ************************************************************/

#include "main.h"	

//CAN初始化
//tsjw:重新同步跳跃时间单元. @ref CAN_synchronisation_jump_width   范围: ; CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   @ref CAN_time_quantum_in_bit_segment_2 范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   @refCAN_time_quantum_in_bit_segment_1  范围: ;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//波特率=Fpclk1/((tsjw+tbs1+tbs2+3)*brp);
//mode: @ref CAN_operating_mode 范围：CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Normal_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//则波特率为:42M/((1+6+7)*6)=500Kbps
//返回值:0,初始化OK;
//其他,初始化失败;




volatile int16_t current_position_206;    	//pitch gimbal
volatile int16_t current_position_205;			//yaw gimbal
volatile int16_t current_position_207;
volatile int16_t current_speed_207;
volatile int16_t current_cm_201;
volatile int16_t current_cm_202;
volatile int16_t current_cm_203;
volatile int16_t current_cm_204;
volatile int16_t current_cm_205;
volatile int16_t current_cm_206;
volatile int16_t current_fric_ti_201;   //201 左摩擦轮
volatile int16_t current_fric_ti_202;   //202 右摩擦轮
volatile int16_t current_fric_s_201;
volatile int16_t current_fric_s_202;

u8 rotate_205_count=0;
u8 rotate_206_count=0;
int16_t rotate_207_count=0;



volatile int16_t continuous_current_position_205;			//yaw gimbal
volatile int16_t continuous_current_position_206;    	//pitch gimbal
volatile int16_t continuous_current_position_filtered_205;			//yaw gimbal
volatile int16_t continuous_current_position_filtered_206;    	//pitch gimbal
volatile float continuous_current_position_207;
volatile int16_t previous_position_205;			//yaw gimbal
volatile int16_t previous_position_206;    	//pitch gimbal
volatile int16_t previous_position_207; 


volatile int16_t t_i_1=0;
volatile int16_t t_i_2=0;
volatile int16_t t_i_3=0;
volatile int16_t t_i_4=0;

SCM_rx_mes SCM_rx_message;//超级电容数据

//CAN1_Mode_Init
//功能：CAN1初始化函数
//参数：传入tsjw,tbs2,tbs1,brp,mode 决定CAN1的模式和波特率
//返回值：0
uint8_t CAN1_Mode_Init(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = mode;
    CAN_InitStructure.CAN_SJW = tsjw;
    CAN_InitStructure.CAN_BS1 = tbs1;
    CAN_InitStructure.CAN_BS2 = tbs2;
    CAN_InitStructure.CAN_Prescaler = brp;
    CAN_Init(CAN1, &CAN_InitStructure);

    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    return 0;
}

int count_207_temp=0;
int16_t yaw_speed=0;

Bytes2Float adi_angle;
Bytes2Float adi_gyro;
int16_t aid_dynamic_mach_angle=0;


void CAN1_RX0_IRQHandler(void)
{   
	CanRxMsg rx_message;
	
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
		{
				
        CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
					if(rx_message.StdId == 0x207)      //拨弹轮电机           
					{ 	
						auto_aim_count_++;		
							current_speed_207 = (rx_message.Data[2]<<8) | rx_message.Data[3];		
							current_position_207 = (rx_message.Data[0]<<8) | rx_message.Data[1];	
					}
					else if(rx_message.StdId == 0x502)      //ADI pitch         
					{ 	
						adi_angle.b[0]=rx_message.Data[0];
						adi_angle.b[1]=rx_message.Data[1];
						adi_angle.b[2]=rx_message.Data[2];
						adi_angle.b[3]=rx_message.Data[3];
						Pitch=adi_angle.f;
						adi_gyro.b[0]=rx_message.Data[4];
						adi_gyro.b[1]=rx_message.Data[5];
						adi_gyro.b[2]=rx_message.Data[6];
						adi_gyro.b[3]=rx_message.Data[7];
						adis16470_real_data.gyro_y=adi_gyro.f;					
					}
					else if(rx_message.StdId == 0x503)      //ADI yaw        
					{ 	
						
						adi_angle.b[0]=rx_message.Data[0];
						adi_angle.b[1]=rx_message.Data[1];
						adi_angle.b[2]=rx_message.Data[2];
						adi_angle.b[3]=rx_message.Data[3];
						Yaw=adi_angle.f;
						adi_gyro.b[0]=rx_message.Data[4];
						adi_gyro.b[1]=rx_message.Data[5];
						adi_gyro.b[2]=rx_message.Data[6];
						adi_gyro.b[3]=rx_message.Data[7];
						adis16470_real_data.gyro_z=adi_gyro.f;
					}
					else if(rx_message.StdId == 0x501)      //ADI yaw        
					{ 	
						if((rx_message.Data[1])==1)
						{
							clearing_flag=1;
							yaw_position_0=continuous_current_position_205;
						}
						else if((rx_message.Data[1])==2)
						{
							clearing_flag=0;
							aid_dynamic_mach_angle=continuous_current_position_205-yaw_position_0;
							CAN2_Send_Clear(aid_dynamic_mach_angle);
							aid_dynamic_mach_angle=0;
						}
					}
/****************************** c620电调 ******************************************/
//					else if(rx_message.StdId == 0x201)
//					{
//						current_fric_s_201 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
//						current_fric_ti_201=(rx_message.Data[4]<<8) | rx_message.Data[5];
//					}
//					else if(rx_message.StdId == 0x202)
//					{
//						current_fric_s_202 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
//						current_fric_ti_202=(rx_message.Data[4]<<8) | rx_message.Data[5];
//					}  
/************************************************************************************/
					
/***************************** 820R电调 *********************************************/
					else if(rx_message.StdId == 0x201)
					{
						current_fric_s_201 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
//						int16_t i=2;
//						while(i<4){
//						while( (USART_GetFlagStatus(USART2,USART_FLAG_TXE)) !=SET);
//						USART2->DR = rx_message.Data [i];
//						i++;
					}
					else if(rx_message.StdId == 0x202)
					{
						current_fric_s_202 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
					}
/************************************************************************************/
					
				CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    }
}


u8 CAN2_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	   GPIO_InitTypeDef       GPIO_InitStructure; 
     CAN_InitTypeDef        CAN_InitStructure;
     CAN_FilterInitTypeDef  CAN_FilterInitStructure;
     NVIC_InitTypeDef       NVIC_InitStructure;

  
     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);                                                                                                                   

     RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); 	
        

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
    GPIO_Init(GPIOB, &GPIO_InitStructure);
        

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2); //GPIOA11???CAN1
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2); //GPIOA12???CAN1
          

    CAN_InitStructure.CAN_TTCM=DISABLE;        
    CAN_InitStructure.CAN_ABOM=DISABLE;      
    CAN_InitStructure.CAN_AWUM=DISABLE;
    CAN_InitStructure.CAN_NART=ENABLE;       
    CAN_InitStructure.CAN_RFLM=DISABLE;        
    CAN_InitStructure.CAN_TXFP=DISABLE;        
    CAN_InitStructure.CAN_Mode= mode;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=tsjw;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;  //分频系数(Fdiv)为brp+1	   
    CAN_Init(CAN2, &CAN_InitStructure); 

		CAN_FilterInitStructure.CAN_FilterNumber=14;          
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 
    CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; 
    CAN_FilterInit(&CAN_FilterInitStructure);
                
    CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0                 
  
    NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
					
		return 0;
}   


static int u8count1 = 0; 
void CAN2_RX0_IRQHandler(void)
{   
	CanRxMsg rx_message;
	
    if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET) 
		{
        CAN_Receive(CAN2, CAN_FIFO0, &rx_message); 
	
        if(rx_message.StdId == 0x205)           //206 是pitch
        {             
            current_position_205 = (rx_message.Data[0]<<8) | rx_message.Data[1];	//数据8位，要的16位，来两个数据
						yaw_speed=(rx_message.Data[2]<<8)|rx_message.Data[3];
        }
				else if(rx_message.StdId == 0x208)           //206 是pitch
        {             
            current_position_205 = (rx_message.Data[0]<<8) | rx_message.Data[1];	
						yaw_speed=(rx_message.Data[2]<<8)|rx_message.Data[3];
        }
        else if(rx_message.StdId == 0x206)      //205 是YAW轴            
        { 
            current_position_206 = (rx_message.Data[0]<<8) | rx_message.Data[1];
						current_cm_206=(rx_message.Data[2]<<8)|rx_message.Data[3];// 201205 for pid pitch
        }
				else if(rx_message.StdId == 0x201)      //左前          
        { 
             current_cm_201 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
					t_i_1=(rx_message.Data[4]<<8) | rx_message.Data[5];
					
//					u8count1++;
//					if(u8count1 == 50){
//						u8count1 = 0 ;
//						UART_DMA_SEND(current_cm_201);
//					}
        }
				else if(rx_message.StdId == 0x202)       //右前         
        { 
             current_cm_202 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
					t_i_2=(rx_message.Data[4]<<8) | rx_message.Data[5];
				
        }
				else if(rx_message.StdId == 0x203)       //右后        
        { 
             current_cm_203 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
					t_i_3=(rx_message.Data[4]<<8) | rx_message.Data[5];
        }
				else if(rx_message.StdId == 0x204)       //左后       
        { 
             current_cm_204 = (rx_message.Data[2]<<8) | rx_message.Data[3];	
					t_i_4=(rx_message.Data[4]<<8) | rx_message.Data[5];
        }
						else if(rx_message.StdId == 0x301)      //电容          
		{ 	
			SCM_rx_message.charge_enabled_state=rx_message.Data[0] & 0x0001;
			SCM_rx_message.residue_power=rx_message.Data[1];
			SCM_rx_message.charge_power=(uint16_t)(rx_message.Data[2]<<8  | rx_message.Data[3])*25;
			SCM_rx_message.chassis_power=(int16_t)(rx_message.Data[4]<<8  | rx_message.Data[5])*25;
			SCM_rx_message.cap_vol=(int16_t)(rx_message.Data[6]<<8  | rx_message.Data[7])*1.25;
			Lost_Connection_Count=0;
		}

				CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
    }
}



void continue_value(void)   //增大值 使之连续
{
	/**********************************************/
	if(!flag_Ready)
	{
		continuous_current_position_205=current_position_205;
	}
	else
	{
		if(abs(previous_position_205-current_position_205)>6000)
		{
			if(previous_position_205<1000)
			{
				rotate_205_count-=1;
			}
			else
				rotate_205_count+=1;
		}
		continuous_current_position_205=current_position_205+rotate_205_count*8192;
		continuous_current_position_filtered_205=LPF_FirstOrder_filter(&filter_205,continuous_current_position_205);
	}
	previous_position_205=current_position_205;
	/**********************************************/
	if(!flag_Ready)
	{continuous_current_position_206=current_position_206;}
	else
	{
		if(abs(previous_position_206-current_position_206)>6000)
		{
			if(previous_position_206<1000)
			{
				rotate_206_count-=1;
			}
			else
				rotate_206_count+=1;
		}
		continuous_current_position_206=current_position_206+rotate_206_count*8192;
		continuous_current_position_filtered_206=LPF_FirstOrder_filter(&filter_206,continuous_current_position_206);
		

		
		
	}
	previous_position_206=current_position_206;
	
	/**********************************************/
	if(previous_position_207 == (uint16_t)-1)
	{
		previous_position_207 = current_position_207;
	}
	else
	{
		float delta = current_position_207 - previous_position_207;
		delta += (delta>6144)?-8192:((delta<-6144)?8192:0);
		continuous_current_position_207 += (delta / 36.f)*0.0439453125f;// /360*10*360/8192
	}
	previous_position_207=current_position_207;

	
	
}





/***************************************************************************************
 *Name     : CAN2_Cmd_Bottom
 *Function ：CAN2底盘控制程序
 *Input    ：Speed A B C D ，输出四个电机的值
 *Output   ：无 
 *Description : 控制四个底盘电机运动
****************************************************************************************/
void CAN2_Cmd_Bottom(int16_t candata1,int16_t candata2,int16_t candata3,int16_t candata4)
{	
	CanTxMsg TxMessage;
  TxMessage.StdId=0x200;	           // 标准标识符为0x200
  TxMessage.ExtId=0x00;	             // 设置扩展标示符（29位）
  TxMessage.IDE=CAN_ID_STD;					 // 使用标准帧模式
  TxMessage.RTR=CAN_RTR_DATA;				 // 发送的是数据
  TxMessage.DLC=8;							     // 数据长度为2字节
  TxMessage.Data[0]= (uint8_t)(candata1 >> 8);
  TxMessage.Data[1]= (uint8_t) candata1;	
	TxMessage.Data[2]= (uint8_t)(candata2 >> 8);
  TxMessage.Data[3]= (uint8_t) candata2;	
	TxMessage.Data[4]= (uint8_t)(candata3 >> 8);
  TxMessage.Data[5]= (uint8_t) candata3;	
	TxMessage.Data[6]= (uint8_t)(candata4 >> 8);
  TxMessage.Data[7]= (uint8_t) candata4;	
	
	CAN_Transmit(CAN2, &TxMessage);
}



/***************************************************************************************
 *Name     : CAN1_Feed_Cmd
 *Function ：CAN1拨弹电机控制
 *Input    ：无
 *Output   ：无 
 *Description : 单独控制拨弹轮电机
****************************************************************************************/
void CAN1_Feed_Cmd(int16_t l_data)
{
	 CanTxMsg tx_message;
    
    tx_message.StdId = 0x1ff;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
		tx_message.Data[0] = 0x00;
    tx_message.Data[1] = 0x00;
    tx_message.Data[2] = 0x00;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = (uint8_t)(l_data >> 8);
    tx_message.Data[5] = (uint8_t)l_data;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    
    CAN_Transmit(CAN1,&tx_message);
}



void CAN2_Cmd_Float_Clear(void)  //零飘？
{
	CanTxMsg tx_message;
  tx_message.StdId = 0x500;   //小板子的一个接收中断ID
  tx_message.IDE = CAN_Id_Standard;
  tx_message.RTR = CAN_RTR_Data;
  tx_message.DLC = 0x08;
        
	tx_message.Data[0] = 0x00;
  tx_message.Data[1] = 0x01;
  tx_message.Data[2] = 0x00;
  tx_message.Data[3] = 0x00;
  tx_message.Data[4] = 0x00;
  tx_message.Data[5] = 0x00;
  tx_message.Data[6] = 0x00;
  tx_message.Data[7] = 0x00;
    
	CAN_Transmit(CAN2,&tx_message);
}



void CAN2_Send_Clear(int16_t clear_angle)
{
	CanTxMsg tx_message;
  tx_message.StdId = 0x400;
  tx_message.IDE = CAN_Id_Standard;
  tx_message.RTR = CAN_RTR_Data;
  tx_message.DLC = 0x08;
        
	tx_message.Data[0] = (unsigned char)(clear_angle >> 8);
  tx_message.Data[1] = (unsigned char)clear_angle;
  tx_message.Data[2] = 0x00;
  tx_message.Data[3] = 0x00;
  tx_message.Data[4] = 0x00;
  tx_message.Data[5] = 0x00;
  tx_message.Data[6] = 0x00;
  tx_message.Data[7] = 0x00;
    
	CAN_Transmit(CAN2,&tx_message);
}



/***************************************************************************************
 *Name     : CAN1_Cmd_standard
 *Function ：云台电机校准
 *Input    ：无
 *Output   ：无 
 *Description : 新电机初次接入时需校准
****************************************************************************************/
void CAN1_Cmd_standard(void)
{
		CanTxMsg tx_message;
		tx_message.StdId = 0x3f0;
		tx_message.IDE = CAN_Id_Standard;
		tx_message.RTR = CAN_RTR_Data;
		tx_message.DLC = 0x08;
    
		tx_message.Data[0] = 'c';
		tx_message.Data[1] = 0x00;
    tx_message.Data[2] = 0x00;
		tx_message.Data[3] = 0x00;
    tx_message.Data[4] = 0x00;
		tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
		
		CAN_Transmit(CAN1,&tx_message);
}


/***************************************************************************************
 *Name     : CAN2_Cmd_All
 *Function ：CAN2云台电机控制程序
 *Input    ：传入pitch、yaw电机控制值
 *Output   ：无 
 *Description : 控制云台运动，包括pitch轴、yaw轴和拨弹轮
****************************************************************************************/
void CAN2_Cmd_All(int16_t current_205,int16_t current_206)
{
    CanTxMsg tx_message;
    
    tx_message.StdId = 0x1ff;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    #ifdef INFANTRY_1
		tx_message.Data[0] =	0x00;
    tx_message.Data[1] =	0x00;
    tx_message.Data[2] = (unsigned char)(current_206 >> 8);
    tx_message.Data[3] = (unsigned char) current_206;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = (unsigned char)(current_205 >> 8);
    tx_message.Data[7] = (unsigned char) current_205;
		#else
		tx_message.Data[0] = (unsigned char)(current_205 >> 8);
    tx_message.Data[1] = (unsigned char) current_205;
    tx_message.Data[2] = (unsigned char)(current_206 >> 8);
    tx_message.Data[3] = (unsigned char) current_206;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
		#endif
    
    CAN_Transmit(CAN2,&tx_message);
}



/***************************************************************************************
 *Name     : CAN1_fri_Cmd
 *Function ：CAN1摩擦轮控制
 *Input    ：
 *Output   ：无 
 *Description : 
****************************************************************************************/
void CAN1_fri_Cmd(int16_t l_data,int16_t r_data)
{
	CanTxMsg tx_message;
    
    tx_message.StdId = 0x200;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
		tx_message.Data[0] = (uint8_t)(l_data >> 8);
    tx_message.Data[1] = (uint8_t)l_data;
    tx_message.Data[2] = (uint8_t)(r_data >> 8);
    tx_message.Data[3] = (uint8_t)r_data;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    
    CAN_Transmit(CAN1,&tx_message);
}


void CAN2_Cap_Transmes(int16_t flag)//选择通信信道
{
	CanTxMsg tx_cap_message;
	u8 Powersourses_Charge=0x0001;
	u8 Powersourses_Uncharge=0x0000;
	u8 Capsourses_Charge=0x0003;
	u8 Capsourses_Uncharge=0x0002;
	
	
	if (flag==1) //电容没充满，有剩余功率
	{
  tx_cap_message.StdId = 0x300;
  tx_cap_message.IDE = CAN_Id_Standard;
  tx_cap_message.RTR = CAN_RTR_Data;
  tx_cap_message.DLC = 0x08;
    
	tx_cap_message.Data[0]= (unsigned char)(Powersourses_Charge);
  tx_cap_message.Data[1] = (unsigned char)0;
  tx_cap_message.Data[2] = (unsigned char)(Residue_Power >> 8);
  tx_cap_message.Data[3] = (unsigned char)Residue_Power;
  tx_cap_message.Data[4] = (unsigned char)0;
  tx_cap_message.Data[5] = (unsigned char)0;
  tx_cap_message.Data[6] = (unsigned char)0;
	tx_cap_message.Data[7] = (unsigned char)0;
    }
	else if(flag==0)  //用电池供电
	{
  tx_cap_message.StdId = 0x300;
  tx_cap_message.IDE = CAN_Id_Standard;
  tx_cap_message.RTR = CAN_RTR_Data;
  tx_cap_message.DLC = 0x08;
    
	tx_cap_message.Data[0]= (unsigned char)(Powersourses_Uncharge);
  tx_cap_message.Data[1] = (unsigned char)0;
  tx_cap_message.Data[2] = (unsigned char)0;
  tx_cap_message.Data[3] = (unsigned char)0;
  tx_cap_message.Data[4] = (unsigned char)0;
  tx_cap_message.Data[5] = (unsigned char)0;
  tx_cap_message.Data[6] = (unsigned char)0;
	tx_cap_message.Data[7] = (unsigned char)0;
    }	
	else if(flag==2)  //用电容供电,电池同时供电
	{
  tx_cap_message.StdId = 0x300;
  tx_cap_message.IDE = CAN_Id_Standard;
  tx_cap_message.RTR = CAN_RTR_Data;
  tx_cap_message.DLC = 0x08;
    
	tx_cap_message.Data[0]= (unsigned char)(Capsourses_Charge);
  tx_cap_message.Data[1] = (unsigned char)0;
  tx_cap_message.Data[2] = (unsigned char)(Residue_Power>>8);
  tx_cap_message.Data[3] = (unsigned char)(Residue_Power);
  tx_cap_message.Data[4] = (unsigned char)0;
  tx_cap_message.Data[5] = (unsigned char)0;
  tx_cap_message.Data[6] = (unsigned char)0;
	tx_cap_message.Data[7] = (unsigned char)0;
    }
	else if(flag==3)  //用电容供电
	{
  tx_cap_message.StdId = 0x300;
  tx_cap_message.IDE = CAN_Id_Standard;
  tx_cap_message.RTR = CAN_RTR_Data;
  tx_cap_message.DLC = 0x08;
    
	tx_cap_message.Data[0]= (unsigned char)(Capsourses_Uncharge);
  tx_cap_message.Data[1] = (unsigned char)0;
  tx_cap_message.Data[2] = (unsigned char)0;
  tx_cap_message.Data[3] = (unsigned char)0;
  tx_cap_message.Data[4] = (unsigned char)0;
  tx_cap_message.Data[5] = (unsigned char)0;
  tx_cap_message.Data[6] = (unsigned char)0;
	tx_cap_message.Data[7] = (unsigned char)0;
    }
	
	CAN_Transmit(CAN2,&tx_cap_message);
	
}





 void Speed_Cmd()
{
	char msgl[64] = "";
	sprintf(msgl, "%d", current_fric_s_201);
	int i = 0;
	while(i <= strlen(msgl)) {
		while( (USART_GetFlagStatus(USART2,USART_FLAG_TC)) !=SET);
		USART2->DR =msgl [i];
	}

	
	char msgr[64] = "";
	sprintf(msgr, "%d\n", current_fric_s_202);
	int j = 0;
	while(j <= strlen(msgl)) {
		while( (USART_GetFlagStatus(USART2,USART_FLAG_TC)) !=SET);
		USART2->DR =msgl [j];
	}
		
}



