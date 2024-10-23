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
#include <stdio.h>
#include <string.h>  
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

#define RX_BUFFER_SIZE 1 
volatile bool rx_done = false;
uint8_t edbg_rx_data;
volatile bool tx_done = false;

void APP_SERCOM_5_WriteCallback(uintptr_t context)
{
   tx_done = true;
}

void APP_SERCOM_5_ReadCallback(uintptr_t context)
{
    if((SERCOM5_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXS_Msk) == SERCOM_USART_INT_INTFLAG_RXS_Msk)
    {
        SERCOM5_REGS->USART_INT.SERCOM_INTFLAG |= (uint8_t)SERCOM_USART_INT_INTFLAG_RXS_Msk;
        
    }  
    
    rx_done = true;
}

void usart_send_string(const char *str)
{
    SERCOM5_USART_Write((void *)&str[0], strlen(str));
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    SERCOM5_REGS->USART_INT.SERCOM_INTENSET = (uint8_t)SERCOM_USART_INT_INTENSET_RXS_Msk;
    
    // EDBG SERCOM Read and Write Callback
    SERCOM5_USART_ReadCallbackRegister(APP_SERCOM_5_ReadCallback, 0);
    SERCOM5_USART_WriteCallbackRegister(APP_SERCOM_5_WriteCallback, 0);
    
    // Read request for EDBG
    SERCOM5_USART_Read(&edbg_rx_data, RX_BUFFER_SIZE);
    
    while ( true )
    { 
        if (!rx_done)
        {
            tx_done = false;
            usart_send_string("\r\n Device entered into standby sleep mode");
            while(!(SERCOM5_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_TXC_Msk));
            
            // Enters standby sleep mode.
            PM_StandbyModeEnter();
        }
        while(!rx_done);
        
        tx_done = false;
        usart_send_string("\r\n Character received after wakeup :");
        while(!tx_done);
        
        tx_done = false;
        SERCOM5_USART_Write(&edbg_rx_data, RX_BUFFER_SIZE);
        while(!tx_done);
        
        rx_done = false;
        SERCOM5_USART_Read(&edbg_rx_data, RX_BUFFER_SIZE);
        SYS_Tasks ( );
    }


    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/
