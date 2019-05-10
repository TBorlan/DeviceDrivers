/*
 * UART.c
 *
 *  Created on: Apr 22, 2019
 *      Author: expleo-user
 */

#ifndef DEVICEDRIVERS_DEVICEDRIVER_UART_C_
#define DEVICEDRIVERS_DEVICEDRIVER_UART_C_

#define USE_CC3220_ROM_DRV_API

#include "DeviceDriver_UART.h"

extern UARTCC32XX_HWAttrsV1 uartCC3220SHWAttrs[CC3220SF_LAUNCHXL_UARTCOUNT];






enum StatusReturnCode DeviceDriver_UART_initHwAttr(DeviceDriver_UART_Handle handle, CC3220SF_LAUNCHXL_UARTName name){

    handle->HwAttrPtr = &uartCC3220SHWAttrs[name];

    return Return_OK;

}

enum StatusReturnCode DeviceDriver_UART_init(DeviceDriver_UART_Handle handle,DeviceDriver_Buffer_Handler rxbufffer,DeviceDriver_Buffer_Handler txbuffer){

    // enable power
    switch(handle->HwAttrPtr->baseAddr){
        case UARTA0_BASE:
            handle->SwAttrPtr->powerMngrId = PowerCC32XX_PERIPH_UARTA0;
        case UARTA1_BASE:
            handle->SwAttrPtr->powerMngrId = PowerCC32XX_PERIPH_UARTA1;
        default:
            handle->SwAttrPtr->powerMngrId = -1;
    }
    Power_setDependency(handle->SwAttrPtr->powerMngrId);



    handle->SwAttrPtr->rxBuffer = rxbufffer;
    handle->SwAttrPtr->txBuffer = txbuffer;


    // mux pins for UART
    uint8_t pin = handle->HwAttrPtr->rxPin & 0x00ff;
    uint8_t mode = ((handle->HwAttrPtr->rxPin) & 0xff00) >> 8 ;
    MAP_PinTypeUART((unsigned long)pin, (unsigned long)mode);
    pin = handle->HwAttrPtr->txPin & 0x00ff;
    mode = ((handle->HwAttrPtr->txPin) & 0xff00) >> 8 ;
    MAP_PinTypeUART((unsigned long)pin, (unsigned long)mode);

    return Return_OK;

}

enum StatusReturnCode DeviceDriver_UART_open(DeviceDriver_UART_Handle handle){

    MAP_UARTIntClear(handle->HwAttrPtr->baseAddr, UART_INT_TX | UART_INT_RX |
            UART_INT_RT);
    MAP_UARTFlowControlSet(handle->HwAttrPtr->baseAddr, UART_FLOWCONTROL_NONE);

    uint32_t freq[2];

    BIOS_getCpuFreq((Types_FreqHz *)freq);

    MAP_UARTConfigSetExpClk(handle->HwAttrPtr->baseAddr, freq[1], 115200,
        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
        UART_CONFIG_PAR_NONE);

    MAP_UARTEnable(handle->HwAttrPtr->baseAddr);

    return Return_OK;

}

enum StatusReturnCode DeviceDriver_UART_initCallbackFunc(DeviceDriver_UART_Handle handle){

    Hwi_Params_init(&(handle->HwIntPtr->TIRTOSHwIntParamsVar));

    handle->HwIntPtr->TIRTOSHwIntParamsVar.arg = (uintptr_t)handle;

    handle->HwIntPtr->TIRTOSHwIntParamsVar.priority = handle->HwAttrPtr->intPriority;

    handle->HwIntPtr->TIRTOSHwIntParamsVar.enableInt = FALSE;

    handle->HwIntPtr->TIRTOSHwIntHandleVar = Hwi_create(handle->HwAttrPtr->intNum,handle->HwIntPtr->CallbackFuncPtr,&(handle->HwIntPtr->TIRTOSHwIntParamsVar),Error_IGNORE);

    handle->SwAttrPtr->enableCallback = true;

    return Return_OK;

}



enum StatusReturnCode DeviceDriver_UART_initHwInt(DeviceDriver_UART_Handle handle, uint32_t flags, uint32_t FIFOlevels){

    MAP_UARTFIFOLevelSet(handle->HwAttrPtr->baseAddr,FIFOlevels & (uint32_t)(0x07),FIFOlevels & (uint32_t)(0x38));
    MAP_UARTIntEnable(handle->HwAttrPtr->baseAddr,flags);
    Hwi_enableInterrupt(handle->HwAttrPtr->intNum);

    return Return_OK;
}




void DeviceDriver_UART_readEcho(DeviceDriver_UART_Handle handle){
    long character;
    character =  MAP_UARTCharGet(handle->HwAttrPtr->baseAddr);

    MAP_UARTCharPutNonBlocking(handle->HwAttrPtr->baseAddr, '\r');
    MAP_UARTCharPutNonBlocking(handle->HwAttrPtr->baseAddr, '\n');
    MAP_UARTCharPutNonBlocking(handle->HwAttrPtr->baseAddr, (char) character + 1);
}

enum StatusReturnCode DeviceDriver_UART_initUDMARxChAttr(DeviceDriver_UART_Handle handle, DeviceDriver_UDMA_ChannelHandle channel){

    if(handle->HwAttrPtr->baseAddr == UARTA0_BASE){
        channel->ChannelID = UDMA_CH8_UARTA0_RX;
    }
    else{
        channel->ChannelID = UDMA_CH10_UARTA1_RX;
    }

    channel->TransferMode = UDMA_MODE_BASIC;

    channel->DataSize = UDMA_SIZE_8;

    channel->SourceIncr = UDMA_SRC_INC_NONE;

    channel->ArbitrationSize = UDMA_ARB_1;

    channel->DestinationIncr = UDMA_DST_INC_8;

    channel->SourcePtr = (void *)(handle->HwAttrPtr->baseAddr + UART_O_DR);

    channel->DestinationPtr = handle->SwAttrPtr->rxBuffer->buffer;

    channel->TransferItemCount = handle->SwAttrPtr->rxBuffer->size;

    return Return_OK;

}

enum StatusReturnCode DeviceDriver_UART_initUDMATxChAttr(DeviceDriver_UART_Handle handle, DeviceDriver_UDMA_ChannelHandle channel){

    if(handle->HwAttrPtr->baseAddr == UARTA0_BASE){
        channel->ChannelID = UDMA_CH9_UARTA0_TX;
    }
    else{
        channel->ChannelID = UDMA_CH11_UARTA1_TX;
    }

    channel->TransferMode = UDMA_MODE_BASIC;

    channel->DataSize = UDMA_SIZE_8;

    channel->ArbitrationSize = UDMA_ARB_1;

    channel->DestinationIncr = UDMA_DST_INC_NONE;

    channel->SourceIncr = UDMA_SRC_INC_8;

    channel->SourcePtr = handle->SwAttrPtr->txBuffer->buffer;

    channel->DestinationPtr = (void *)(handle->HwAttrPtr->baseAddr + UART_O_DR);

    channel->TransferItemCount = handle->SwAttrPtr->txBuffer->size;

    return Return_OK;

}





#endif
