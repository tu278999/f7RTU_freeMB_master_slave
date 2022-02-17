/*
 * mbtask.c
 *
 *  Created on: Oct 15, 2021
 *      Author: tu.lb174310
 */
#include "mbtask_user.h"

#include "main.h"

/*freertos include */
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "e_port.h"
#include "mb.h"
#include "mb_m.h"
#include "user_mb_app.h"
#include "mbtcp.h"

extern USHORT   usSRegInStart;
extern USHORT   usSRegInBuf[S_REG_INPUT_NREGS];

extern USHORT   usSRegHoldStart;
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS];

extern USHORT   usSCoilStart;
extern UCHAR    ucSCoilBuf[S_COIL_NCOILS/8];

extern USHORT   usSDiscInStart;
extern UCHAR    ucSDiscInBuf[S_DISCRETE_INPUT_NDISCRETES/8];


TaskHandle_t MasterMonitorHandle = NULL;
TaskHandle_t MasterPollHandle = NULL;
TaskHandle_t SlavePollHandle = NULL;
TaskHandle_t LedTaskHandle = NULL;

void vInitMBTask(void){
	BaseType_t status;

#if MB_MASTER_RTU_ENABLED || MB_MASTER_TCP_ENABLED
	status = xTaskCreate(mastermonitor_task, "master monitor task", 600, NULL, osPriorityNormal, &MasterMonitorHandle);
	configASSERT(status == pdPASS);
#endif

#if MB_MASTER_RTU_ENABLED
	status = xTaskCreate(masterpoll_task, "master poll task", 600, NULL, osPriorityNormal1, &MasterPollHandle);
	configASSERT(status == pdPASS);
#endif

#if MB_SLAVE_RTU_ENABLED
	status = xTaskCreate(slavepoll_task, "slave poll task", 600, NULL, osPriorityNormal, &SlavePollHandle);
	configASSERT(status == pdPASS);
#endif

	status = xTaskCreate(led_task, "LED task", 200, NULL, osPriorityNormal, &LedTaskHandle);
	configASSERT(status == pdPASS);
}

USHORT usModbusUserData[10];
UCHAR  ucModbusUserData[10];

void mastermonitor_task(void*p){

#if MB_MASTER_TCP_ENABLED
	eMBMasterTCPDoInit(502);
#endif
    eMBMasterReqErrCode    errorCode = MB_MRE_NO_ERR;
    usModbusUserData[0] = 0x1000;
    usModbusUserData[1] = 0x1111;
    usModbusUserData[2] = 0x2222;
    usModbusUserData[3] = 0x3333;
    usModbusUserData[4] = 0x4444;
	while(1)
	{
				//GHI 1 vào bit thứ 8 của slave modbus (function code = 05)
		        errorCode = eMBMasterReqWriteCoil(1,8,0xFF00, portMAX_DELAY);

		        //đọc 8 bit đầu tiên của slave modbus	(function code = 02)
		        errorCode = eMBMasterReqReadDiscreteInputs(1, 0, 8, portMAX_DELAY);

		        //Đọc 16 thanh ghi INPUT đầu tiên của slave modbus  (function code = 04)
		     	errorCode = eMBMasterReqReadInputRegister(1,0,16,portMAX_DELAY);

		     	//ghi vào 4 thanh ghi đầu tiên của slave modbus  (function code = 10)
		     	errorCode = eMBMasterReqWriteMultipleHoldingRegister(1,0,2,usModbusUserData,portMAX_DELAY);

		     	vTaskDelay(500);
        if (errorCode != MB_MRE_NO_ERR) {
        	/*
        	 * if it have any error, we can do somthing here...
        	 */
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

#define PORT_MODBUS_RTU		0


void slavepoll_task(void*p){

	  usSRegInBuf[0] 	= 0x11;
	  usSRegInBuf[1] 	= 0x22;
	  usSRegInBuf[2] 	= 0x33;
	  usSRegInBuf[3] 	= 0x44;
	  usSRegHoldBuf[0] 	= 0x1111;
	  usSRegHoldBuf[1] 	= 0x2222;
	  ucSDiscInBuf[0] 	= 0xAA;
	  ucSCoilBuf[0] 	= 0xf4;

	eMBInit(MB_RTU, 1, PORT_MODBUS_RTU, 115200, MB_PAR_NONE);
	eMBEnable();
	HAL_Delay(2);

	while(1){

		eMBPoll();

	}

}

void led_task(void*p){

	while(1){
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		vTaskDelay(500);
	}

}
