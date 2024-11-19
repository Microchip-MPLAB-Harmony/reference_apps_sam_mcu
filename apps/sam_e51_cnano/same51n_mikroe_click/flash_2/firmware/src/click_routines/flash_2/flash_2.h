/* ************************************************************************** */
/** Flash 2 Click source file

  @Company
    Microchip Technology Inc.

  @File Name
    flash_2.h

  @Summary
    Flash 2 click routine declaration File.

  @Description
    This file declares the interface to the Flash 2 click using SPI PLIB.
 *  This click routine provides to read the sensor data on Flash 2 click.
 */
/* ************************************************************************** */

// DOM-IGNORE-BEGIN
/*
    (c) 2024 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/
// DOM-IGNORE-END


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */

#ifndef FLASH_2_H    /* Guard against multiple inclusion */
#define flash_2_H

#include <stdlib.h>
#include "configuration.h"
#include "driver/sst26/drv_sst26.h"
#include "definitions.h"

#define BUFFER_SIZE           16384
#define FLASH_MEM_ADDRESS     0x0

typedef enum
{
    /* Application's state machine's initial state. */
    FLASH_2_STATE_INIT,

    /* Open the Driver */
    FLASH_2_STATE_OPEN_DRIVER,

    /* Get Device Geometry */
    FLASH_2_STATE_GEOMETRY_GET,

    /* Erase Flash */
    FLASH_2_STATE_ERASE_FLASH,

    /* Erase Wait */
    FLASH_2_STATE_ERASE_WAIT,

    /* Write to Memory */
    FLASH_2_STATE_WRITE_MEMORY,

    /* Write Wait */
    FLASH_2_STATE_WRITE_WAIT,

    /* Read From Memory */
    FLASH_2_STATE_READ_MEMORY,

    /* Read Wait */
    FLASH_2_STATE_READ_WAIT,

    /* Verify Data Read */
    FLASH_2_STATE_VERIFY_DATA,

    /* The app idles */
    FLASH_2_STATE_SUCCESS,

    /* An app error has occurred */
    FLASH_2_STATE_ERROR,
            
    FLASH_2_STATE_IDLE

          
} FLASH_2_STATES;

typedef struct
{
    /* The application's current state */
    FLASH_2_STATES  state;

    /* Driver Handle */
    DRV_HANDLE handle;

    /* SST26 Device Geometry */
    DRV_SST26_GEOMETRY geometry;

    /* Jedec-ID*/
    uint32_t jedec_id;
       
    /* Memory Address */
    uint8_t mem_addr;
    
    /* Buffer size */
    uint8_t buf_size;
    
    /* Buffer Address */
    uint32_t *buffer;
    
    /* Device Status */
    volatile bool readWriteStatus;
    
    /* The application's transfer status */
    volatile bool transferDone;
    
} FLASH_2_DATA;


//FLASH_2_DATA flash2Data;
void FLASH_2_Initialize(void);
void FLASH_2_Tasks(void);

bool FLASH_2_Transfer_Status (void);

void FLASH_2_Write(uint32_t *write_buffer, uint32_t buffer_size, uint32_t mem_address);
void FLASH_2_Read(uint32_t *read_buffer, uint32_t buffer_size, uint32_t mem_address);

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
