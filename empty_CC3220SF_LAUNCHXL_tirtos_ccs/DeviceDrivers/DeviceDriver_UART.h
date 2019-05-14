/*
 * UART.h
 *
 *  Created on: Apr 22, 2019
 *      Author: expleo-user
 */

#ifndef DEVICEDRIVERS_DEVICEDRIVER_UART_H_
#define DEVICEDRIVERS_DEVICEDRIVER_UART_H_

#define USE_CC3220_ROM_DRV_API

#include "DeviceDriver_COMMON.h"
#include "DeviceDriver_UDMA.h"


typedef UARTCC32XX_HWAttrsV1 *DeviceDriver_UART_HwAttr;

typedef struct DeviceDriver_UART_HwInt_{
    Hwi_Handle  TIRTOSHwIntHandleVar;
    Hwi_Params  TIRTOSHwIntParamsVar;
    void (*CallbackFuncPtr)(uintptr_t);
}*DeviceDriver_UART_HwInt;

typedef struct DeviceDriver_UART_SwAttr_{
    bool Opened;
    unsigned int powerMngrId;
    bool enableCallback;
    bool enableTxuDMA;
    bool enableRxuDMA;
    DeviceDriver_Buffer_Handler rxBuffer;
    DeviceDriver_Buffer_Handler txBuffer;


}*DeviceDriver_UART_SwAttr;



typedef struct DeviceDriver_UART_Handle_{
    DeviceDriver_UART_HwAttr HwAttrPtr;
    DeviceDriver_UART_HwInt  HwIntPtr;
    DeviceDriver_UART_SwAttr SwAttrPtr;
}*DeviceDriver_UART_Handle;


enum StatusReturnCode DeviceDriver_UART_init(DeviceDriver_UART_Handle handle,DeviceDriver_Buffer_Handler rxbufffer,DeviceDriver_Buffer_Handler txbuffer);

enum StatusReturnCode DeviceDriver_UART_initHwAttr(DeviceDriver_UART_Handle handle, CC3220SF_LAUNCHXL_UARTName name);

enum StatusReturnCode DeviceDriver_UART_open(DeviceDriver_UART_Handle handle);

void DeviceDriver_UART_readEcho(DeviceDriver_UART_Handle handle);

enum StatusReturnCode DeviceDriver_UART_initCallbackFunc(DeviceDriver_UART_Handle);

enum StatusReturnCode DeviceDriver_UART_initHwInt(DeviceDriver_UART_Handle handle, uint32_t flags, uint32_t FIFOlevels);

enum StatusReturnCode DeviceDriver_UART_initUDMARxChAttr(DeviceDriver_UART_Handle handle, DeviceDriver_UDMA_ChannelHandle channel);

enum StatusReturnCode DeviceDriver_UART_initUDMATxChAttr(DeviceDriver_UART_Handle handle, DeviceDriver_UDMA_ChannelHandle channel);

enum StatusReturnCode DeviceDriver_UART_openUDMAChannel(DeviceDriver_UART_Handle handle, DeviceDriver_UDMA_ChannelHandle channel);

#endif /* DEVICEDRIVERS_DEVICEDRIVER_UART_H_ */
