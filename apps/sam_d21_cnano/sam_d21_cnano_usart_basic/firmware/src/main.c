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
  
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
#define RX_BUFFER_SIZE 1

volatile bool SERCOM_1_readStatus = false;
volatile bool SERCOM_1_writeStatus = false;

volatile bool SERCOM_5_readStatus = false;
volatile bool SERCOM_5_writeStatus = false;

//SERCOM 1 - Extension
//SERCOM 5 - EDBG

void SERCOM_1_WriteCallback(uintptr_t context)
{
    SERCOM_1_writeStatus = true;
}

void SERCOM_1_ReadCallback(uintptr_t context)
{
    SERCOM_1_readStatus = true;
}

void SERCOM_5_WriteCallback(uintptr_t context)
{
    SERCOM_5_writeStatus = true;
}

void SERCOM_5_ReadCallback(uintptr_t context)
{
    SERCOM_5_readStatus = true;
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    // Extension SERCOM Read and Write Callback 
    SERCOM1_USART_ReadCallbackRegister(SERCOM_1_ReadCallback, 0);
    SERCOM1_USART_WriteCallbackRegister(SERCOM_1_WriteCallback, 0);
    
    // EDBG SERCOM Read and Write Callback 
    SERCOM5_USART_ReadCallbackRegister(SERCOM_5_ReadCallback, 0);
    SERCOM5_USART_WriteCallbackRegister(SERCOM_5_WriteCallback, 0);
    
    uint8_t rxBuffer;

    // Read request for Extension 
    SERCOM1_USART_Read(&rxBuffer, RX_BUFFER_SIZE);
    
    // Read request for EDBG
    SERCOM5_USART_Read(&rxBuffer, RX_BUFFER_SIZE);
    while ( true )
    {
        if(SERCOM_1_readStatus == true)
        {
            SERCOM_1_readStatus = false;  
            
            //Transmit received bytes from EDBG
            SERCOM5_USART_Write(&rxBuffer, RX_BUFFER_SIZE);
        }

        if(SERCOM_5_writeStatus == true)
        {
            SERCOM_5_writeStatus = false;
            SERCOM1_USART_Read(&rxBuffer, RX_BUFFER_SIZE);
        }
        
        if(SERCOM_5_readStatus == true)
        {
            SERCOM_5_readStatus = false; 
            
            //Transmit received bytes from Extension
            SERCOM1_USART_Write(&rxBuffer, RX_BUFFER_SIZE);
        }
        
        if(SERCOM_1_writeStatus == true)
        {
            SERCOM_1_writeStatus = false;
            SERCOM5_USART_Read(&rxBuffer, RX_BUFFER_SIZE);
        }
        SYS_Tasks ( );
    }

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/
