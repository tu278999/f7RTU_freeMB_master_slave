/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbtcp.h,v 1.2 2006/12/07 22:10:34 wolti Exp $
 */

#ifndef _MB_TCP_H
#define _MB_TCP_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif
#include "e_port.h"
/* ----------------------- Defines ------------------------------------------*/
#define MB_TCP_PSEUDO_ADDRESS   255

#define MB_TCP_DEFAULT_PORT  		502        /* TCP listening port. */
#define MB_TCP_BUF_MAX_SIZE     	(256 + 7) /* Must hold a complete Modbus TCP frame. */


#define MB_TCP_TID_OFFSET      	0
#define MB_TCP_PID_OFFSET       2
#define MB_TCP_LEN_OFFSET       4
#define MB_TCP_UID_OFFSET       6
#define MB_TCP_FUNC_OFFSET      7

#define MB_TCP_PROTOCOL_ID  	0   /* 0 = Modbus Protocol */

/* ----------------------- Function prototypes ------------------------------*/
eMBErrorCode eMBTCPDoInit( USHORT ucTCPPort );

#if MB_SLAVE_TCP_ENABLED

void            eMBTCPStart( void );
void            eMBTCPStop( void );

eMBErrorCode eMBTCPServerServe(UCHAR * pucMBAPFrame, USHORT *usMBAPlength );

eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs );
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode );
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode );
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete );
#endif /*MB_SLAVE_TCP_ENABLED*/
#if MB_MASTER_TCP_ENABLED

eMBErrorCode
eMBMasterTCPDoInit( USHORT ucTCPPort );
void
eMBMasterTCPStart( void );
void
eMBMasterTCPStop( void );

void vMBMasterTCPReqCloseConn(UCHAR address);

//
//eMBErrorCode eMBMasterRegInputCB( 	UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs );
//eMBErrorCode eMBMasterRegHoldingCB(	UCHAR * pucRegBuffer, USHORT usAddress,
//        							USHORT usNRegs, eMBRegisterMode eMode);
//eMBErrorCode eMBMasterRegCoilsCB(	UCHAR * pucRegBuffer, USHORT usAddress,
//        							USHORT usNCoils, eMBRegisterMode eMode);
//eMBErrorCode eMBMasterRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete );

eMBErrorCode eMBMasterCheckMBAPRcvBuf(UCHAR  *ucMasterMBAPRcvBuf, USHORT usMasterRcvMBAPLength);

void vMBMasterGetMBAPSndBuf( UCHAR ** pucFrame, USHORT *pusLength );
void vMBMasterGetMBAPRcvBuf(UCHAR **pucFrame, USHORT *puslength);



#endif /* */

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
