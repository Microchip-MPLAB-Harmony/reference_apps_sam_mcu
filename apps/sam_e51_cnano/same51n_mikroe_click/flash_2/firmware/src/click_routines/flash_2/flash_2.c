/* ************************************************************************** */
/** Flash 2 Click source file

  @Company
    Microchip Technology Inc.

  @File Name
    flash_2.c

  @Summary
    Flash 2 click routine Implementation File. 

  @Description
    This file defines the interface to the Flash 2 click using SPI PLIB.
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

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */
/** Descriptive Data Item Name

  @Summary
    Brief one-line summary of the data item.
    
  @Description
    Full description, explaining the purpose and usage of data item.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
  @Remarks
    Any additional remarks
 */
#include <string.h>
#include "flash_2.h"

static uint32_t erase_index = 0;
static uint32_t write_index = 0;

FLASH_2_DATA  flash2Data;

void FLASH_2_Initialize ( void )
{
    /* Place the App state machine in its initial state.*/ 
    flash2Data.state = FLASH_2_STATE_IDLE;
}

void FLASH_2_Read(uint32_t *read_buffer, uint32_t buffer_size, uint32_t mem_address)
{
    flash2Data.mem_addr = mem_address;
    flash2Data.buf_size = buffer_size;
    flash2Data.buffer = read_buffer;
    flash2Data.readWriteStatus = false;
    
    flash2Data.state = FLASH_2_STATE_INIT;
}

void FLASH_2_Write(uint32_t *write_buffer, uint32_t buffer_size, uint32_t mem_address)
{ 
    flash2Data.mem_addr = mem_address;
    flash2Data.buf_size = buffer_size;
    flash2Data.buffer = write_buffer;
    flash2Data.readWriteStatus = true;
    
    flash2Data.state = FLASH_2_STATE_INIT;
}

bool FLASH_2_Transfer_Status (void)
{
    return flash2Data.transferDone;
}

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */
void FLASH_2_Tasks ( void )
{
    DRV_SST26_TRANSFER_STATUS transferStatus = DRV_SST26_TRANSFER_ERROR_UNKNOWN;

    /* Check the application's current state. */
    switch ( flash2Data.state )
    {
        /* Application's initial state. */
        case FLASH_2_STATE_INIT:
        {
            if (DRV_SST26_Status(DRV_SST26_INDEX) == SYS_STATUS_READY)
            {
                flash2Data.state = FLASH_2_STATE_OPEN_DRIVER;
                flash2Data.transferDone = false;
            }
            break;
        }
        /* Opens the driver */
        case FLASH_2_STATE_OPEN_DRIVER:
        {
            flash2Data.handle = DRV_SST26_Open(DRV_SST26_INDEX, DRV_IO_INTENT_READWRITE);
            if (flash2Data.handle != DRV_HANDLE_INVALID)
            {
                flash2Data.state = FLASH_2_STATE_GEOMETRY_GET;
            }
            break;
        }
        /* Geometry of the device */
        case FLASH_2_STATE_GEOMETRY_GET:
        {
            if (DRV_SST26_GeometryGet(flash2Data.handle, &flash2Data.geometry) != true)
            {    
                flash2Data.state = FLASH_2_STATE_ERROR;
                break;
            }
            
            if(flash2Data.readWriteStatus == true)
            /* After successful geometry retrieval, the state moves to write operation */
            {
                erase_index = 0;
                write_index = 0;
                flash2Data.state = FLASH_2_STATE_ERASE_FLASH;
            }
            else
            {
                flash2Data.state = FLASH_2_STATE_READ_MEMORY;
            }

            break;
        }
        /* Flash erase */
        case FLASH_2_STATE_ERASE_FLASH:
        {
            if (DRV_SST26_SectorErase(flash2Data.handle, (FLASH_MEM_ADDRESS + erase_index)) != true)
            {                
                flash2Data.state = FLASH_2_STATE_ERROR;
            }
            flash2Data.state = FLASH_2_STATE_ERASE_WAIT;

            break;
        }
        /* Flash erase wait */
        case FLASH_2_STATE_ERASE_WAIT:
        {
            transferStatus = DRV_SST26_TransferStatusGet(flash2Data.handle);

            if(transferStatus == DRV_SST26_TRANSFER_COMPLETED)
            {
                erase_index += flash2Data.geometry.erase_blockSize;

                if (erase_index < BUFFER_SIZE)
                {
                    flash2Data.state = FLASH_2_STATE_ERASE_FLASH;
                }
                else
                {
                    flash2Data.state = FLASH_2_STATE_WRITE_MEMORY;
                }
            }
            else if (transferStatus == DRV_SST26_TRANSFER_ERROR_UNKNOWN)
            {
                flash2Data.state = FLASH_2_STATE_ERROR;
            }
            break;
        }
        /* Flash write */
        case FLASH_2_STATE_WRITE_MEMORY:
        {
            if (DRV_SST26_PageWrite(flash2Data.handle,(void *)flash2Data.buffer + write_index, (FLASH_MEM_ADDRESS + write_index)) != true)
            {
                flash2Data.state = FLASH_2_STATE_ERROR;
                break;
            }
            flash2Data.state = FLASH_2_STATE_WRITE_WAIT;
            break;
        }
        /* Flash write write */
        case FLASH_2_STATE_WRITE_WAIT:
        {
            transferStatus = DRV_SST26_TransferStatusGet(flash2Data.handle);

            if(transferStatus == DRV_SST26_TRANSFER_COMPLETED)
            {
                write_index += flash2Data.geometry.write_blockSize;

                if (write_index < BUFFER_SIZE)
                {
                    flash2Data.state = FLASH_2_STATE_WRITE_MEMORY;
                }
                else
                {
                    flash2Data.state = FLASH_2_STATE_SUCCESS;
                }
            }
            else if (transferStatus == DRV_SST26_TRANSFER_ERROR_UNKNOWN)
            {
                flash2Data.state = FLASH_2_STATE_ERROR;
            }
            break;
        }
        /* Flash read */
        case FLASH_2_STATE_READ_MEMORY:
        {
            if (DRV_SST26_Read(flash2Data.handle, (void *)flash2Data.buffer, BUFFER_SIZE, FLASH_MEM_ADDRESS) != true)
            {
                flash2Data.state = FLASH_2_STATE_ERROR;
            }
            else
            {
                flash2Data.state = FLASH_2_STATE_READ_WAIT;
            }

            break;
        }
        /* Flash read wait*/
        case FLASH_2_STATE_READ_WAIT:
        {
            transferStatus = DRV_SST26_TransferStatusGet(flash2Data.handle);

            if(transferStatus == DRV_SST26_TRANSFER_COMPLETED)
            {
                flash2Data.state = FLASH_2_STATE_SUCCESS;
            }
            else if (transferStatus == DRV_SST26_TRANSFER_ERROR_UNKNOWN)
            {
                flash2Data.state = FLASH_2_STATE_ERROR;
            }

            break;
        }
        /* Successful transfer*/
        case FLASH_2_STATE_SUCCESS:
        {
            DRV_SST26_Close(flash2Data.handle);
            flash2Data.state = FLASH_2_STATE_IDLE;
            flash2Data.transferDone = true;
            break;
        }       
        /* Unsuccessful transfer */
        case FLASH_2_STATE_ERROR:
        {
            DRV_SST26_Close(flash2Data.handle);
            
            flash2Data.state = FLASH_2_STATE_IDLE;
            break;
        }       
        default:
        case FLASH_2_STATE_IDLE:
        {
            break;
        }     
    }
}

/* *****************************************************************************
 End of File
 */
