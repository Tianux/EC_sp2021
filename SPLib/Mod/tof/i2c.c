/************************************************************
 *File        :i2c
 *Author    :  @LiuZhe 937150058@qq.com
 *Version    : V0.0.2 
 *Update    : 2019.02.28  2019.04.05
 *Description:     Use I/O to simulate IIC and Test sensors such as TOF10120 GP2Y0E03 and GY56�.
 *Ps:    ʵ��IIC�ϹҶ���ӻ�
            GY56 �������
             GP2Y0E03 ����������ϵͳƫ���Ҳ��ȶ�
            TOF10120 Ӳ���ջ���δ�������
 ************************************************************/
/**
  ******************************************************************************
  * @file       sp_gpio.c
  * @author     LiuZhe 937150058@qq.com
  * @version    v0.1-beta
  * @date       2019.Mar.02
  * @brief      Use I/O to simulate IIC 
  ******************************************************************************
  * @license
    * @demo:Ŀǰ��Ҫ��ʹ�ܵ� GPIO �� PIN Ŀǰ���ļ���Ӧ��ΪPF0(SDA)--I2(A��) PF1(SCL)---I1(A��)    
  ******************************************************************************
  */
    
/* Includes ------------------------------------------------------------------*/

    
#include "i2c.h"
#include "sp_gpio.h"

#define SDA_IN_PF0()  {GPIOF->MODER&=~(3<<( 0 *2));GPIOF->MODER|=0<< 0 *2;}    //PF0����ģʽ
#define SDA_OUT_PF0() {GPIOF->MODER&=~(3<<( 0 *2));GPIOF->MODER|=1<< 0 *2;}    //PF0���ģʽ

#define SDA_IN_PE4()  {GPIOE->MODER&=~(3<<( 4 *2));GPIOE->MODER|=0<< 4 *2;}    //PE4����ģʽ
#define SDA_OUT_PE4() {GPIOE->MODER&=~(3<<( 4 *2));GPIOE->MODER|=1<< 4 *2;}    //PE4���ģʽ
//IO��������     
#define IIC_SCL_PF1    PFout(1) //SCL
#define IIC_SDA_PF0    PFout(0) //SDA     
#define READ_SDA_PF0   PFin(0)  //����SDA 

#define IIC_SCL_PE5    PEout(5) //SCL
#define IIC_SDA_PE4    PEout(4) //SDA     
#define READ_SDA_PE4   PEin(4)  //����SDA 


void IIC_Init(void)
{            
  GPIO_InitTypeDef  GPIO_InitStructure1;
	GPIO_InitTypeDef  GPIO_InitStructure2;
    
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOFʱ��

  //GPIOF0,F1��ʼ������
  GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure1.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure1.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOF, &GPIO_InitStructure1);//��ʼ��
	
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure2.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure2);//��ʼ��
	
  IIC_SCL_PF1 = 1;
  IIC_SDA_PF0 = 1;
	
	IIC_SCL_PE5 = 1;
	IIC_SDA_PE4 = 1;
}

void IIC_Start(void)
{
    SDA_OUT_PF0();     //sda�����
	  SDA_OUT_PE4();
    IIC_SDA_PF0 = 1;
		IIC_SDA_PE4 = 1;
    IIC_SCL_PF1 = 1;
	  IIC_SCL_PE5 = 1;
    delay_us(T);
    IIC_SDA_PF0 = 0;//START:when CLK is high,DATA change form high to low 
	  IIC_SDA_PE4 = 0;
    delay_us(T);
    IIC_SCL_PF1 = 0;//ǯסI2C���ߣ�׼�����ͻ��������
	  IIC_SCL_PE5 = 0;
    delay_us(T);
}      
//����IICֹͣ�ź�
void IIC_Stop(void)
{
  //  SDA_OUT();//sda�����
	  SDA_OUT_PF0();     //sda�����
	  SDA_OUT_PE4();
    IIC_SCL_PF1 = 0;
	  IIC_SCL_PE5 = 0;
//    IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    IIC_SDA_PF0 = 0;
		IIC_SDA_PE4 = 0;
    delay_us(T);
 //   IIC_SCL=1; 
	  IIC_SCL_PF1 = 1;
	  IIC_SCL_PE5 = 1;
    delay_us(T);
 //   IIC_SDA=1;//����I2C���߽����ź�
	  IIC_SDA_PF0 = 1;
		IIC_SDA_PE4 = 1;
    delay_us(T);
}
////�ȴ�Ӧ���źŵ���
////����ֵ��1������Ӧ��ʧ��
////        0������Ӧ��ɹ�
//uint8_t IIC_Wait_Ack(void)
//{

//} 
//����ACKӦ��
void IIC_Ack(void)
{
  //  IIC_SCL=0;
    IIC_SCL_PF1 = 0;
	  IIC_SCL_PE5 = 0;
	
    //SDA_OUT();
	  SDA_OUT_PF0();     //sda�����
	  SDA_OUT_PE4();
	
    //IIC_SDA=0;
    IIC_SDA_PF0 = 0;
		IIC_SDA_PE4 = 0;
	
    delay_us(T);
    //IIC_SCL=1;
    IIC_SCL_PF1 = 1;
	  IIC_SCL_PE5 = 1;	
    delay_us(T);
   // IIC_SCL=0;
    IIC_SCL_PF1 = 0;
	  IIC_SCL_PE5 = 0;
	
   // IIC_SDA=1;
	  IIC_SDA_PF0 = 1;
		IIC_SDA_PE4 = 1;
    delay_us(T);
}
//������ACKӦ��            
void IIC_NAck(void)
{
   // IIC_SCL=0;
    IIC_SCL_PF1 = 0;
	  IIC_SCL_PE5 = 0;
	  SDA_OUT_PF0();     //sda�����
	  SDA_OUT_PE4();
//    IIC_SDA=1;
		IIC_SDA_PF0 = 1;
		IIC_SDA_PE4 = 1;
    delay_us(T);
//    IIC_SCL=1;
    IIC_SCL_PF1 = 1;
	  IIC_SCL_PE5 = 1;	
    delay_us(T);
    //IIC_SCL=0;
	  IIC_SCL_PF1 = 0;
	  IIC_SCL_PE5 = 0;
	
   // IIC_SDA=1;
		IIC_SDA_PF0 = 1;
		IIC_SDA_PE4 = 1;	
    delay_us(T);
}                                          
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��              
void IIC_Send_Byte_PF(uint8_t txd)
{                        
    uint8_t t;   
	  SDA_OUT_PF0();     //sda�����
	 // SDA_OUT_PE4();
    IIC_SCL_PF1=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0x80; t!=0; t>>=1)
    {              
        IIC_SDA_PF0=(txd&0x80)>>7;  //ȥtxd�����λ ��Ϊ���λ����Ĵ���
        txd<<=1;                //txd=txd <<1
        delay_us(T);            //��TEA5767��������ʱ���Ǳ����
        IIC_SCL_PF1=1;
        delay_us(T); 
        IIC_SCL_PF1=0;
    }
    delay_us(T);
    
    IIC_SDA_PF0=1;
    SDA_IN_PF0();      //SDA����Ϊ����
    delay_us(2);
    IIC_SCL_PF1=1;
    uint8_t ret = READ_SDA_PF0;
    delay_us(T);
    IIC_SCL_PF1=0;//ʱ�����0
    delay_us(T);
    delay_us(10);
//    return ret;
}

void IIC_Send_Byte_PE(uint8_t txd)
{                        
    uint8_t t;   
  	//SDA_OUT_PF0();     //sda�����
	  SDA_OUT_PE4();
    IIC_SCL_PE5=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0x80; t!=0; t>>=1)
    {              
        IIC_SDA_PE4=(txd&0x80)>>7;  //ȥtxd�����λ ��Ϊ���λ����Ĵ���
        txd<<=1;                //txd=txd <<1
        delay_us(T);            //��TEA5767��������ʱ���Ǳ����
        IIC_SCL_PE5=1;
        delay_us(T); 
        IIC_SCL_PE5=0;
    }
    delay_us(T);
    
    IIC_SDA_PE4=1;
    SDA_IN_PE4();      //SDA����Ϊ����
    delay_us(2);
    IIC_SCL_PE5=1;
    uint8_t ret = READ_SDA_PE4;
    delay_us(T);
    IIC_SCL_PE5=0;//ʱ�����0
    delay_us(T);
    delay_us(10);
//    return ret;
} 

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte_PF(unsigned char ack)
{
    unsigned char i,receive=0;
    IIC_SCL_PF1=0;
    IIC_SDA_PF0=1;
    delay_us(T);
    SDA_IN_PF0();//SDA����Ϊ����
    for(i=0;i<8;i++)
    {
        delay_us(T);
        IIC_SCL_PF1=1;
        receive<<=1;
        if(READ_SDA_PF0) receive++;
        delay_us(T);
        IIC_SCL_PF1=0;
    }
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

uint8_t IIC_Read_Byte_PE(unsigned char ack)
{
    unsigned char i,receive=0;
    IIC_SCL_PE5=0;
    IIC_SDA_PE4=1;
    delay_us(T);
    SDA_IN_PE4();//SDA����Ϊ����
    for(i=0;i<8;i++)
    {
        delay_us(T);
        IIC_SCL_PE5=1;
        receive<<=1;
        if(READ_SDA_PE4) receive++;
        delay_us(T);
        IIC_SCL_PE5=0;
    }
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}
/**
  * @brief  tell the salve to begin to transit via IIC and the slave issues acknowledgement.
  * @param  Slave_addr: i2c addressing uses the high 7 bits 
  */
void    IIC_BeginTrasnmission_PF(uint8_t Slave_addr)
//uint8_t IIC_BeginTrasnmission(uint8_t Slave_addr)
{
    /*Master generates a start condition.----------------------------------------*/
    //if(IIC_Start()==0){IIC_Stop(); return RESET;}
    IIC_Start();
    /**
        Master places 1st byte data that are combined slave address (7bits) 
        with a read/write control bit (R/W=0) to SDA.
    */
    IIC_Send_Byte_PF((Slave_addr<<1) | 0);
    /*Slave issues acknowledgement.---------------------------------------------*/
    //if(!IIC_Wait_Ack()){IIC_Stop(); return RESET;}
//    IIC_Wait_Ack();
    //return SET;
}  

void    IIC_BeginTrasnmission_PE(uint8_t Slave_addr)
{
    IIC_Start();
    IIC_Send_Byte_PE((Slave_addr<<1) | 0);
}  

/**
  * @brief  from which register to read data
  * @param  addr: the register's address 
  */
void IIC_Write_Addr_PF(uint8_t addr)
//uint8_t IIC_Write_Addr(uint8_t addr)
{
    IIC_Send_Byte_PF(addr);    //Master places 2nd byte address data on SDA.
    //if(!IIC_Wait_Ack()){IIC_Stop(); return RESET;}//Slave issues acknowledgement.    
//    IIC_Wait_Ack();
    //return SET;
}    

void IIC_Write_Addr_PE(uint8_t addr)
{
    IIC_Send_Byte_PE(addr);    //Master places 2nd byte address data on SDA.

} 
/**
  * @brief  write data 
  * @param  databuffer: data
  */
void IIC_Write_Data_PF(uint16_t* databuffer)
{
//uint8_t IIC_Write_Data(uint16_t* databuffer)
//{
    IIC_Send_Byte_PF(*databuffer);
    //if(!IIC_Wait_Ack()){IIC_Stop(); return RESET;}
//    IIC_Wait_Ack();
//    return SET;
}

void IIC_Write_Data_PE(uint16_t* databuffer)
{
    IIC_Send_Byte_PE(*databuffer);
}

/**
  * @brief  IIC stop
  */
void IIC_endTrasnmission(void)
{
    IIC_Stop();
}

/**
  * @brief  request reading from sensor
  * @param  Slave_addr: i2c addressing uses the high 7 bits 
  * @param  cnt: the number of byte
  */
void requestFrom_PF(uint8_t Slave_addr,uint8_t cnt,uint16_t* requestdata)
{
    //if(IIC_Start()==0){IIC_Stop(); return RESET;}//Master generates a start condition.
    IIC_Start();
    IIC_Send_Byte_PF((Slave_addr<<1) | 1);
//    IIC_Wait_Ack();
    for(int i=0; i<cnt; i++)
    {
        requestdata[i] = IIC_Read_Byte_PF( (i<cnt-1)?1:0 );    //8 bits data is read from addr
        delay_us(10);
    }
    //return SET;
}

void requestFrom_PE(uint8_t Slave_addr,uint8_t cnt,uint16_t* requestdata)
{
    //if(IIC_Start()==0){IIC_Stop(); return RESET;}//Master generates a start condition.
    IIC_Start();
    IIC_Send_Byte_PE((Slave_addr<<1) | 1);
//    IIC_Wait_Ack();
    for(int i=0; i<cnt; i++)
    {
        requestdata[i] = IIC_Read_Byte_PE( (i<cnt-1)?1:0 );    //8 bits data is read from addr
        delay_us(10);
    }
    //return SET;
}

