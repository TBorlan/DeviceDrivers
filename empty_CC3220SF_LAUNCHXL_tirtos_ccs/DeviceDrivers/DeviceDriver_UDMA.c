/*
 * DeviceDriver_UDMA.c
 *
 *  Created on: May 7, 2019
 *      Author: expleo-user
 */

#ifndef DEVICEDRIVERS_DEVICEDRIVER_UDMA_C_
#define DEVICEDRIVERS_DEVICEDRIVER_UDMA_C_

#define USE_CC3220_ROM_DRV_API

#include "DeviceDriver_UDMA.h"

extern tDMAControlTable dmaControlTable;


enum StatusReturnCode DeviceDriver_UDMA_init(DeviceDriver_UDMA_Handle handle){

    Power_setDependency(PowerCC32XX_PERIPH_UDMA);

    MAP_uDMAEnable();

    MAP_uDMAControlBaseSet(handle->ControlTablePtr);

    handle->Enabled = true;

    return Return_OK;

}

enum StatusReturnCode DeviceDriver_UDMA_initHwAttr(DeviceDriver_UDMA_Handle handle){

    handle->Enabled = false;

    handle->ControlTablePtr = &dmaControlTable;

    return Return_OK;

}



#endif
