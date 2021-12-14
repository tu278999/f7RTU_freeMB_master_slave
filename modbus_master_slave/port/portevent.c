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
 * File: $Id: portevent.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "event_groups.h"
#include "e_port.h"
#include "mb.h"
#include "mbport.h"

#if MB_SLAVE_RTU_ENABLED > 0

/* ----------------------- Variables ----------------------------------------*/
static EventGroupHandle_t	xSlaveOsEvent = NULL;


/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
	xSlaveOsEvent = xEventGroupCreate();

	configASSERT(xSlaveOsEvent != NULL);

    return TRUE;
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
	xEventGroupSetBits(xSlaveOsEvent, eEvent);
    return TRUE;
}

BOOL
xMBPortEventPostFromISR( eMBEventType eEvent )
{
	BaseType_t xresult = pdFAIL;
	xresult = xEventGroupSetBitsFromISR(xSlaveOsEvent, eEvent, NULL);
	if(xresult == pdFAIL){
		while(1);
	}
	return TRUE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
	EventBits_t recEvent;
	recEvent = xEventGroupWaitBits(	xSlaveOsEvent,
												   (EV_READY 		  | //do i need to use
													EV_FRAME_RECEIVED | // type casting
													EV_EXECUTE 		  |
													EV_FRAME_SENT 	  ),
									pdTRUE, //clear on exit
									pdFALSE,//wait for any bits
									portMAX_DELAY);

    switch (recEvent)
    {
    case EV_READY:
        *eEvent = EV_READY;
        break;
    case EV_FRAME_RECEIVED:
        *eEvent = EV_FRAME_RECEIVED;
        break;
    case EV_EXECUTE:
        *eEvent = EV_EXECUTE;
        break;
    case EV_FRAME_SENT:
        *eEvent = EV_FRAME_SENT;
        break;
    }
    return TRUE;
}

#endif
