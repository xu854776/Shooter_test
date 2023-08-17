/*
 * friction.c
 *
 *  Created on: Aug 17, 2023
 *      Author: DELL
 */
#include "friction.h"
#include "pid.h"
#include "can.h"
int16_t Chassis_ctrl[2] = {0};
float speed_x = 0,speed_y = 0,omega = 0,speed_x_commend = 0,speed_y_commend = 0,angle_rad = 0;

void sendMessage(void)
{
  /* USER CODE BEGIN sendMessage */
  /* Infinite loop */
  for(;;)
  {
	  ChassisCtrl();
	  Can_TxData[0] = Chassis_ctrl[0]>>8;
	  Can_TxData[1] = Chassis_ctrl[0];
	  Can_TxData[2] = Chassis_ctrl[1]>>8;
	  Can_TxData[3] = Chassis_ctrl[1];
	  Can_TxData[4] = Chassis_ctrl[2]>>8;
	  Can_TxData[5] = Chassis_ctrl[2];
	  Can_TxData[6] = Chassis_ctrl[3]>>8;
	  Can_TxData[7] = Chassis_ctrl[3];
	  HAL_CAN_AddTxMessage(&hcan1,&Can_cmdHeader[Motor_RightFriction_ID],Can_TxData,(uint32_t*)CAN_TX_MAILBOX0);
  }
  /* USER CODE END sendMessage */
}


/*
 * @brief	根据设定好的目标速度进行PID运算
 * @param	无
 * @retval	向底盘电机发送的can数据
 */
void ChassisCtrl(void)
{
		for(int i=1;i<3;i++)
			{
				PID_Incr(&PID_Motor_Speed[i],Motor[i].speed,Motor[i].target_speed);
				Chassis_ctrl[i-1] += PID_Motor_Speed[i].Output;
				if(Chassis_ctrl[i-1]>16384)
					Chassis_ctrl[i-1] = 16384;
				if(Chassis_ctrl[i-1]<-16384)
					Chassis_ctrl[i-1] = -16384;
			}
}

