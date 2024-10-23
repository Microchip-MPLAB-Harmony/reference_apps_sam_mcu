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

void RTS_ENABLE(void)
{
    USART0_REGS->US_CR = US_CR_USART_RTSEN_Msk;
}

void RTS_DISABLE(void)
{
    USART0_REGS->US_CR = US_CR_USART_RTSDIS_Msk;
}

void USART0_WriteEventHandler ( uintptr_t context )
{
    USART0_writeStatus = true;
}

void USART0_ReadEventHandler (uintptr_t context)
{
    RTS_ENABLE();
    USART0_readStatus = true;
}

void ext_usart_init()
{
    USART0_REGS->US_MR |= US_MR_USART_MODE_HW_HANDSHAKING;
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    SYSTICK_TimerStart();
    ext_usart_init();
    
    USART0_WriteCallbackRegister(USART0_WriteEventHandler, (uintptr_t)NULL);
    USART0_ReadCallbackRegister(USART0_ReadEventHandler, (uintptr_t)NULL);
    
    RTS_DISABLE();
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
            RTS_DISABLE();
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

