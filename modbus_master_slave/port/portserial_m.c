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
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "e_port.h"
#include "mb.h"
#include "mbport.h"
#include "mbrtu.h"
#include"main.h"

#if MB_MASTER_RTU_ENABLED > 0
/* ----------------------- define---------------------------------*/

/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Callback for the protocol stack ------------------*/
extern pxMBFrameCB pxMBMasterFrameCBByteReceived;
extern pxMBFrameCB pxMBMasterFrameCBTransmitterEmpty;

/* ----------------------- static functions ---------------------------------*/
#if MASTER_USE_UART2
 void USART2_IRQHandler(void);
#else
 void USART3_IRQHandler(void);
#endif

/* ----------------------- extern variable ---------------------------------*/


/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
							  eMBParity eParity)
{
	  /*
	  Do nothing, Initialization is handled by MX_USART2_UART_Init() or USART3
	  Fixed port, baudrate, databit and parity
	  */

		/*
		 * Please note: when user uses EVEN or ODD parity, the Data bit value chose
		 * in CubeMx initialize must be 9 bit
		 */

	return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{

	  /* If xRXEnable enable serial receive interrupts. If xTxENable enable
	  * transmitter empty interrupts.
	  */
	  if (xRxEnable) {
	    __HAL_UART_ENABLE_IT(&huartmaster, UART_IT_RXNE);
//	    HAL_GPIO_WritePin(RE_GPIO_Port, RE_Pin, GPIO_PIN_RESET);// PC10 <=> RE..Receiver Output Enable (Low to enable)
	  } else {
	    __HAL_UART_DISABLE_IT(&huartmaster, UART_IT_RXNE);
//	    HAL_GPIO_WritePin(RE_GPIO_Port, RE_Pin, GPIO_PIN_SET);
	  }

	  if (xTxEnable) {
	    __HAL_UART_ENABLE_IT(&huartmaster, UART_IT_TXE);
//	    HAL_GPIO_WritePin(DE_GPIO_Port, DE_Pin, GPIO_PIN_SET);// PC11 <=> DE……….Driver Output Enable (high to enable)
	  } else {
	    __HAL_UART_DISABLE_IT(&huartmaster, UART_IT_TXE);
//	    HAL_GPIO_WritePin(DE_GPIO_Port, DE_Pin, GPIO_PIN_RESET);
	  }

}

void vMBMasterPortClose(void)
{
	//..
	return;
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
	  /* Put a byte in the UARTs transmit buffer. This function is called
	  * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
	  * called. */
	huartmaster.Instance->TDR = (uint8_t)(ucByte & 0xFFU);
	return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{

	  /* Return the byte in the UARTs receive buffer. This function is called
	  *  by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
	  */

	*pucByte = (uint8_t)(huartmaster.Instance->RDR & (uint8_t)0x00FF);

	  return TRUE;
}

/**
  * @brief
  */
#if MASTER_USE_UART2
void USART2_IRQHandler(void)
#else
void USART3_IRQHandler(void)
#endif
{

	uint32_t isrflags   = READ_REG(huartmaster.Instance->ISR);
	uint32_t cr1its     = READ_REG(huartmaster.Instance->CR1);

    if ( ((isrflags & USART_ISR_RXNE) != 0U)
         && ((cr1its & USART_CR1_RXNEIE) != 0U) )
    {

		//xMBMasterRTUReceiveFSM();
		pxMBMasterFrameCBByteReceived();

		SET_BIT((&huartmaster)->Instance->RQR,  USART_RQR_RXFRQ );

		return;

	}

	if((__HAL_UART_GET_FLAG(&huartmaster, UART_FLAG_TXE) != RESET) &&(__HAL_UART_GET_IT_SOURCE(&huartmaster, UART_IT_TXE) != RESET)) {

		//xMBMasterRTUTransmitFSM();
		pxMBMasterFrameCBTransmitterEmpty();
		return ;

	}

	HAL_UART_IRQHandler(&huartmaster);
}


#endif
