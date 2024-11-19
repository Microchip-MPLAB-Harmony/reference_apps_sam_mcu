/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#define RX_BUFFER_SIZE 1
#define TX_BUFFER_SIZE 1

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
volatile bool USART1_writeStatus = false;
volatile bool USART1_readStatus = false;

volatile bool USART0_writeStatus = false;
volatile bool USART0_readStatus = false;

uint8_t rxBuffer;
uint8_t txBuffer = 0xAA;

//USART0 - Extension

void USART_0_RTS_Enable(void)
{
    USART0_REGS->US_CR = US_CR_USART_RTSEN_Msk;
}

void USART_0_RTS_Disable(void)
{
    USART0_REGS->US_CR = US_CR_USART_RTSDIS_Msk;
}

void USART0_WriteEventHandler ( uintptr_t context )
{
    USART0_writeStatus = true;
}

void USART0_ReadEventHandler (uintptr_t context)
{
    USART_0_RTS_Enable();
    USART0_readStatus = true;
}

void USART_0_Init()
{
    USART0_REGS->US_MR |= US_MR_USART_MODE_HW_HANDSHAKING;
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    SYSTICK_TimerStart();
    USART_0_Init();
    
    USART0_WriteCallbackRegister(USART0_WriteEventHandler, (uintptr_t)NULL);
    USART0_ReadCallbackRegister(USART0_ReadEventHandler, (uintptr_t)NULL);
    
    USART_0_RTS_Disable();
    USART0_Read(&rxBuffer, RX_BUFFER_SIZE);
    
    USART0_Write(&txBuffer, TX_BUFFER_SIZE);
    
    while ( true )
    {    
        SYSTICK_DelayUs(500U);
        if(USART0_writeStatus == true)
        {
            USART0_writeStatus = false;

            //Transmit received bytes from EDBG
            USART0_Write(&txBuffer, TX_BUFFER_SIZE);
        }
        
        if(USART0_readStatus == true)
        {
            USART0_readStatus = false;
            
            //Receive transmitted bytes from EDBG
            USART_0_RTS_Disable();
            USART0_Read(&rxBuffer, RX_BUFFER_SIZE);
        }
        
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

