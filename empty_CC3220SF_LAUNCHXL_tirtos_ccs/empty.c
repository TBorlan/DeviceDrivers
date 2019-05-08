/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */

/* For usleep() */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>

/* Board Header file */
#include "Board.h"

#include <DeviceDrivers/DeviceDriver_UART.h>

#include <CC3220SF_LAUNCHXL.h>



void UART0ISR(uintptr_t arg);



/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0){

    struct DeviceDriver_UART_Handle_ UART_HANDLE_STRUCT;

    struct DeviceDriver_UART_HwInt_ UART_HWINT_STRUCT;

    struct DeviceDriver_UART_SwAttr_ UART_SWATTR_STRUCT;

    DeviceDriver_UART_Handle UART0_Handle = &UART_HANDLE_STRUCT;

    UART0_Handle->HwIntPtr = &UART_HWINT_STRUCT;

    UART0_Handle->SwAttrPtr = &UART_SWATTR_STRUCT;

    UART0_Handle->HwIntPtr->CallbackFuncPtr = UART0ISR;

    DeviceDriver_UART_initHwAttr(UART0_Handle, CC3220SF_LAUNCHXL_UART0);


    DeviceDriver_UART_initCallbackFunc(UART0_Handle);

    DeviceDriver_UART_open(UART0_Handle);

    DeviceDriver_UART_initHwInt(UART0_Handle, UART_INT_RX, UART_FIFO_RX1_8|UART_FIFO_TX7_8);

    while(1){
        //DeviceDriver_UART_readEcho(UART0_Handle);
    }

}


void UART0ISR(uintptr_t arg){

    DeviceDriver_UART_Handle UART0_Handle = (DeviceDriver_UART_Handle)arg;
    uint32_t intStatus = MAP_UARTIntStatus(UART0_Handle->HwAttrPtr->baseAddr, true);
    MAP_UARTIntClear(UART0_Handle->HwAttrPtr->baseAddr, intStatus);
    if (intStatus & UART_INT_RX){
        char RX;
        RX = (char)(MAP_UARTCharGetNonBlocking(UART0_Handle->HwAttrPtr->baseAddr));
        MAP_UARTCharPutNonBlocking(UART0_Handle->HwAttrPtr->baseAddr, RX);
    }

    return;
}

