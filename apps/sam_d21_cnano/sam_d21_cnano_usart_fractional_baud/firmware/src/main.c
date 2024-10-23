#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#define RX_BUFFER_SIZE 1  
#define FRAC_BAUD_RATE 11000
#define USART_SAMPLE_NUM 16

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
void APP_SERCOM_1_WriteCallback(uintptr_t context)
{
    writeStatus_SERCOM_1 = true;
}

void APP_SERCOM_1_ReadCallback(uintptr_t context)
{
    readStatus_SERCOM_1 = true;
}

void APP_SERCOM_5_WriteCallback(uintptr_t context)
{
    writeStatus_SERCOM_5 = true;
}

void APP_SERCOM_5_ReadCallback(uintptr_t context)
{
    readStatus_SERCOM_5 = true;
}

// Function to calculate the fractional baud value
void calculate_fractional_baud_value(const uint32_t baudrate, const uint32_t peripheral_clock, uint8_t sample_num)
{
    uint32_t mul_ratio;
    mul_ratio = (uint64_t)((uint64_t)peripheral_clock * (uint64_t)1000) / (uint64_t)(baudrate * sample_num);
    baud = mul_ratio / 1000;
    fp = ((mul_ratio - (baud * 1000)) * 8) / 1000;
}

// USART initialization with fractional baud rate settings
void ext_usart_init(void)
{
    calculate_fractional_baud_value(FRAC_BAUD_RATE, SERCOM1_USART_FrequencyGet(), USART_SAMPLE_NUM);
    
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
    ext_usart_init();
    
    // Register Read and Write Callbacks
    SERCOM1_USART_ReadCallbackRegister(APP_SERCOM_1_ReadCallback, 0);
    SERCOM1_USART_WriteCallbackRegister(APP_SERCOM_1_WriteCallback, 0);
    SERCOM5_USART_ReadCallbackRegister(APP_SERCOM_5_ReadCallback, 0);
    SERCOM5_USART_WriteCallbackRegister(APP_SERCOM_5_WriteCallback, 0);
    
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
