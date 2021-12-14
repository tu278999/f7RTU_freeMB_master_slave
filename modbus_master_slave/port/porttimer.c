/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
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
 * File: $Id: porttimer.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "main.h"
/* ----------------------- Modbus includes ----------------------------------*/


#include "e_port.h"
#include "mb.h"
#include "mbport.h"
#include "mbrtu.h"

#if MB_SLAVE_RTU_ENABLED > 0

/* ----------------------- extern functions ---------------------------------*/
extern pxMBFrameCB pxMBPortCBTimerExpired;
void TIM6_DAC_IRQHandler(void);

/* -----------------------    variables     ---------------------------------*/
extern TIM_HandleTypeDef htim6;
static uint16_t timeout = 0;
static uint16_t downcounter = 0;		//20000 = 1s for test timer
 
/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
  TIM_MasterConfigTypeDef sMasterConfig;
  
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = (2 * HAL_RCC_GetPCLK1Freq() / 1000000) - 1; //TIMx_CLK = 216Mhz, PCLK1 = 108MHz
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 50 - 1;
  
  timeout = usTim1Timerout50us;
  
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    return FALSE;
  }
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    return FALSE;
  }
  
  return TRUE;
}
 
void
vMBPortTimersEnable(  )
{
  /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
  downcounter = timeout;
  HAL_TIM_Base_Start_IT(&htim6);
}
 
void
vMBPortTimersDisable(  )
{
  /* Disable any pending timers. */
  HAL_TIM_Base_Stop_IT(&htim6);
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
	if(__HAL_TIM_GET_FLAG(&htim6, TIM_FLAG_UPDATE) != RESET
	   && __HAL_TIM_GET_IT_SOURCE(&htim6, TIM_IT_UPDATE) !=RESET)
	{
		__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
		if (!--downcounter)
		{
			pxMBPortCBTimerExpired();	// it is "xMBRTUTimerT35Expired()"
			//downcounter = 20000;
			//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			//todo return;
		}
	}
  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

#endif
