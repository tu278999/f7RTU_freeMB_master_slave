/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "main.h"

#include "timers.h"
#include "e_port.h"
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- Define ----------------------------------------*/

#define CONVERTTIMER_PERIOD		4000	//200ms
#define RESPONSETIMER_PERIOD	18000   //900ms

/* ----------------------- Variables ----------------------------------------*/

//T35 variable
extern TIM_HandleTypeDef htim7;
static uint16_t t35timer = 0;
static uint16_t mdowncounter = 0;

extern pxMBFrameCB pxMBMasterPortCBTimerExpired;

/* ----------------------- functions ---------------------------------*/
 void TIM7_IRQHandler(void);

/* ----------------------- Start implementation -----------------------------*/

BOOL xMBMasterPortTimersInit(USHORT usTim7Timerout50us)	//each entering ISRTIM7 is 50us
{
	  TIM_MasterConfigTypeDef sMasterConfig;

	  htim7.Instance = TIM7;
	  htim7.Init.Prescaler = (2 * HAL_RCC_GetPCLK1Freq() / 1000000) - 1;
	  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim7.Init.Period = 50 - 1;

	  t35timer = usTim7Timerout50us;	//1.75 ms for t35 timeout
	  	  	  	  	  	  	  	  	  	//user should set t35timer = 36 <=> 1800 us

	  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
	  {
	    return FALSE;
	  }

	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
	  {
	    return FALSE;
	  }

	  return TRUE;
}

void vMBMasterPortTimersT35Enable(void)
{
	  /* Enable the timer with the t35timer passed to xMBMasterPortTimersInit( ) */
	  mdowncounter = t35timer;
	  HAL_TIM_Base_Start_IT(&htim7);

}

void vMBMasterPortTimersConvertDelayEnable(void)	//be called from UART_ISR
{
	mdowncounter = CONVERTTIMER_PERIOD;		//200ms
	HAL_TIM_Base_Start_IT(&htim7);

}

void vMBMasterPortTimersRespondTimeoutEnable(void)
{
	mdowncounter = RESPONSETIMER_PERIOD;	//900ms
	HAL_TIM_Base_Start_IT(&htim7);
}

void vMBMasterPortTimersDisable(void)
{
	  /* Disable any pending timers. */
	  HAL_TIM_Base_Stop_IT(&htim7);
}

/**
  * @brief This function handles TIM7 global interrupt.
  */

 void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
	if(__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE) != RESET
	   && __HAL_TIM_GET_IT_SOURCE(&htim7, TIM_IT_UPDATE) !=RESET)
	{
		__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
		if (!--mdowncounter)
		{
			pxMBMasterPortCBTimerExpired();	// it is "xMBMasterRTUTimerExpired()"
		}
	}
  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

#endif
