/**
  ******************************************************************************
  * @file       sp_shoot.h
  * @author     LSS
  * @version    v0.0-alpha
  * @date       2019.Mar.12
  * @brief      shooting control
  * @usage      
  ******************************************************************************
  * @license
  *
  ******************************************************************************
  */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SP_SHOOT_H
#define __SP_SHOOT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sp_pid.h"
#include "sp_conf.h"
#include "sp_utility.h"
#include "sp_motor.h"
#include "sp_dma.h"
#include "sp_rc.h"
#include "sp_filter.h"

/** @defgroup   FrictionOpe
  * @note       Friction Operations
  * @ingroup    Shoot
  * @{
  */
	
typedef enum{
	CRUISE_MODE = 0,			  //������Ѳ��ģʽ
	ESCAPE_MODE = 1,				//����������ģʽ
	STATIC_ATTACK_MODE  = 2, //�����˾�ֹ����ģʽ�����ڲ���Ѫ�Ĺ���״̬
	DYNAMIC_ATTACK_MODE = 3,//�����˾�ֹ����ģʽ�����ڵ�Ѫ�Ĺ���״̬
  ESCAPE_ATTACK_MODE  = 4,//���������ݹ���ģʽ
	CURVE_ATTACK_MODE   = 5,
	REMOTE_MODE = 6,
	STANDBY_MODE = 7,
	
}RobotMode;

extern RobotMode robotMode;


typedef struct {
    uint8_t             changed:1;
    uint8_t             stopping:4;
    volatile float      speed[3];
    uint16_t            target;
    volatile int16_t    output[3];
    PID_Type            pid;
    volatile uint32_t   counters[3];
    uint32_t            counter;
//    uint32_t        counter;            // for test
} PWMFriction_Type;

void Friction_Init(void);
void Friction_Looper(void);
#define Friction_OFF 0
#define Friction_ON  1
#define Shoot_OFF 0
#define Shoot_ON  1

static uint16_t frictionState;
static uint16_t shootState;

void Shooting_Control_Init(void);
void Shooting_Control_Looper (void);

#ifdef __cplusplus
}
#endif
#endif /*__SP_SHOOT_H */

/************************ (C) COPYRIGHT Tongji Super Power *****END OF FILE****/
