/*
 * mbtask.c
 *
 *  Created on: Oct 15, 2021
 *      Author: tu.lb174310
 */
#include "mbtask_user.h"

#include "main.h"

/*freertos include */
#include "FreeRTOS.h"
#include "task.h"
#include "e_port.h"
#include "mb.h"
#include "mb_m.h"
#include "user_mb_app.h"


extern USHORT   usSRegInStart;
extern USHORT   usSRegInBuf[S_REG_INPUT_NREGS];

extern USHORT   usSRegHoldStart;
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS];

extern USHORT   usSCoilStart;
extern UCHAR    ucSCoilBuf[S_COIL_NCOILS/8];

extern USHORT   usSDiscInStart;
extern UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8];

USHORT usModbusUserData[10];
UCHAR  ucModbusUserData[10];

TaskHandle_t MasterMonitorHandle = NULL;
TaskHandle_t MasterPollHandle = NULL;
TaskHandle_t SlavePollHandle = NULL;
TaskHandle_t LedTaskHandle = NULL;



void vInitMBTask(void){
	BaseType_t status;

#if MB_MASTER_RTU_ENABLED
	status = xTaskCreate(mastermonitor_task, "master monitor task", 600, NULL, 2, &MasterMonitorHandle);
	configASSERT(status == pdPASS);

	status = xTaskCreate(masterpoll_task, "master poll task", 600, NULL, 3, &MasterPollHandle);
	configASSERT(status == pdPASS);
#endif

#if MB_SLAVE_RTU_ENABLED

	status = xTaskCreate(slavepoll_task, "slave poll task", 600, NULL, 2, &SlavePollHandle);
	configASSERT(status == pdPASS);

#endif

	status = xTaskCreate(led_task, "LED task", 200, NULL, 2, &LedTaskHandle);
	configASSERT(status == pdPASS);

//	vTaskStartScheduler();

}

void mastermonitor_task(void*p){

    eMBMasterReqErrCode    errorCode = MB_MRE_NO_ERR;
    uint16_t errorCount = 0;

    usModbusUserData[0] = 0x1111;
    usModbusUserData[1] = 0x2222;
    usModbusUserData[2] = 0x2333;
    usModbusUserData[3] = 0x3333;

	while(1)
	{

				//errorCode = eMBMasterReqReadDiscreteInputs(1,0,8,portMAX_DELAY);
		//      errorCode = eMBMasterReqWriteMultipleCoils(1,3,5,ucModbusUserData,portMAX_DELAY);

		        errorCode = eMBMasterReqWriteCoil(1,8,0xFF00, portMAX_DELAY);

				//errorCode = eMBMasterReqReadCoils(1,0,8,portMAX_DELAY);
		     	errorCode = eMBMasterReqReadInputRegister(1,0,8,portMAX_DELAY);

		     	//errorCode = eMBMasterReqWriteHoldingRegister(1,0,ucModbusUserData[3],portMAX_DELAY);

		     	errorCode = eMBMasterReqWriteMultipleHoldingRegister(1,0,4,usModbusUserData,portMAX_DELAY);
		     	//errorCode = eMBMasterReqReadHoldingRegister(1,3,2,portMAX_DELAY);
		//      errorCode = eMBMasterReqReadWriteMultipleHoldingRegister(1,3,2,usModbusUserData,5,2,portMAX_DELAY);


        if (errorCode != MB_MRE_NO_ERR) {
            errorCount++;
        }
	}


}

void masterpoll_task(void*p){

    eMBMasterInit(MB_RTU, 2, 115200,  MB_PAR_EVEN);
    eMBMasterEnable();
    HAL_Delay(10);// delay for startup master modbus

	while(1)
	{
		eMBMasterPoll();
	}

}

void slavepoll_task(void*p){

	  usSRegInBuf[0] = 0x11;
	  usSRegInBuf[1] = 0x22;
	  usSRegInBuf[2] = 0x33;
	  usSRegInBuf[3] = 0x44;
	  usSRegHoldBuf[0] = 0x1111;
	  usSRegHoldBuf[1] = 0x2222;
	  ucSDiscInBuf[0] = 0xAA;
	  ucSCoilBuf[0] = 0xf1;
	eMBInit(MB_RTU, 1, 9, 115200, MB_PAR_NONE);
	eMBEnable();
	HAL_Delay(2);
	while(1){
		eMBPoll();
	}

}

void led_task(void*p){

	while(1){
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		//HAL_GPIO_WritePin(GreenLed_GPIO_Port, GreenLed_Pin, GPIO_PIN_SET);
		vTaskDelay(500);
	}

}
