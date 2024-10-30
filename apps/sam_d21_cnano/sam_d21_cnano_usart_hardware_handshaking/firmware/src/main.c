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

uint8_t rxBuffer;
uint8_t txBuffer = 0xAA;

volatile bool readStatus_SERCOM_1= false;
volatile bool writeStatus_SERCOM_1 = false;

volatile bool readStatus_SERCOM_5 = false;
volatile bool writeStatus_SERCOM_5 = false;

//SERCOM 1 - Extension

void SERCOM_1_WriteCallback(uintptr_t context)
{
    SERCOM1_USART_Write(&txBuffer, TX_BUFFER_SIZE);  
}

void SERCOM_1_ReadCallback(uintptr_t context)
{   
    readStatus_SERCOM_1 = true;
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    SYSTICK_TimerStart();
    
    // Extension SERCOM Read and Write Callback
    SERCOM1_USART_WriteCallbackRegister(SERCOM_1_WriteCallback, 0);
    SERCOM1_USART_ReadCallbackRegister(SERCOM_1_ReadCallback, 0);
    
    // Read request for Extension
    SERCOM1_USART_Write(&txBuffer, TX_BUFFER_SIZE);
    SERCOM1_USART_Read(&rxBuffer, RX_BUFFER_SIZE);
        
    while ( true )
    { 
        SYSTICK_DelayMs(1U);
        
        if(readStatus_SERCOM_1 == true)
        {
            readStatus_SERCOM_1 = false;
            SERCOM1_USART_Read(&rxBuffer, RX_BUFFER_SIZE);
        }
          
        SYS_Tasks ( );
    }

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/
