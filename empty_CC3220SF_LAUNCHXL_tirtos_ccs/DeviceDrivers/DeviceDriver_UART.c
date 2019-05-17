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
extern DeviceDriver_UDMA_Handle udmaHandle;


//*****************************************************************************
//
//! Assigns a hardware structure to the handle
//!
//! \param handle to be initialized
//! \param name is one field of enum "CC3220SF_LAUNCHXL_UARTName"
//!
//! This function should be called prior to every other function that uses an UART handle.
//! It should not be used to reconfigure an already configured handle.
//!
//! \return Returns StatusReturnCode field member
//
//*****************************************************************************
enum StatusReturnCode DeviceDriver_UART_initHwAttr(DeviceDriver_UART_Handle handle, CC3220SF_LAUNCHXL_UARTName name){

    handle->HwAttrPtr = &uartCC3220SHWAttrs[name];

    return Return_OK;

}

//*****************************************************************************
//
//! Initialize UART specified by its handle
//!
//! \param handle to be initialized
//! \param rxbufffer to be used by UART
//! \param txbuffer to be used by UART
//!
//! Powers on an UART peripheral, assigns tx and rx buffers and muxex GPIO pins to UART
//!
//! \return Returns StatusReturnCode field member
//
//*****************************************************************************
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

//*****************************************************************************
//
//! Starts the UART Peripheral
//!
//! \param handle of UART peripheral
//!
//! Sets the UART clock for generating the required baudrate
//! and enables UART peripheral
//!
//! \return Returns StatusReturnCode field member
//
//*****************************************************************************
enum StatusReturnCode DeviceDriver_UART_open(DeviceDriver_UART_Handle handle){

    MAP_UARTIntClear(handle->HwAttrPtr->baseAddr, UART_INT_TX | UART_INT_RX |
            UART_INT_RT);
    MAP_UARTFlowControlSet(handle->HwAttrPtr->baseAddr, UART_FLOWCONTROL_NONE);

    uint32_t freq[2];

    BIOS_getCpuFreq((Types_FreqHz *)freq);

    MAP_UARTConfigSetExpClk(handle->HwAttrPtr->baseAddr, freq[1], 115200,
        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
        UART_CONFIG_PAR_NONE);

    unsigned int dmaFlags = 0;

    if(handle->SwAttrPtr->enableRxuDMA == true){
        dmaFlags = dmaFlags | UART_DMA_RX;
    }
    if(handle->SwAttrPtr->enableTxuDMA == true){
        dmaFlags = dmaFlags | UART_DMA_TX;
    }

    MAP_UARTDMAEnable(handle->HwAttrPtr->baseAddr, dmaFlags);


    MAP_UARTEnable(handle->HwAttrPtr->baseAddr);

    return Return_OK;

}

//*****************************************************************************
//
//! Creates a TI-RTOS Hardware Interrupt for UART
//!
//! \param handle which holds necessary information
//!
//! This function creates a TI-RTOS Hardware Interrupt using information pointed by the UART handle.
//! This function should be called prior to initializing the UART interrupt sources.
//! This function doesn't enable the TI-RTOS Hardware Interrupt, only creates it
//!
//! \return Returns StatusReturnCode field member
//
//*****************************************************************************
enum StatusReturnCode DeviceDriver_UART_initCallbackFunc(DeviceDriver_UART_Handle handle){

    Hwi_Params_init(&(handle->HwIntPtr->TIRTOSHwIntParamsVar));

    handle->HwIntPtr->TIRTOSHwIntParamsVar.arg = (uintptr_t)handle;

    handle->HwIntPtr->TIRTOSHwIntParamsVar.priority = handle->HwAttrPtr->intPriority;

    handle->HwIntPtr->TIRTOSHwIntParamsVar.enableInt = FALSE;

    handle->HwIntPtr->TIRTOSHwIntHandleVar = Hwi_create(handle->HwAttrPtr->intNum,handle->HwIntPtr->CallbackFuncPtr,&(handle->HwIntPtr->TIRTOSHwIntParamsVar),Error_IGNORE);

    handle->SwAttrPtr->enableCallback = true;

    return Return_OK;

}


//*****************************************************************************
//
//! Configures and enables interrput sources for a specific UART
//!
//! \param handle which points to the UART peripheral
//! \param flags is the logical OR of the interrupt sources
//! \param FIFOlevels is the logical OR of the FIFO thresholds for Rx and Tx FIFOs
//!
//! This function enables UART interrupt sources specified as ORed values from uart.h file.
//! It also sets the UARt FIFO level triggers passed as ORed values, as specified in uart.h.
//! This function should only be called after DeviceDriver_UART_initCallbackFunc.
//!
//! \return Returns StatusReturnCode field member
//
//*****************************************************************************
enum StatusReturnCode DeviceDriver_UART_initHwInt(DeviceDriver_UART_Handle handle, uint32_t flags, uint32_t FIFOlevels){


    MAP_UARTFIFOLevelSet(handle->HwAttrPtr->baseAddr,FIFOlevels & (uint32_t)(0x07),FIFOlevels & (uint32_t)(0x38));
    MAP_UARTIntEnable(handle->HwAttrPtr->baseAddr,flags);
    if(flags & UART_INT_DMATX){
        handle->SwAttrPtr->enableTxuDMA = true;
    }
    else{
        handle->SwAttrPtr->enableTxuDMA = false;
    }
    if(flags & UART_INT_DMARX){
        handle->SwAttrPtr->enableRxuDMA = true;
    }
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

//*****************************************************************************
//
//! Initialize a configuration structure for UDMA Rx channel
//!
//! \param handle which points to the UART peripheral
//! \param channel structure to be initialized
//!
//! The channel structure is configured as follows:
//! *Basic Transfer Mode
//! *Channel ID selected as per handle specification
//! *Destination and Transfer size specified as per Rx buffer bound to the handler
//! *Data size of 8 bits
//! *Arbitration size of 1 element
//!
//! \return Returns StatusReturnCode field member
//2) Answer or reject to the call => On HMI remain the screen with `incoming call process` (see marker 000 and 001)
//*****************************************************************************
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

//*****************************************************************************
//
//! Initialize a configuration structure for UDMA Tx channel
//!
//! \param handle which points to the UART peripheral
//! \param channel structure to be initialized
//!
//! The channel structure is configured as follows:
//! *Basic Transfer Mode
//! *Channel ID selected as per handle specification
//! *Source and Transfer size specified as per Rx buffer bound to the handler
//! *Data size of 8 bits
//! *Arbitration size of 1 element
//!
//! \return Returns StatusReturnCode field member
//
//*****************************************************************************
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

//*****************************************************************************
//
//! Configures and enables a uDMA channel for UART
//!
//! \param channel structure containing configuration information
//!
//! \return Returns StatusReturnCode field member
//
//*****************************************************************************
enum StatusReturnCode DeviceDriver_UART_openUDMAChannel(DeviceDriver_UDMA_ChannelHandle channel){

    if(!udmaHandle->Enabled){

        DeviceDriver_UDMA_init();

    }
    MAP_uDMAChannelAssign(channel->ChannelID);
    MAP_uDMAChannelControlSet(channel->ChannelID, channel->DataSize |
                                                  channel->ArbitrationSize |
                                                  channel->DestinationIncr |
                                                  channel->SourceIncr);

    MAP_uDMAChannelTransferSet(channel->ChannelID, channel->TransferMode,
                                               channel->SourcePtr,
                                               channel->DestinationPtr,
                                               channel->TransferItemCount);
    MAP_uDMAChannelEnable(channel->ChannelID);

    return Return_OK;

}



#ifdef DEVICEDRIVER_ENABLELOGGING

static char buffer[2][1024];

static volatile enum {Primary_Select, Alternate_Select} activeBuffer;

static const DeviceDriver_UDMA_ChannelAttr primaryChannel = { UDMA_CH9_UARTA0_TX | UDMA_PRI_SELECT,
                                                              UDMA_MODE_AUTO,
                                                              primaryBuffer,
                                                              UARTA0_BASE + UART_O_DR,
                                                              UDMA_ARB_1,
                                                              UDMA_SRC_INC_8,
                                                              UDMA_DST_INC_NONE,
                                                              UDMA_SIZE_8,
                                                              1023
                                                            };

static const DeviceDriver_UDMA_ChannelAttr alternateChannel = { UDMA_CH9_UARTA0_TX | UDMA_ATTR_ALTSELECT,
                                                              UDMA_MODE_AUTO,
                                                              alternativeBuffer,
                                                              UARTA0_BASE + UART_O_DR,
                                                              UDMA_ARB_1,
                                                              UDMA_SRC_INC_8,
                                                              UDMA_DST_INC_NONE,
                                                              UDMA_SIZE_8,
                                                              1023
                                                            };

enum StatusReturnCode DeviceDriver_UART_setupLogging(){

    for (int i = 0; i < 1024; i++){
        primaryBuffer[i] = 0;
    }
    activeBuffer = Primary_Select;

    return Return_OK;

}

void void DeviceDriver_Log(char tag[], char message[]){
    static unsigned short len;
    if (buffer[activeBuffer][0] == 0){
        len = 0;
    }
    int i = 0;
    int j = 0;
    while(tag[i] != 0 && message[j] != 0){
        if(tag[i] != 0){
            i++;
        }
        if(message[j] != 0){
            j++;
        }
    }
    unsigned short newlen = i + j + 1;
    if(1024 - len > newlen){
        buffer[activeBuffer][len] = '/r';
        buffer[activeBuffer][len +1] = '/n';
        len = len + 2;
        for(int k = 0; k < i -1; k++){
            buffer[activeBuffer][len + k] = tag[k];
        }
        buffer[activeBuffer][len + k + 1] = '/t';
        len = len + i;


    }



}


#endif



#endif


