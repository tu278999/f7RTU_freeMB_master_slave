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


#if MB_SLAVE_TCP_ENABLED
/* ----------------------- Defines  -----------------------------------------*/
#define MB_MAX_NUM_CLIENT 	2

/* ----------------------- Static variables ---------------------------------*/

ip_addr_t ListOfClient[MB_MAX_NUM_CLIENT];

//static uint32_t countsend = 0; //for debugging

//create handle netconn task
static osThreadId_t netconnTaskHandle;
static osThreadAttr_t netconn_attributes = {
  .name = "NetconnTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

SemaphoreHandle_t mbresource = NULL;


/* ----------------------- Static functions ---------------------------------*/
static eMBErrorCode prvxMBPortServerServe(struct netconn  *newconn);
static BOOL prvxMBTCPPortSendResponse( 	struct netconn *newconn, const UCHAR *pucMBTCPFrame, USHORT usTCPLength );
static void vMBPortNetconnTask(void*p);


/* ----------------------- function implementation -----------------------------*/
BOOL
xMBTCPPortInit( USHORT usTCPPort ){

	BOOL status = TRUE;
//	BaseType_t sta;

	//initialize authorized IP client address
	//refer to: @MB_MAX_NUM_CLIENT
	IP4_ADDR(&ListOfClient[0], 192, 168, 0, 2);
	IP4_ADDR(&ListOfClient[1], 192, 168, 0, 12);


	mbresource = xSemaphoreCreateBinary();
	configASSERT(mbresource != NULL);
	xSemaphoreGive(mbresource);

	//create netconn task for receiving frame from Lwip.
	netconnTaskHandle = osThreadNew(vMBPortNetconnTask, NULL, &netconn_attributes);
	if(netconnTaskHandle == NULL)
		status = FALSE;

  return status;
}

static void vMBPortNetconnTask(void* p){

	err_t err, accept_err;
	struct netconn  *conn;
	struct netconn  *newconn;
	uint8_t check_remoteIP;
	ip_addr_t tempIP;
	u16_t tempPort;
	int i;

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);

	 if (conn!= NULL)
	 {
	    /* Bind to port 502 */
	    err = netconn_bind(conn, NULL,(u16_t)MB_TCP_DEFAULT_PORT);
	    if (err == ERR_OK)
		{
		  	 /* Put the connection into LISTEN state */
	    	/*
	    	 * It acctually listen with BackLog
	    	 * Max number of Backlog is 5
	    	 */
		  	 netconn_listen(conn);

		  	 while(1)
		  	 {
		  		 //check authority Client IP
		  		 while(check_remoteIP)
		  		 {
				  		accept_err = netconn_accept(conn, &newconn);

				        if(accept_err == ERR_OK)
				        {
				        	netconn_getaddr(newconn, &tempIP, &tempPort, 0);
				        	for( i = 0; i < MB_MAX_NUM_CLIENT; i++)
				        	{
				        		if(tempIP.addr == ListOfClient[i].addr)
				        		{
				        			check_remoteIP = 0;
				        			break;
				        		}

				        	}

			        		if(check_remoteIP)	//if Client's IP is authorized
			        		{

			        			/*todo
			        			 *
			        			 * we need to do something here
			        			 */

			        			vMBPortReleaseClient(newconn);
//			        			taskDISABLE_INTERRUPTS();
//			        			while(1);
			        		}

				        }
		  		 }
		  		 check_remoteIP = 1;
		  		  /*
		  		   * the connection is established
		  		   * Now server will wait the request from client
		  		   *
		  		   */
		          /* serve handle data received */
		        	if(prvxMBPortServerServe(newconn) == MB_ELOSTCONN)
		        	{
		        		vMBPortReleaseClient(newconn);
		        		continue;

		        	}
		  	 }

		}
	    else//error____
	    {
			taskDISABLE_INTERRUPTS();
			for( ;; );
	    }

	  }/*if (conn!= NULL)*/
}

static eMBErrorCode prvxMBPortServerServe(struct netconn  *newconn){

	eMBErrorCode    MB_err = MB_EIO;
	err_t 	recv_err;
	UCHAR   *ucMBAPbuf;
	USHORT 	usMBAPlen;
	USHORT 	usLength;
	struct netbuf *inbuf;
  /*note in zalo cloud within keyword: 'netconn lybatu' */
  //netconn_set_recvtimeout(newconn, u16Timeoutms );
	while(1){
		/*
		 * WHEN connection was accomplished, Data will be transfered in
		 * infinite loop 'while(1)'.
		 * The 'netconn_recv()' function will block to receive MPAB frame from client and so on...
		 */
		recv_err = netconn_recv(newconn, &inbuf);

	  if (recv_err == ERR_OK)
	  {
		  if (netconn_err(newconn) == ERR_OK)
		  {
			/* Read the data from the port, blocking if nothing yet there.
			We assume the request (the part we care about) is in one netbuf */
			  netbuf_data(inbuf, (void**)&ucMBAPbuf, &usMBAPlen);
			  netbuf_delete(inbuf); // delete the buffer always

			  vMBPortTakeSrcServer();


			if(usMBAPlen > MB_TCP_BUF_MAX_SIZE)
			{
				//todo ____error
				// back to netconn_recv to receive new request from client
				continue;

			}
			else
			{
				if(usMBAPlen >= MB_TCP_FUNC_OFFSET)
				{
					//get the value of TWO LENGTH BYTE in MBAP Frame
					usLength  = ucMBAPbuf[MB_TCP_LEN_OFFSET] << 8U;
					usLength |= ucMBAPbuf[MB_TCP_LEN_OFFSET + 1];

					if( usMBAPlen < (MB_TCP_UID_OFFSET + usLength) )
					{
						//todo ____error
						VMBPortReleaseSrcServer();
						continue;
					}
					else if( usMBAPlen == (MB_TCP_UID_OFFSET + usLength))
					{
						MB_err = eMBTCPServerServe(ucMBAPbuf, &usMBAPlen);
						if(MB_err == MB_ENOERR)
						{
							//send response over tcp
							if(prvxMBTCPPortSendResponse(newconn, ucMBAPbuf, usMBAPlen) != MB_ENOERR)
							{
								//todo
								// do something

							}
						}
						else if(MB_err == MB_EPROTOCOL)
						{
							VMBPortReleaseSrcServer();
							continue;

						}
						else
						{
							//todo
							//chưa nghĩ ra...

						}

					}

				}

			}

			VMBPortReleaseSrcServer();

		  }

	  } //if(recv_err == ERR_OK)
	  else
	  {
		  MB_err = MB_ELOSTCONN;
		  break;
	  }

	}//while(1)
  return MB_err;
}

void vMBPortTakeSrcServer(void){
	  xSemaphoreTake(mbresource, portMAX_DELAY);
}

void VMBPortReleaseSrcServer(void){

	xSemaphoreGive(mbresource);
}

void vMBPortReleaseClient( struct netconn  *newconn )
{
	netconn_close(newconn);
	netconn_delete(newconn);
}

static BOOL prvxMBTCPPortSendResponse( struct netconn *newconn, const UCHAR * pucMBTCPFrame, USHORT usTCPLength )	//input is MBAP frame
{
	eMBErrorCode    bFrameSent = MB_EIO;
	size_t uBytesWritten = 0;
	err_t err = ERR_OK;
	//netconn_set_sendtimeout(newconn, u16Timeoutms);
	err = netconn_write_partly(newconn, pucMBTCPFrame, (size_t)usTCPLength, NETCONN_COPY, &uBytesWritten);
	if(err == ERR_OK)
	{
		bFrameSent = MB_ENOERR;
	}
//	else
//	{
//		bFrameSent = TRUE;
//		countsend++;
//	}

  return bFrameSent;
}

#endif













