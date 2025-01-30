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

#include <stdio.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes

#define TEMP_SENSOR_SLAVE_ADDR                  0x004F
#define TEMP_SENSOR_REG_ADDR                    0x00

/* Timer Counter Time period match values for input clock of 1000 Hz */
#define PERIOD_500MS                            500
#define PERIOD_1S                               1000
#define PERIOD_2S                               2000
#define PERIOD_4S                               4000

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
static volatile bool isTC0TimerExpired = false;
static volatile bool changeTempSamplingRate = false;
static volatile bool isUSARTTxComplete = false;
static volatile bool isTemperatureRead = false;
static uint8_t temperatureVal;
static uint8_t i2cWrData = TEMP_SENSOR_REG_ADDR;
static uint8_t i2cRdData[2] = {0};
static uint8_t __attribute__ ((aligned(32))) uartTxBuffer[100] = {0};
typedef enum
{
    TEMP_SAMPLING_RATE_500MS = 0,
    TEMP_SAMPLING_RATE_1S = 1,
    TEMP_SAMPLING_RATE_2S = 2,
    TEMP_SAMPLING_RATE_4S = 3,
} TEMP_SAMPLING_RATE;
static TEMP_SAMPLING_RATE tempSampleRate = TEMP_SAMPLING_RATE_500MS;




static uint8_t getTemperature(uint8_t* rawTempValue)
{
    int16_t temp;
    // Convert the temperature value read from sensor to readable format (Degree Celsius)
    // For demonstration purpose, temperature value is assumed to be positive.
    // The maximum positive temperature measured by sensor is +125 C
    temp = (rawTempValue[0] << 8) | rawTempValue[1];
    temp = (temp >> 7) * 0.5;
    temp = (temp * 9/5) + 32;
    return (uint8_t)temp;
}

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );




    /* Start the timer channel 0*/

    while ( true )
    {
        if (isTC0TimerExpired == true)
        {

        }
        if (isTemperatureRead == true)
        {
            isTemperatureRead = false;
            if(changeTempSamplingRate == false)
            {

            }
            else
            {

            }
            /* Clean cache lines having source buffer before submitting a transfer
             * request to DMA to load the latest data in the cache to the actual
             * memory */

        }
    }
    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

