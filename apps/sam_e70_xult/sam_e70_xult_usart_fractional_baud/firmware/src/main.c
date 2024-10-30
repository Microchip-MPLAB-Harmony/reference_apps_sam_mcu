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
#define FRAC_BAUD_RATE 11000
#define USART_SAMPLE_NUM 16
#define BAUD_DIVISOR 1000
#define BAUD_FRACTION_MULT 8
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
bool USART1_writeStatus = false;
bool USART1_readStatus = false;

bool USART0_writeStatus = false;
bool USART0_readStatus = false;

uint16_t cd;
uint8_t fp;

//USART0 - Extension
//USART 1 - EDBG

void USART1_WriteEventHandler ( uintptr_t context )
{
    USART1_writeStatus = true;
}

void USART0_WriteEventHandler ( uintptr_t context )
{
    USART0_writeStatus = true;
}

void USART1_ReadEventHandler (uintptr_t context)
{
    USART1_readStatus = true;
}

void USART0_ReadEventHandler (uintptr_t context)
{
    USART0_readStatus = true;
}

void USART_1_Fractional_Baud(const uint32_t baudrate,const uint32_t peripheral_clock,uint8_t sample_num)
{
    uint32_t mul_ratio;
    mul_ratio = (uint64_t)((uint64_t)peripheral_clock*(uint64_t)BAUD_DIVISOR)/(uint64_t)(baudrate*sample_num);
    cd = mul_ratio/BAUD_DIVISOR;
    fp = ((mul_ratio - (cd*BAUD_DIVISOR))*BAUD_FRACTION_MULT)/BAUD_DIVISOR;
}

void USART_1_usart_init(void)
{
    USART_1_Fractional_Baud(FRAC_BAUD_RATE,USART0_FrequencyGet(),USART_SAMPLE_NUM);
    
    USART0_REGS->US_CR = (US_CR_USART_RXDIS_Msk & US_CR_USART_TXDIS_Msk);
    
    USART0_REGS->US_BRGR = US_BRGR_CD(cd) | US_BRGR_CD(fp);
    
    USART0_REGS->US_CR = (US_CR_USART_TXEN_Msk | US_CR_USART_RXEN_Msk);
}
int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    USART_1_usart_init();
    
    // EDBG SERCOM Read and Write Callback
    USART1_WriteCallbackRegister(USART1_WriteEventHandler, (uintptr_t)NULL); 
    USART1_ReadCallbackRegister(USART1_ReadEventHandler, (uintptr_t)NULL);
    
    // Extension SERCOM Read and Write Callback
    USART0_WriteCallbackRegister(USART0_WriteEventHandler, (uintptr_t)NULL);
    USART0_ReadCallbackRegister(USART0_ReadEventHandler, (uintptr_t)NULL);
    
    uint8_t rxBuffer;
    
    // Read request for EDBG
    USART1_Read(&rxBuffer, RX_BUFFER_SIZE);
    
    // Read request for Extension
    USART0_Read(&rxBuffer, RX_BUFFER_SIZE);
    while ( true )
    {
        if(USART0_readStatus == true)
        {
            USART0_readStatus = false;
            
            //Transmit received bytes from EDBG
            USART1_Write(&rxBuffer, RX_BUFFER_SIZE);
        }
        
        if(USART1_writeStatus == true)
        {
            USART1_writeStatus = false;
            USART0_Read(&rxBuffer, RX_BUFFER_SIZE);
        }
        
        if(USART1_readStatus == true)
        {
            USART1_readStatus = false;
            
            //Transmit received bytes from Extension
            USART0_Write(&rxBuffer, RX_BUFFER_SIZE);
        }
        if(USART0_writeStatus == true)
        {
            USART0_writeStatus = false;
            USART1_Read(&rxBuffer, RX_BUFFER_SIZE);
        }
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

