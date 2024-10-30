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

uint16_t baud;
uint8_t fp;

volatile bool readStatus_SERCOM_1 = false;
volatile bool writeStatus_SERCOM_1 = false;

volatile bool readStatus_SERCOM_5 = false;
volatile bool writeStatus_SERCOM_5 = false;

// Callback functions to set the status flags
//SERCOM 1 - Extension
//SERCOM 5 - EDBG

void SERCOM_1_WriteCallback(uintptr_t context)
{
    writeStatus_SERCOM_1 = true;
}

void SERCOM_1_ReadCallback(uintptr_t context)
{
    readStatus_SERCOM_1 = true;
}

void SERCOM_5_WriteCallback(uintptr_t context)
{
    writeStatus_SERCOM_5 = true;
}

void SERCOM_5_ReadCallback(uintptr_t context)
{
    readStatus_SERCOM_5 = true;
}

// Function to calculate the fractional baud value
void SERCOM_5_Fractional_Baud(const uint32_t baudrate, const uint32_t peripheral_clock, uint8_t sample_num)
{
    uint32_t mul_ratio;
    mul_ratio = (uint64_t)((uint64_t)peripheral_clock * (uint64_t)BAUD_DIVISOR) / (uint64_t)(baudrate * sample_num);
    baud = mul_ratio / BAUD_DIVISOR;
    fp = ((mul_ratio - (baud * BAUD_DIVISOR)) * BAUD_FRACTION_MULT) / BAUD_DIVISOR;
}

// USART initialization with fractional baud rate settings
void SERCOM_5_USART_Init(void)
{
    SERCOM_5_Fractional_Baud(FRAC_BAUD_RATE, SERCOM1_USART_FrequencyGet(), USART_SAMPLE_NUM);
    
    SERCOM1_USART_Disable();
    SERCOM1_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_SAMPR(1UL);
    SERCOM1_REGS->USART_INT.SERCOM_BAUD = SERCOM_USART_INT_BAUD_FRAC_BAUD(baud) | SERCOM_USART_INT_BAUD_FRAC_FP(fp);
    SERCOM1_USART_Enable();
}

int main ( void )
{
    // Initialize all modules
    SYS_Initialize(NULL);
    
    // Initialize external USART
    SERCOM_5_USART_Init();
    
    // Register Read and Write Callbacks
    SERCOM1_USART_ReadCallbackRegister(SERCOM_1_ReadCallback, 0);
    SERCOM1_USART_WriteCallbackRegister(SERCOM_1_WriteCallback, 0);
    SERCOM5_USART_ReadCallbackRegister(SERCOM_5_ReadCallback, 0);
    SERCOM5_USART_WriteCallbackRegister(SERCOM_5_WriteCallback, 0);
    
    uint8_t rxBuffer; 
    
    // Initial Read Requests
    SERCOM1_USART_Read(&rxBuffer, RX_BUFFER_SIZE);
    SERCOM5_USART_Read(&rxBuffer, RX_BUFFER_SIZE);

    while (true)
    {
        // Check if SERCOM1 read is complete
        if (readStatus_SERCOM_1 == true)
        {
            readStatus_SERCOM_1 = false;
            // Transmit received bytes from SERCOM1 to SERCOM5
            SERCOM5_USART_Write(&rxBuffer, RX_BUFFER_SIZE);
        }
        // Check if SERCOM5 write is complete
        if (writeStatus_SERCOM_5 == true)
        {
            writeStatus_SERCOM_5 = false;
            // Request next read from SERCOM1
            SERCOM1_USART_Read(&rxBuffer, RX_BUFFER_SIZE);
        }
        // Check if SERCOM5 read is complete
        if (readStatus_SERCOM_5 == true)
        {
            readStatus_SERCOM_5 = false;
            // Transmit received bytes from SERCOM5 to SERCOM1
            SERCOM1_USART_Write(&rxBuffer, RX_BUFFER_SIZE);
        }
        // Check if SERCOM1 write is complete
        if (writeStatus_SERCOM_1 == true)
        {
            writeStatus_SERCOM_1 = false;
            // Request next read from SERCOM5
            SERCOM5_USART_Read(&rxBuffer, RX_BUFFER_SIZE);
        }
        
        // Perform system tasks
        SYS_Tasks();
    }

    return (EXIT_FAILURE);
}
