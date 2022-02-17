/*
 * porttcp_m.c
 *
 *  Created on: Dec 14, 2021
 *      Author: tu.lb174310
 */

#include <stdio.h>
#include <string.h>

#include "e_port.h"
#include "main.h"

/* ----------------------- RTOS includes ------------------------------------*/
#include "cmsis_os2.h"
#include "semphr.h"
#include "task.h"
#include "portmacro.h"

/* ----------------------- lwIP includes ------------------------------------*/

#include "lwip/api.h"
#include "err.h"
#include "ip4_addr.h"
#include "netif.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbtcp.h"
#include "mb_m.h"

#if MB_MASTER_TCP_ENABLED

#define TCP_MAX_NUM_SERVER			1
#define NETCONN_MB_TASK_PRIO		osPriorityNormal //it have to higher the priority of "monitor task"
#define NETCONN_MB_TASK_STACK		500U

/******************************Debug variable******************************/
static uint32_t debug_ResponFrame = 0;
static uint32_t debug_countsend = 0;
TaskHandle_t netconnMBMaster_handle = NULL;
extern eMBException eExtTCPMaster;

static eMBErrorCode prvxMBMasterTCPSendMBAP( struct netconn *newconn, const UCHAR * pucMBTCPFrame, USHORT usTCPLength );
static void prvvMBMasterNetconnTask(void *p);
static void prvvMBMasterTCPCloseConn(struct netconn ** netconn, UCHAR add);



/* -----------------------DEFINE SERVER ---------------------------------*/
uint8_t SERVER_IP[4];
#define SERVER_PORT	502 //server listen port
#define TIMEOUT_RESPONSE	1000U	//ms


/* ----------------------- function implementation -----------------------------*/
BOOL
xMBMasterTCPPortInit( USHORT usTCPPort ){

	BOOL status = TRUE;
	BaseType_t sta;

	xMBMasterPortEventInit();
	vMBMasterOsResInit();

	sta = xTaskCreate(	prvvMBMasterNetconnTask, "netconn MB Master" , NETCONN_MB_TASK_STACK, NULL,
						NETCONN_MB_TASK_PRIO, &netconnMBMaster_handle	);
	configASSERT(sta == pdPASS);

  return status;
}

static void prvvMBMasterNetconnTask(void *p)
{
	  err_enum_t err;
	  struct netconn *conn[TCP_MAX_NUM_SERVER] = {NULL};
	  struct netbuf *inbuf;
	  UCHAR *ucMBAPFrame;
	  USHORT usLength;
	  eMBMasterEventType  eEvent;
	  eMBErrorCode MB_err = MB_ENOERR;
	  uint8_t count_timeout = 0;
	  ip_addr_t server_addr[TCP_MAX_NUM_SERVER]; //server address
	  UCHAR DestAddress,DestAddMinus;

	  SERVER_IP[0] = 192;
	  SERVER_IP[1] = 168;
	  SERVER_IP[2] = 0;
	  SERVER_IP[3] = 2;
      IP4_ADDR(&server_addr[0], SERVER_IP[0], SERVER_IP[1], SERVER_IP[2], SERVER_IP[3]); //server ip

	  while(1)
	  {
		  if(xMBMasterPortEventGet(&eEvent) == TRUE)
		  {
			  HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
			  //get destination address
			  DestAddress  = ucMBMasterGetDestAddress();
			  DestAddMinus = DestAddress - 1;

			  if(eEvent == EV_MASTER_FRAME_SENT)
			  {
				  if(conn[DestAddMinus] == NULL)
				  {
					  conn[DestAddMinus] = netconn_new(NETCONN_TCP); //new tcp netconn
					  if (conn[DestAddMinus] != NULL)
					  {
					      err = netconn_connect(  conn[DestAddMinus],
					    		  	  	  	  	  &server_addr[DestAddMinus],
												  SERVER_PORT ); //connect to the server

					      if (err != ERR_OK)
					      {
					    	  prvvMBMasterTCPCloseConn(conn, DestAddMinus);

					    	  xMBMasterPortEventPost(EV_MASTER_ERROR_CONNECTION);

					    	  vMBMasterRunResRelease();
					          continue;
					      }

					  }
				  } /*(conn[DestAddMinus] == NULL)*/

				  /*
				   * brief: Send request and set TIMEOUT for waiting response frame
				   * 		-if SendRequest error => close and reconnect the current connection
				   * 			and notify to User_App 'EV_MASTER_ERROR_SEND_DATA'
				   * 		-if SendRequest ok then Receive timeout cause by some reason
				   * 			-> send Request again (until count = 2)
				   */
				  vMBMasterGetMBAPSndBuf(&ucMBAPFrame, &usLength);

				  while(count_timeout < 2)
				  {
						MB_err = prvxMBMasterTCPSendMBAP( conn[DestAddMinus], ucMBAPFrame, usLength);
						if( MB_err != MB_ENOERR)
						{
							break;
						}

						netconn_set_recvtimeout(conn[DestAddMinus], TIMEOUT_RESPONSE);
						err = netconn_recv(conn[DestAddMinus], &inbuf);
						if(err == ERR_TIMEOUT)
						{

							count_timeout++;
						}
						else
						{
							err = netconn_err(conn[DestAddMinus]);
							break;

						}
				  }	/*while(count_timeout < 2)*/

				  if(MB_err == MB_ESENDDATA)
				  {
					  //should I close the netconn
					  prvvMBMasterTCPCloseConn(conn, DestAddMinus);
					  vMBMasterRunResRelease();
					  xMBMasterPortEventPost(EV_MASTER_ERROR_SEND_DATA);
					  continue;	//while(1)
				  }

				  if(err != ERR_TIMEOUT )
				  {
					  if(err != ERR_OK)	//it may be {ERR_ABRT, ERR_RST,   ERR_CLSD, ERR_CONN...}
					  {
						  prvvMBMasterTCPCloseConn(conn, DestAddMinus);
						  vMBMasterRunResRelease();
						  xMBMasterPortEventPost(EV_MASTER_ERROR_CONNECTION);
						  continue;	//while(1)
					  }

				  }

				  if(count_timeout >= 2)
				  {
					vMBMasterRunResRelease();
					prvvMBMasterTCPCloseConn(conn, DestAddress);
				  	xMBMasterPortEventPost(EV_MASTER_ERROR_RESPOND_TIMEOUT);
				  	continue; //while(1)
				  }
				  count_timeout = 0;

				  //vMBMasterGetMBAPRcvBuf(&ucMBAPFrame, &usLength);
				  netbuf_data(inbuf, (void**)&ucMBAPFrame, &usLength);
				  netbuf_delete(inbuf); // delete the buffer always

				  debug_ResponFrame++;
				  MB_err = eMBMasterCheckMBAPRcvBuf(ucMBAPFrame,  usLength);

				  /*
				   * handle exception
				   * These handles do not Close and Delete the current connection
				   */

				  if(MB_err != MB_ENOERR)	//error IO
				  {
				  	xMBMasterPortEventPost(EV_MASTER_ERROR_RECEIVE_DATA);
				  }
				  else
				  {
					if(eExtTCPMaster != MB_EX_NONE)
					{
						xMBMasterPortEventPost(EV_MASTER_ERROR_EXECUTE_FUNCTION);
					}
					else
					{
						xMBMasterPortEventPost(EV_MASTER_PROCESS_SUCESS);
					}

				  }
				  vMBMasterRunResRelease();	//semaphore should be alternated by Mutex

			  }
			  else if(eEvent == EV_MASTER_CLOSE_CONNECTION) //todo : add event wait
			  {
				  /*there is an API that called by application user task, that post EV_MASTER_CLOSE_CONNECTION
				  	  to this task 'prvvMBMasterNetconnTask'
				  */
				  if(conn[DestAddMinus] != NULL)
					  prvvMBMasterTCPCloseConn(conn, DestAddMinus);

				  vMBMasterRunResRelease();

			  }

		  }/* xMBMasterPortEventGet(&eEvent) == TRUE */

	}//while(1)

}

static void prvvMBMasterTCPCloseConn(struct netconn ** netconn, UCHAR add){
			netconn_close(netconn[add]);
			netconn_delete(netconn[add]);//what if I do not use this API??? todo
			netconn[add] = NULL;
}

static eMBErrorCode prvxMBMasterTCPSendMBAP( struct netconn *newconn, const UCHAR * pucMBTCPFrame, USHORT usTCPLength )	//input is MBAP frame
{
	eMBErrorCode   bFrameSent = MB_ENOERR;
	size_t uBytesWritten = 0;
	err_t err = ERR_OK;

	/*
	 * NOTE:when we send multiple request at a time, it should insert timeout instruction
	 * todo
	 *
	 */
	//netconn_set_sendtimeout(newconn, u16Timeoutms);
	err = netconn_write_partly(newconn, pucMBTCPFrame, (size_t)usTCPLength, NETCONN_COPY, &uBytesWritten);
	if(err != ERR_OK)
	{
		bFrameSent = MB_ESENDDATA;
	}
	else
	{
		debug_countsend++;
	}

  return bFrameSent;
}





#endif
