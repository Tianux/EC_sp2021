#include "sp_conf.h"

#include <stdio.h>
#include <string.h>
#include "define.h"
#include "frame.h"
#include "crc_table.h"
#include "sp_imu.h"
#include "gimbal.h"

//created by sui qi
//modified by twb 2017.7.18 add send module
//���ļ�����ͨ���ܳ������Ƶİ������Ӿ�ͨ��

frame sendtoCom_frame;//���ݰ�
u8 sendbuffer[100];

/***************************************************************************************
 *Name     : sumCheck
 *Function �����У�麯��
 *Input    ��buff����У�����ݰ�����len��У�����ݳ��ȣ�
 *Output   ��u8��У������
 *Description :���У�飨δʹ�ã�
****************************************************************************************/
u8 sumCheck(u8 *buff,u16 len)
{
    u16 i=0;
    u32 sum=0;
    for(i=0;i<len;i++)
        sum+=buff[i];
    return (u8)sum&0x000000ff;
}
/***************************************************************************************
 *Name     : crc8Check
 *Function ��8λCRCУ�麯��
 *Input    ��buff����У�����ݰ�����len��У�����ݳ��ȣ�
 *Output   ��u8��У������
 *Description :8λCRCУ�飬����У����
****************************************************************************************/
u8 crc8Check(u8 *buff,u16 len)
{
    u8 ucIndex,ucCRC8=(u8)CRC8_INIT;
    while (len--)
    {
        ucIndex = ucCRC8^(*buff++);
        ucCRC8 = CRC8_Table[ucIndex];
    }
    return(ucCRC8);
}
/***************************************************************************************
 *Name     : crc16Check
 *Function ��16λCRCУ�麯��
 *Input    ��buff����У�����ݰ�����len��У�����ݳ��ȣ�
 *Output   ��u8��У������
 *Description :16λCRCУ�飬����У����
****************************************************************************************/
u16 crc16Check(u8 *buff,u32 len)
{
    u8 chData;
    u16 wCRC=(u16)CRC16_INIT;
    if(buff == NULL)
    {
        return 0xFFFF;
    }
    while(len--)
    {
        chData = *buff++;
        (wCRC) = ((u16)(wCRC) >> 8) ^ CRC16_Table[((u16)(wCRC) ^ (u16)(chData)) &0x00ff];
    }
    return wCRC;
}
/***************************************************************************************
 *Name     : unpackFrame
 *Function ���������
 *Input    ��buff���������ݰ�����len�����ݳ��ȣ���fram��Ҫ������ݵĽṹ�壩
 *Output   ��int��0Ϊ�ɹ���
 *Description �����ͬʱ����8λCRCУ��
****************************************************************************************/
int unpackFrame(u8 *buff, u16 len, frame *fram)
{    
		if(len>=sizeof(frame)) {
				memcpy(fram, (buff+(len-sizeof(frame))), sizeof(frame));
				if(fram->crc8check == crc8Check((u8*)fram, sizeof(frame)-2)) {
						return 0;
				}
		}
		return -1;
	
//    u16 i=0;
//    while(buff[i]!=0xff||buff[i+1]!=0xff)    
//    {
//        i++;
//        if(i>44)
//            break;
//    }    
//    if(i == sizeof(frame)) {
//        if(buff[i]!=0xff||buff[0]!=0xff)
//            return 1;
//    }
//    if(i<23)
//    {
//				memcpy(fram,(buff+i),sizeof(frame));
//    }
//    else
//    {
//        memcpy(fram,(buff+i),46-i);
//        memcpy(((u8 *)fram)+46-i,buff,sizeof(frame)-46+i);
//    }
//    if(fram->crc8check==crc8Check((u8 *)fram,sizeof(frame)-1))
//    {
//        memset(buff,0x00,46);
//        return 0;
//    }
//    else
//        return 2;
}
/***************************************************************************************
 *Name     : packFrame
 *Function ���������
 *Input    ��buff�����ݰ�����fram��Ҫ�ŵ����ݰ���Ľṹ�壩
 *Output   ����
 *Description �����ṹ���ڵ�����װ�����ݰ���������֡βУ��
****************************************************************************************/
void packFrame(u8 *buff,frame *fram)
{    
    memcpy(buff,fram,sizeof(frame));
    buff[sizeof(frame)-2] = crc8Check((u8 *)fram,sizeof(frame)-2);
		buff[sizeof(frame)-1] = 0xf2;
		buff[sizeof(frame)] = 0x00;
}
/***************************************************************************************
 *Name     : packFrame1
 *Function ���������
 *Input    ��buff�����ݰ�����fram��Ҫ�ŵ����ݰ���Ľṹ�壩
 *Output   ����
 *Description �����ṹ���ڵ�����װ�����ݰ���ʡ����ǰ6�ֽڣ���������֡βУ��
****************************************************************************************/
void packFrame1(u8 *buff,frame *fram)
{    
    memcpy(buff+6,((u8*)fram)+6,8);//ֻ�����ڴ��������ƫ��ֵ
    buff[sizeof(frame)-1] = crc8Check((u8 *)fram,sizeof(frame)-1);
}

/***************************************************************************************
 *Name     : sendtoComputer
 *Function ����׼�������Ӿ������ݴ������ݰ�
 *Input    ����
 *Output   ����
 *Description �����ṹ���ڵ�����װ�����ݰ�
****************************************************************************************/
void sendtoComputer(void)
{
#if defined(BOARD_MODE) && (BOARD_MODE == 1 || BOARD_MODE == 2)	
    sendtoCom_frame.yaw = spGIMBAL_Controller._target.gimbal_yaw_motor->state.angle*27.f/39.f;
#else
		sendtoCom_frame.yaw = spGIMBAL_Controller._target.gimbal_yaw_motor->state.angle ;
#endif
		sendtoCom_frame.timestamp ++;
		sendtoCom_frame.pitch = spGIMBAL_Controller._target.gimbal_pitch_motor->state.angle;
#if defined(BOARD_MODE) && (BOARD_MODE == 2) && defined(USING_IMU)
		sendtoCom_frame.pitch = IMU_Controllers.imu_state.kalman.euler.pitch;
		sendtoCom_frame.yaw = IMU_Controllers.imu_state.kalman.euler.yaw;
#endif
    packFrame(sendbuffer, &sendtoCom_frame);//����ÿ�ΰ����ڴ�ʱ��
#ifdef USING_USB
		USB_SendData(sendbuffer,sizeof(sendtoCom_frame ) + 1);
#else
    spDMA.controller.start(spDMA_USART2_tx_stream, (uint32_t)sendbuffer, (uint32_t)&USART2->DR, sizeof(sendtoCom_frame));
#endif
//    DMA_Cmd(DMA1_Stream6,ENABLE);
//    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
}
/***************************************************************************************
 *Name     : sendtoComputerInit
 *Function �������ʼ������
 *Input    ����
 *Output   ����
 *Description ���״ν�����װ�����ݰ�
****************************************************************************************/
void sendtoComputerInit(void)
{
    sendtoCom_frame.head = 0xf1;
    sendtoCom_frame.timestamp = 0x00;
    sendtoCom_frame.yaw = spGIMBAL_Controller._target.gimbal_yaw_motor->state.angle*0.0439f;
    sendtoCom_frame.pitch = spGIMBAL_Controller._target.gimbal_pitch_motor->state.angle*0.0439f;
//    sendtoCom_frame.yaw = (current_position_205-MIDDLE_YAW)*0.0439f;
//    sendtoCom_frame.pitch = (current_position_206-MIDDLE_PITCH)*0.0439f;
		sendtoCom_frame.end = 0xf2;
    packFrame(sendbuffer,&sendtoCom_frame);

}
/***************************************************************************************
 *Name     : ChangeToAuto
 *Function ����������ģʽ����
 *Input    ����
 *Output   ����
 *Description �����Ӿ����Ϳ�������ģʽ�����ͨ��CAN2�������ư壩
****************************************************************************************/
void ChangeToAuto(void)//ѡ�������ģʽ
{
    
    #ifdef little_board
    
        CanTxMsg TxMessage;
        
        TxMessage.StdId=0x212;               // ��׼��ʶ��Ϊ0x212
        TxMessage.ExtId=0x00;                 // ������չ��ʾ����29λ���������ã�
        TxMessage.IDE=CAN_ID_STD;                     // ʹ�ñ�׼֡ģʽ
        TxMessage.RTR=CAN_RTR_DATA;                 // ���͵�������
        TxMessage.DLC=8;                                 // ���ݳ���Ϊ2�ֽ�
        TxMessage.Data[0]= 0x01;
        TxMessage.Data[1]= 0x01;    
        TxMessage.Data[2]= 0x00;
        TxMessage.Data[3]= 0x00;    
        TxMessage.Data[4]= 0x00;
        TxMessage.Data[5]= 0x00;    
        TxMessage.Data[6]= 0x00;
        TxMessage.Data[7]= 0x00;    
        
        CAN_Transmit(CAN2, &TxMessage);
    
    #else
    
        u32 AutoFlag[2]={1,1};
        memcpy(sendbuffer+14,&AutoFlag,8);
        
    #endif
}
/***************************************************************************************
 *Name     : ChangeToManual
 *Function ���������ֶ�ģʽ����
 *Input    ����
 *Output   ����
 *Description �����Ӿ����Ϳ����ֶ�ģʽ�����ͨ��CAN2�������ư壩
****************************************************************************************/
void ChangeToManual(void)//ѡ����ֶ�ģʽ
{

    #ifdef little_board
    
        CanTxMsg TxMessage;
        
        TxMessage.StdId=0x212;               // ��׼��ʶ��Ϊ0x212
        TxMessage.ExtId=0x00;                 // ������չ��ʾ����29λ��
        TxMessage.IDE=CAN_ID_STD;                     // ʹ�ñ�׼֡ģʽ
        TxMessage.RTR=CAN_RTR_DATA;                 // ���͵�������
        TxMessage.DLC=8;                                 // ���ݳ���Ϊ2�ֽ�
        TxMessage.Data[0]= 0x00;
        TxMessage.Data[1]= 0x00;    
        TxMessage.Data[2]= 0x00;
        TxMessage.Data[3]= 0x00;    
        TxMessage.Data[4]= 0x00;
        TxMessage.Data[5]= 0x00;    
        TxMessage.Data[6]= 0x00;
        TxMessage.Data[7]= 0x00;    
        
        CAN_Transmit(CAN2, &TxMessage);
    
    #else
    
        u32 AutoFlag=0;
        memcpy(sendbuffer+14,&AutoFlag,4);
        
    #endif
    
}
/***************************************************************************************
 *Name     : ChangeToPower
 *Function ������С��ģʽ����
 *Input    ����
 *Output   ����
 *Description �����Ӿ����Ϳ���С��ģʽ�����ͨ��CAN2�������ư壩
****************************************************************************************/
void ChangeToPower(void)//ѡ�����������ģʽ
{

    #ifdef little_board
    
        CanTxMsg TxMessage;
        
        TxMessage.StdId=0x212;               // ��׼��ʶ��Ϊ0x212
        TxMessage.ExtId=0x00;                 // ������չ��ʾ����29λ��
        TxMessage.IDE=CAN_ID_STD;                     // ʹ�ñ�׼֡ģʽ
        TxMessage.RTR=CAN_RTR_DATA;                 // ���͵�������
        TxMessage.DLC=8;                                 // ���ݳ���Ϊ2�ֽ�
        TxMessage.Data[0]= 0x01;
        TxMessage.Data[1]= 0x03;    
        TxMessage.Data[2]= 0x00;
        TxMessage.Data[3]= 0x00;    
        TxMessage.Data[4]= 0x00;
        TxMessage.Data[5]= 0x00;    
        TxMessage.Data[6]= 0x00;
        TxMessage.Data[7]= 0x00;    
        
        CAN_Transmit(CAN2, &TxMessage);
    
    #else
    
        u32 AutoFlag[2]={1,3};
        memcpy(sendbuffer+14,&AutoFlag,8);
        
    #endif
}
/***************************************************************************************
 *Name     : ChangeToBigPower
 *Function ���������ģʽ����
 *Input    ����
 *Output   ����
 *Description �����Ӿ����Ϳ������ģʽ�����ͨ��CAN2�������ư壩
****************************************************************************************/
void ChangeToBigPower(void)//ѡ�����������ģʽ
{

    #ifdef little_board
    
        CanTxMsg TxMessage;
        
        TxMessage.StdId=0x212;               // ��׼��ʶ��Ϊ0x200
        TxMessage.ExtId=0x00;                 // ������չ��ʾ����29λ��
        TxMessage.IDE=CAN_ID_STD;                     // ʹ�ñ�׼֡ģʽ
        TxMessage.RTR=CAN_RTR_DATA;                 // ���͵�������
        TxMessage.DLC=8;                                 // ���ݳ���Ϊ2�ֽ�
        TxMessage.Data[0]= 0x01;
        TxMessage.Data[1]= 0x04;    
        TxMessage.Data[2]= 0x00;
        TxMessage.Data[3]= 0x00;    
        TxMessage.Data[4]= 0x00;
        TxMessage.Data[5]= 0x00;    
        TxMessage.Data[6]= 0x00;
        TxMessage.Data[7]= 0x00;    
        
        CAN_Transmit(CAN2, &TxMessage);
    
    #else
    
        u32 AutoFlag[2]={1,4};
        memcpy(sendbuffer+14,&AutoFlag,8);
        
    #endif
}






