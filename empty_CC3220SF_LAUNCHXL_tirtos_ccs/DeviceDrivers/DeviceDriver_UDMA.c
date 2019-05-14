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

static tDMAControlTable dmaControlTable[64] __attribute__ ((aligned (1024)));

struct DeviceDriver_UDMA_Handle_ udmaStruct = {NULL , false};

DeviceDriver_UDMA_Handle udmaHandle = &udmaStruct;

enum StatusReturnCode DeviceDriver_UDMA_init(){

    Power_setDependency(PowerCC32XX_PERIPH_UDMA);

    MAP_uDMAEnable();

    MAP_uDMAControlBaseSet(udmaHandle->ControlTablePtr);

    udmaHandle->Enabled = true;

    return Return_OK;

}

enum StatusReturnCode DeviceDriver_UDMA_initHwAttr(){

    udmaHandle->Enabled = false;

    udmaHandle->ControlTablePtr = &dmaControlTable;

    return Return_OK;

}



#endif
