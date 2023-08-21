/*
 * @file		WatchDog.c/h
 * @brief	一个简易版本的看门狗程序
 * @history
 * 版本			作者			编写日期
 * v1.0.0		郑煜壅		2023/4/1
 *
 */

#include "main.h"
#include "WatchDog.h"
#include "pid.h"
WatchDog_t remote_WatchDog;
WatchDog_t referee_WatchDog;
WatchDog_t motor_WatchDog[8];
uint8_t Motor_Status,Shooter_Status_last;
/*
 * @brief	喂狗程序代码，放在需要进行检测的程序段中
 * @param	对应的看门狗指针（由用户自定义）
 * @retval	无
 */
void feedDog(WatchDog_t* hdog)
{
//	hdog->status = 1;
	hdog->last_time = uwTick;
}
/*
 * @brief	看门狗状态更新，放在任意能够稳定运行的程序段中
 * @param	对应的看门狗指针（由用户自定义）
 * @retval	无
 */
void Dog_Status_update(WatchDog_t* hdog)
{
	if(uwTick - hdog->last_time > OFFLINE_TIME)
		hdog->status = 0;
	else hdog->status = 1;
}

void WatchDogAllUpdate()
{
	  Shooter_Status_last = motor_WatchDog[Motor_AmmoFeed_ID].status;

	  Motor_Status = 1;
	  Dog_Status_update(&remote_WatchDog);//remote WatchDog update
	  for(uint8_t i=1;i<7;i++)
	  {
		Dog_Status_update(&motor_WatchDog[i]);//motor WatchDog update
		if(!motor_WatchDog[i].status)
			Motor_Status = 0;
	  }

	Dog_Status_update(&motor_WatchDog[7]);
}

void OfflineProtection()
{
	if((!remote_WatchDog.status) )
	{
		for(int i=0;i<8;i++)
		{
			PID_Clear(&PID_Motor_Angle[i]);
			PID_Clear(&PID_Motor_Speed[i]);
		}
	}
	else if(PID_Motor_Speed[1].Kp == 0)//有至少一个控制器在线时重新初始化
		PID_Init();
	if(!motor_WatchDog[7].status)
	{
		PID_Clear(&PID_Motor_Angle[7]);
		PID_Clear(&PID_Motor_Speed[7]);
	}
	else if(PID_Motor_Speed[7].Kp == 0)//有至少一个控制器在线时重新初始化
	{
		PID_Motor_Speed[7].Kp = PID_SpeedCtrl_Config[7][0];
		PID_Motor_Speed[7].Ki = PID_SpeedCtrl_Config[7][1];
		PID_Motor_Speed[7].Kd = PID_SpeedCtrl_Config[7][2];
		PID_Motor_Speed[7].Output_Max = PID_SpeedCtrl_Config[7][3];
		PID_Motor_Speed[7].Err_sum_Max = PID_SpeedCtrl_Config[7][4];
		PID_Motor_Speed[7].PID_Type = Speed;
	}
}
