/* ************************************************************************** */
/** Flash 2 click example source file

  @Company
    Microchip Technology Inc.

  @File Name
    flash_2_example.c

  @Summary
    Flash 2 click routine example implementation File.

  @Description
    This file implements the usage of the Flash 2 click routine APIs.
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
#include "flash_2.h"
#include "flash_2_example.h"
#include <string.h>
/* TODO:  Include other files here if needed. */


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

uint32_t read_buf[BUFFER_SIZE];
uint32_t write_buf[BUFFER_SIZE];

void flash_2_example_initialize(void)
{   
    uint32_t i = 0;
    
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        write_buf[i] = i;
    }
    FLASH_2_Initialize();
}

void flash_2_example_tasks(void)
{    
    /* Flash write */
    FLASH_2_Write(write_buf, sizeof(write_buf), FLASH_MEM_ADDRESS);
    do
    {
        FLASH_2_Tasks();
    }while(FLASH_2_Transfer_Status() != true);
    
    /* Flash read */
    FLASH_2_Read(read_buf, sizeof(read_buf), FLASH_MEM_ADDRESS);
    do
    {
        FLASH_2_Tasks();
    }while(FLASH_2_Transfer_Status() != true);
    
    /* Checking the transmitted and received data */
    if(!(memcmp(write_buf,read_buf,BUFFER_SIZE)))
    {
        LED_Clear();
    }
    
    while(1);
}

/* *****************************************************************************
 End of File
 */
