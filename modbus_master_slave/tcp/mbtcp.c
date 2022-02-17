/*
 * mbtcp.c
 *
 *  Created on: Nov 5, 2021
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

#define MB_TCP_PROTOCOL_ID  0   /* 0 = Modbus Protocol */

#if   MB_SLAVE_TCP_ENABLED

xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBFuncReadDiscreteInputs},
#endif
};


eMBErrorCode
eMBTCPDoInit( USHORT ucTCPPort )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( xMBTCPPortInit( ucTCPPort ) == FALSE )
    {
        eStatus = MB_EPORTERR;
    }
    return eStatus;
}

void
eMBTCPStart( void )
{

}

void
eMBTCPStop( void )
{


}

/*
 * brief: eMBTCPServerServe to process MBAP frame coming from Client and respond back to client
 *
 */
eMBErrorCode eMBTCPServerServe(UCHAR * pucMBAPFrame, USHORT *usMBAPlength ){
	eMBErrorCode    eStatus = MB_EIO;
	int i;
	USHORT 			usPID;
	UCHAR 			*pucPDUFrame;
	USHORT 			usPDULength;
	UCHAR			ucFunctionCode;
	eMBException 	eException;

	usPID  = pucMBAPFrame[MB_TCP_PID_OFFSET] << 8U;
	usPID |= pucMBAPFrame[MB_TCP_PID_OFFSET + 1];

	 if( usPID == MB_TCP_PROTOCOL_ID )
	 {
		 pucPDUFrame = &pucMBAPFrame[MB_TCP_FUNC_OFFSET];
		 usPDULength = *usMBAPlength - MB_TCP_FUNC_OFFSET;
		 eStatus = MB_ENOERR;
	 }
	 else{
		 eStatus = MB_EPROTOCOL;
		 return eStatus;
		 //error Protocol
		 //todo return or something
	 }

	 ucFunctionCode = pucPDUFrame[MB_PDU_FUNC_OFF];
	 eException = MB_EX_ILLEGAL_FUNCTION;
     for( i = 0; i < MB_FUNC_HANDLERS_MAX; i++ )
     {
         /* No more function handlers registered. Abort. */
         if( xFuncHandlers[i].ucFunctionCode == 0 )
         {
             break;
         }
         else if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode )
         {
             eException = xFuncHandlers[i].pxHandler( pucPDUFrame, &usPDULength );
             break;
         }
     }

     /* If the request was not a broadcast address*/
     if( eException != MB_EX_NONE )
     {
         /* An exception occured. Build an error frame. */
    	 usPDULength = 0;
         pucPDUFrame[usPDULength++] = ( UCHAR )( ucFunctionCode | MB_FUNC_ERROR );
         pucPDUFrame[usPDULength++] = eException;
     }

     //REsetup MBAP Frame to respond to Client
     *usMBAPlength =  usPDULength + MB_TCP_FUNC_OFFSET;
     pucMBAPFrame[MB_TCP_LEN_OFFSET] = ( usPDULength + 1 ) >> 8U;
     pucMBAPFrame[MB_TCP_LEN_OFFSET + 1] = ( usPDULength + 1 ) & 0xFF;

     return eStatus;
}


#endif
