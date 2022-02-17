/*
 * mbtcp_m.c
 *
 *  Created on: Dec 14, 2021
 *      Author: tu.lb174310
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "e_port.h"


/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbtcp.h"
#include "mbframe.h"
#include "mbport.h"
#include "mbproto.h"
#include "mbfunc.h"

#if MB_MASTER_TCP_ENABLED

/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */

#define MBAP_TCP_TID_OFFSET     0
#define MBAP_TCP_PID_OFFSET     2
#define MBAP_TCP_LEN_OFFSET     4
#define MBAP_TCP_UID_OFFSET     6
#define MBAP_TCP_FUNC_OFFSET    7

//#define MBAP_TCP_TID_HI			0X01
//#define MBAP_TCP_TID_LO			0XAD
#define MBAP_TCP_PID  			0   /* 0 = Modbus Protocol */

#define MBTCP_NUMBEROFSERVER	3

/*
 * NOTE: If client connected directly to the TCP/IP network => UID = 0xff and must be discarded.
 * 		 If the remote server is connected on a Seral Line sub-network and the
 * 		 response comes from a bridge, a router or a gateway, then UID( which != 0xFF)
 * 		 identifies the remote MODBUS server which has originally sent the response.
 */
#define MBAP_TCP_UID_BROADCAST	0	/* Broadcast address*/
#define MBAP_TCP_UID			1	/* This must be variable as address of slave todo*/

static USHORT usMBAP_TCP_TID = 0;

static  UCHAR  ucMasterMBAPSndBuf[MB_PDU_SIZE_MAX + 7];

static  USHORT usMasterSendPDULength;

//static  UCHAR  *ucMasterMBAPRcvBuf;
//static  USHORT usMasterRcvPDULength;//
//static  USHORT usMasterSendMBAPLength;//
//static  USHORT usMasterRcvMBAPLength;

static UCHAR    ucMBMasterDestAddress;
static BOOL     xMBRunInMasterMode = FALSE;
static volatile BOOL   xFrameIsBroadcast = FALSE;

static xMBFunctionHandler xMasterFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
	//TODO Add Master function define
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBMasterFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBMasterFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBMasterFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBMasterFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBMasterFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBMasterFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBMasterFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBMasterFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBMasterFuncReadDiscreteInputs},
#endif
};

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode
eMBMasterTCPDoInit( USHORT ucTCPPort )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( xMBMasterTCPPortInit( ucTCPPort ) == FALSE )
    {
        eStatus = MB_EPORTERR;
    }
    return eStatus;
}

void
eMBMasterTCPStart( void )
{

}

void
eMBMasterTCPStop( void )
{


}

void vMBMasterGetMBAPSndBuf( UCHAR ** pucFrame, USHORT *pusLength )
{

	usMBAP_TCP_TID++;
	//trước mắt cứ code theo kiểu "Only one Request at a time" cho chạy được đã
	ucMasterMBAPSndBuf[MBAP_TCP_TID_OFFSET] 	= usMBAP_TCP_TID >> 8U;	//high
	ucMasterMBAPSndBuf[MBAP_TCP_TID_OFFSET + 1] = usMBAP_TCP_TID & 0XFF;//low

	ucMasterMBAPSndBuf[MBAP_TCP_PID_OFFSET] 	= MBAP_TCP_PID;
	ucMasterMBAPSndBuf[MBAP_TCP_PID_OFFSET + 1] = MBAP_TCP_PID;

	ucMasterMBAPSndBuf[MBAP_TCP_LEN_OFFSET] 	= ( usMasterSendPDULength + 1 ) >> 8U;
	ucMasterMBAPSndBuf[MBAP_TCP_LEN_OFFSET + 1] = ( usMasterSendPDULength + 1 ) & 0xFF;

	ucMasterMBAPSndBuf[MBAP_TCP_UID_OFFSET]   	= MBAP_TCP_UID;	//should be a variable todo

	*pucFrame = ( UCHAR * ) ucMasterMBAPSndBuf;
	*pusLength = usMasterSendPDULength + MBAP_TCP_FUNC_OFFSET;

}


void vMBMasterGetMBAPRcvBuf(UCHAR **pucFrame, USHORT *puslength){
//	*pucFrame  = (UCHAR*)&ucMasterMBAPRcvBuf[0];
//	 puslength = &usMasterRcvMBAPLength;
}


eMBException eExtTCPMaster = MB_EX_NONE;

eMBErrorCode eMBMasterCheckMBAPRcvBuf(UCHAR  *ucMasterMBAPRcvBuf, USHORT usMasterRcvMBAPLength){
	eMBErrorCode eStatus = MB_ENOERR;
	USHORT usLength = 0;
	USHORT usLengthRcvPDU = 0;
	UCHAR  ucFunctionCode;
	UCHAR  *pucPDURcvBuf;
	int i;


	if( (usMasterRcvMBAPLength >= (MBAP_TCP_FUNC_OFFSET + 2)) // '2' involve to exception code which has Two bytes.
		&&(usMasterRcvMBAPLength <= (MB_PDU_SIZE_MAX + MBAP_TCP_FUNC_OFFSET)) )
	{
		usLength  = ucMasterMBAPRcvBuf[MBAP_TCP_LEN_OFFSET] << 8U;
		usLength |= ucMasterMBAPRcvBuf[MBAP_TCP_LEN_OFFSET + 1];

		if( usMasterRcvMBAPLength == (usLength + MBAP_TCP_UID_OFFSET) )
		{
			if(	ucMasterMBAPRcvBuf[MBAP_TCP_TID_OFFSET] 	== (usMBAP_TCP_TID >> 8U) &&
				ucMasterMBAPRcvBuf[MBAP_TCP_TID_OFFSET + 1] == (usMBAP_TCP_TID & 0XFF)&&
				ucMasterMBAPRcvBuf[MBAP_TCP_PID_OFFSET] 	== MBAP_TCP_PID   	 	  &&
				ucMasterMBAPRcvBuf[MBAP_TCP_PID_OFFSET + 1] == MBAP_TCP_PID           &&
				ucMasterMBAPRcvBuf[MBAP_TCP_UID_OFFSET]		== MBAP_TCP_UID)
			{
				pucPDURcvBuf 	=  &ucMasterMBAPRcvBuf[MBAP_TCP_FUNC_OFFSET];
				ucFunctionCode 	= ucMasterMBAPRcvBuf[MBAP_TCP_FUNC_OFFSET];
				eExtTCPMaster 		= MB_EX_ILLEGAL_FUNCTION;
				if( ucFunctionCode >= 0x80)
				{
					eExtTCPMaster = (eMBException)pucPDURcvBuf[MB_PDU_DATA_OFF];
				}
				else
				{
					for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++)
					{
						/* No more function handlers registered. Abort. */
						if (xMasterFuncHandlers[i].ucFunctionCode == 0)
						{
							break;
						}
						else if (xMasterFuncHandlers[i].ucFunctionCode == ucFunctionCode)
						{
							vMBMasterSetCBRunInMasterMode(TRUE);
							usLengthRcvPDU = usLength - 1;
							eExtTCPMaster = xMasterFuncHandlers[i].pxHandler(pucPDURcvBuf, &usLengthRcvPDU);
							vMBMasterSetCBRunInMasterMode(FALSE);
							break;
						}
					}
				}

			}
			else
			{
				//error receive data;
				eStatus = MB_EIO;
			}

		}
		else
		{
			//error receive data;
			eStatus = MB_EIO;
		}

	}

	return eStatus;
}

/*
 * brief: Close the current connection
 * 	after This specified client(stm32 master MBTCP) is done with the remote server
 * 	we have post Event to close the connection and let's the remote server serve the other client
 */
void vMBMasterTCPReqCloseConn(UCHAR address){
	vMBMasterSetDestAddress(address);
	xMBMasterRunResTake(portMAX_DELAY);
	xMBMasterPortEventPost(EV_MASTER_CLOSE_CONNECTION);
}


/* Get Modbus Master send PDU's buffer address pointer.*/
void vMBMasterGetPDUSndBuf( UCHAR ** pucFrame )
{
	*pucFrame = ( UCHAR * ) &ucMasterMBAPSndBuf[MBAP_TCP_FUNC_OFFSET];
}

/* Set Modbus Master send PDU's buffer length.*/
void vMBMasterSetPDUSndLength( USHORT SendPDULength )
{
	usMasterSendPDULength = SendPDULength;
}

/* Get Modbus Master send PDU's buffer length.*/
USHORT usMBMasterGetPDUSndLength( void )
{
	return usMasterSendPDULength;
}

BOOL xMBMasterGetCBRunInMasterMode( void )
{
	return xMBRunInMasterMode;
}
/* Set whether the Modbus Master is run in master mode.*/
void vMBMasterSetCBRunInMasterMode( BOOL IsMasterMode )
{
	xMBRunInMasterMode = IsMasterMode;
}


//TODO : Destination address in TCP must be IP_address (ip4_addr_t)
//todo : note: ucMBMasterDestAddress variable must be changed
void vMBMasterSetDestAddress( UCHAR Address )
{
	ucMBMasterDestAddress = Address;
}

/* Get Modbus Master send destination address. */
UCHAR ucMBMasterGetDestAddress( void )
{
	return ucMBMasterDestAddress;
 }

/* The master request is broadcast? */
void xMBMasterSetBroadcast( BOOL isbroadcast ){
	xFrameIsBroadcast = isbroadcast;
}
/* The master request is broadcast? */
BOOL xMBMasterRequestIsBroadcast( void ){
	return xFrameIsBroadcast;
}

#endif /*MB_MASTER_TCP_ENABLED*/
