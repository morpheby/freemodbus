/*
 * FreeModbus Libary: Atmel AT91SAM3S Demo Application
 * Copyright (C) 2010 Christian Walter <cwalter@embedded-solutions.at>
 *
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * IF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id$
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "port.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define USART1_ENABLED          ( 1 )
#define USART1_REMAP            ( 0 )
#define USART1_IDX              ( 0 )

#define USART2_ENABLED          ( 1 )
#define USART2_IDX              ( USART1_IDX + USART1_ENABLED * 1 )

#define USART3_ENABLED          ( 1 )
#define USART3_REMAP            ( 0 )
#define USART3_IDX              ( USART1_IDX + USART1_ENABLED * 1 + USART2_ENABLED * 1 )

#define USART4_ENABLED          ( 1 )
#define USART4_IDX              ( USART1_IDX + USART1_ENABLED * 1 + USART2_ENABLED * 1 + USART3_ENABLED * 1  )

#define USART_IDX_LAST          ( USART2_IDX )

#define USART_INVALID_PORT      ( 0xFF )

/* ----------------------- Static variables ---------------------------------*/

typedef struct {
    uint16_t pin;
    GPIO_TypeDef *gpio;
    GPIOSpeed_TypeDef speed;
    GPIOMode_TypeDef mode;
} Pin;

#if USART1_ENABLED == 1 && USART1_REMAP == 0
const Pin       xUSART1Pins[] = {
    { GPIO_Pin_9, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_AF_PP },
    { GPIO_Pin_10, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING },
};
const Pin       xUSART1NotREPin = { GPIO_Pin_12, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_AF_PP };
const Pin       xUSART1DEPin = { GPIO_Pin_11, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_AF_PP };
#endif

#if USART1_ENABLED == 1 && USART1_REMAP == 1
const Pin       xUSART1Pins[] = {
    { GPIO_Pin_6, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_AF_PP },
    { GPIO_Pin_7, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING },
};
const Pin       xUSART1NotREPin = { GPIO_Pin_12, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_AF_PP };
const Pin       xUSART1DEPin = { GPIO_Pin_11, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING };
#endif

#if USART2_ENABLED == 1
const Pin       xUSART2Pins[] = {
    { GPIO_Pin_2, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_AF_PP },
    { GPIO_Pin_3, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING },
};
const Pin       xUSART2NotREPin = { GPIO_Pin_1, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_AF_PP };
const Pin       xUSART2DEPin = { GPIO_Pin_0, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING };
#endif

#if USART3_ENABLED == 1 && USART3_REMAP == 0
const Pin       xUSART3Pins[] = {
    { GPIO_Pin_10, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_AF_PP },
    { GPIO_Pin_11, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING},
};
const Pin       xUSART3NotREPin = { GPIO_Pin_14, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_AF_PP };
const Pin       xUSART3DEPin = { GPIO_Pin_13, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING };
#endif

#if USART3_ENABLED == 1 && USART3_REMAP == 1
const Pin       xUSART3Pins[] = {
    { GPIO_Pin_10, GPIOC, GPIO_Speed_50MHz, GPIO_Mode_AF_PP },
    { GPIO_Pin_11, GPIOC, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING },
};
const Pin       xUSART3NotREPin = { GPIO_Pin_14, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_AF_PP };
const Pin       xUSART3DEPin = { GPIO_Pin_13, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING };
#endif

#if USART4_ENABLED == 1 && USART4_REMAP == 0
const Pin       xUSART4Pins[] = {
    { GPIO_Pin_0, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_AF_PP },
    { GPIO_Pin_1, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING },
};
const Pin       xUSART4NotREPin = { GPIO_Pin_4, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_AF_PP };
const Pin       xUSART4DEPin = { GPIO_Pin_3, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING };
#endif

#if USART4_ENABLED == 1 && USART4_REMAP == 1
const Pin       xUSART4Pins[] = {
    { GPIO_Pin_5, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_AF_PP },
    { GPIO_Pin_5, GPIOB, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING },
};
const Pin       xUSART4NotREPin = { GPIO_Pin_15, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_AF_PP };
const Pin       xUSART4DEPin = { GPIO_Pin_7, GPIOA, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING };
#endif

const struct xUSARTHWMappings_t
{
    USART_TypeDef  *pUsart;
    unsigned int    xUSARTID;
    IRQn_Type       xUSARTIrq;
    const Pin      *USARTNotREPin;
    const Pin      *USARTDEPin;
    const Pin      *xUSARTPins;
    uint32_t        xUSARTPinsCnt;


} xUSARTHWMappings[] =
{
#if USART1_ENABLED == 1
    {
    USART1, 0, USART1_IRQn, &xUSART1NotREPin, &xUSART1DEPin, &xUSART1Pins[0], 2},
#endif
#if USART2_ENABLED == 1
    {
    USART2, 1, USART2_IRQn, &xUSART2NotREPin, &xUSART2DEPin, &xUSART2Pins[0], 2},
#endif
#if USART3_ENABLED == 1
    {
    USART3, 2, USART3_IRQn, &xUSART3NotREPin, &xUSART3DEPin, &xUSART3Pins[0], 2},
#endif
#if USART4_ENABLED == 1
    {
    UART4, 3, UART4_IRQn, &xUSART4NotREPin, &xUSART4DEPin, &xUSART4Pins[0], 2},
#endif
};

static UCHAR    ucUsedPort = USART_INVALID_PORT;

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    if( xRxEnable )
    {
        xUSARTHWMappings[ucUsedPort].pUsart->CTLR1 |= USART_Mode_Rx;
        USART_ITConfig(xUSARTHWMappings[ucUsedPort].pUsart, USART_IT_RXNE, ENABLE);
    }
    else
    {
        xUSARTHWMappings[ucUsedPort].pUsart->CTLR1 &= ~USART_Mode_Rx;
        USART_ITConfig(xUSARTHWMappings[ucUsedPort].pUsart, USART_IT_RXNE, DISABLE);
    }

#if USART1_REMAP == 1
    if (xUSARTHWMappings[ucUsedPort].pUsart == USART1) {
        GPIO_PinRemapConfig(GPIO_Remap_USART1);
    }
#endif

#if USART3_REMAP == 1
    if (xUSARTHWMappings[ucUsedPort].pUsart == USART3) {
        GPIO_PinRemapConfig(GPIO_PartialRemap_USART3);
    }
#endif

#if USART4_REMAP == 1
    if (xUSARTHWMappings[ucUsedPort].pUsart == USART4) {
        GPIO_PinRemapConfig(GPIO_PartialRemap_USART4);
    }
#endif

    if( xTxEnable )
    {
        if( NULL != xUSARTHWMappings[ucUsedPort].USARTNotREPin )
        {
            GPIO_InitTypeDef gpio = { xUSARTHWMappings[ucUsedPort].USARTNotREPin->pin,
                                      xUSARTHWMappings[ucUsedPort].USARTNotREPin->speed,
                                      xUSARTHWMappings[ucUsedPort].USARTNotREPin->mode };
            GPIO_Init(xUSARTHWMappings[ucUsedPort].USARTNotREPin->gpio, &gpio);
            xUSARTHWMappings[ucUsedPort].pUsart->CTLR3 |= USART_HardwareFlowControl_CTS;
        } else {
            xUSARTHWMappings[ucUsedPort].pUsart->CTLR3 &= ~USART_HardwareFlowControl_CTS;
        }
        if( NULL != xUSARTHWMappings[ucUsedPort].USARTDEPin )
        {
            GPIO_InitTypeDef gpio = { xUSARTHWMappings[ucUsedPort].USARTDEPin->pin,
                                      xUSARTHWMappings[ucUsedPort].USARTDEPin->speed,
                                      xUSARTHWMappings[ucUsedPort].USARTDEPin->mode };
            GPIO_Init(xUSARTHWMappings[ucUsedPort].USARTDEPin->gpio, &gpio);
            xUSARTHWMappings[ucUsedPort].pUsart->CTLR3 |= USART_HardwareFlowControl_RTS;
        } else {
            xUSARTHWMappings[ucUsedPort].pUsart->CTLR3 &= ~USART_HardwareFlowControl_RTS;
        }
        xUSARTHWMappings[ucUsedPort].pUsart->CTLR1 |= USART_Mode_Tx;
        USART_ITConfig(xUSARTHWMappings[ucUsedPort].pUsart, USART_IT_TC, ENABLE);
        USART_ITConfig(xUSARTHWMappings[ucUsedPort].pUsart, USART_IT_TXE, DISABLE);
    }
    else
    {
        xUSARTHWMappings[ucUsedPort].pUsart->CTLR1 &= ~USART_Mode_Tx;
        USART_ITConfig(xUSARTHWMappings[ucUsedPort].pUsart, USART_IT_TC, DISABLE);
        USART_ITConfig(xUSARTHWMappings[ucUsedPort].pUsart, USART_IT_TXE, ENABLE);
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            bStatus = FALSE;
    
    USART_InitTypeDef usartInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    usartInit.USART_Mode = 0;
    usartInit.USART_BaudRate = ulBaudRate;
    usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    if( ( ucPORT <= USART_IDX_LAST ) )
    {
        bStatus = TRUE;
        switch ( eParity )
        {
        case MB_PAR_NONE:
            usartInit.USART_Parity = USART_Parity_No;
            usartInit.USART_StopBits = USART_StopBits_2;
            break;
        case MB_PAR_ODD:
            usartInit.USART_Parity = USART_Parity_Odd;
            usartInit.USART_StopBits = USART_StopBits_1;
            break;
        case MB_PAR_EVEN:
            usartInit.USART_Parity = USART_Parity_Even;
            usartInit.USART_StopBits = USART_StopBits_1;
            break;
        default:
            bStatus = FALSE;
            break;
        }

        switch ( ucDataBits )
        {
        case 8:
            usartInit.USART_WordLength = USART_WordLength_8b;
            break;
        default:
            bStatus = FALSE;
        }

        if( TRUE == bStatus )
        {
            ucUsedPort = ucPORT;

            NVIC_DisableIRQ( xUSARTHWMappings[ucUsedPort].xUSARTIrq );

            for (int i = 0; i < xUSARTHWMappings[ucUsedPort].xUSARTPinsCnt; ++i) {
                GPIO_InitTypeDef gpio = { xUSARTHWMappings[ucUsedPort].xUSARTPins[i].pin,
                                          xUSARTHWMappings[ucUsedPort].xUSARTPins[i].speed,
                                          xUSARTHWMappings[ucUsedPort].xUSARTPins[i].mode };
                GPIO_Init(xUSARTHWMappings[ucUsedPort].USARTDEPin[i].gpio, &gpio);
            }
            if( NULL != xUSARTHWMappings[ucUsedPort].USARTNotREPin )
            {
                GPIO_InitTypeDef gpio = { xUSARTHWMappings[ucUsedPort].USARTNotREPin->pin,
                                          xUSARTHWMappings[ucUsedPort].USARTNotREPin->speed,
                                          xUSARTHWMappings[ucUsedPort].USARTNotREPin->mode };
                GPIO_Init(xUSARTHWMappings[ucUsedPort].USARTNotREPin->gpio, &gpio);
                usartInit.USART_HardwareFlowControl |= USART_HardwareFlowControl_CTS;
            }
            if( NULL != xUSARTHWMappings[ucUsedPort].USARTDEPin )
            {
                GPIO_InitTypeDef gpio = { xUSARTHWMappings[ucUsedPort].USARTDEPin->pin,
                                          xUSARTHWMappings[ucUsedPort].USARTDEPin->speed,
                                          xUSARTHWMappings[ucUsedPort].USARTDEPin->mode };
                GPIO_Init(xUSARTHWMappings[ucUsedPort].USARTDEPin->gpio, &gpio);
                usartInit.USART_HardwareFlowControl |= USART_HardwareFlowControl_RTS;
            }
            
#if USART1_REMAP == 1
            if (xUSARTHWMappings[ucUseducPORTPort].pUsart == USART1) {
                GPIO_PinRemapConfig(GPIO_Remap_USART1);
            }
#endif

#if USART3_REMAP == 1
            if (xUSARTHWMappings[ucUsedPort].pUsart == USART3) {
                GPIO_PinRemapConfig(GPIO_PartialRemap_USART3);
            }
#endif

#if USART4_REMAP == 1
            if (xUSARTHWMappings[ucUsedPort].pUsart == USART4) {
                GPIO_PinRemapConfig(GPIO_PartialRemap_USART4);
            }
#endif

            if (xUSARTHWMappings[ucUsedPort].pUsart == USART1) {
                RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE);
            } else if (xUSARTHWMappings[ucUsedPort].pUsart == USART2) {
                RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE);
            } else if (xUSARTHWMappings[ucUsedPort].pUsart == USART3) {
                RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);
            } else if (xUSARTHWMappings[ucUsedPort].pUsart == UART4) {
                RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4, ENABLE);
            }
            USART_Init(xUSARTHWMappings[ucUsedPort].pUsart, &usartInit);

            NVIC_ClearPendingIRQ( xUSARTHWMappings[ucUsedPort].xUSARTIrq );
            NVIC_SetPriority( xUSARTHWMappings[ucUsedPort].xUSARTIrq, 0xF << 4 );
            NVIC_EnableIRQ( xUSARTHWMappings[ucUsedPort].xUSARTIrq );

            USART_Cmd(xUSARTHWMappings[ucUsedPort].pUsart, ENABLE);
        }
    }

    return bStatus;
}

void
vMBPortSerialClose( void )
{
    if( USART_INVALID_PORT != ucUsedPort )
    {
        NVIC_DisableIRQ( xUSARTHWMappings[ucUsedPort].xUSARTIrq );
        USART_Cmd(xUSARTHWMappings[ucUsedPort].pUsart, DISABLE);
        ucUsedPort = USART_INVALID_PORT;
    }
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    USART_SendData(xUSARTHWMappings[ucUsedPort].pUsart, ucByte);
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = (CHAR) USART_ReceiveData(xUSARTHWMappings[ucUsedPort].pUsart);
    return TRUE;
}

void
vUSARTHandler( void )
{
    BOOL            bTaskWoken = FALSE;

    vMBPortSetWithinException( TRUE );

    if( USART_GetFlagStatus(xUSARTHWMappings[ucUsedPort].pUsart, USART_FLAG_RXNE) == SET )
    {
        bTaskWoken = pxMBFrameCBByteReceived(  );
        USART_ClearFlag(xUSARTHWMappings[ucUsedPort].pUsart, USART_FLAG_RXNE);
    }
    if( USART_GetFlagStatus(xUSARTHWMappings[ucUsedPort].pUsart, USART_FLAG_TC) == SET )
    {
        bTaskWoken = pxMBFrameCBTransmitterEmpty(  );
        USART_ClearFlag(xUSARTHWMappings[ucUsedPort].pUsart, USART_FLAG_TC);
    }
    if( USART_GetFlagStatus(xUSARTHWMappings[ucUsedPort].pUsart, USART_FLAG_TXE) == SET )
    {
        USART_ITConfig(xUSARTHWMappings[ucUsedPort].pUsart, USART_IT_TXE, DISABLE);
    }
    vMBPortSetWithinException( FALSE );

    portEND_SWITCHING_ISR( bTaskWoken ? pdTRUE : pdFALSE );
}

#if USART1_ENABLED == 1
void
__attribute__((externally_visible, interrupt))
USART1_IRQHandler( void )
{
    vUSARTHandler(  );
}
#endif

#if USART2_ENABLED == 1
void
__attribute__((externally_visible, interrupt))
USART2_IRQHandler( void )
{
    vUSARTHandler(  );
}
#endif

#if USART3_ENABLED == 1
void
__attribute__((externally_visible, interrupt))
USART3_IRQHandler( void )
{
    vUSARTHandler(  );
}
#endif

#if USART4_ENABLED == 1
void
__attribute__((externally_visible, interrupt))
UART4_IRQHandler( void )
{
    vUSARTHandler(  );
}
#endif