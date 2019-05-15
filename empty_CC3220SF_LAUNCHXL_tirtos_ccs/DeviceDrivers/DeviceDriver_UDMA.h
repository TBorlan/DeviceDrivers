/*
 * DeviceDriver_UDMA.h
 *
 *  Created on: May 7, 2019
 *      Author: expleo-user
 */

#ifndef DEVICEDRIVERS_DEVICEDRIVER_UDMA_H_
#define DEVICEDRIVERS_DEVICEDRIVER_UDMA_H_

#include "DeviceDriver_COMMON.h"



typedef struct DeviceDriver_UDMA_Handle_{
    tDMAControlTable *ControlTablePtr;
    bool Enabled;
}*DeviceDriver_UDMA_Handle;



typedef struct DeviceDriver_UDMA_ChannelAttr_{
    unsigned int ChannelID;
    unsigned int TransferMode;
    unsigned int *SourcePtr;
    unsigned int *DestinationPtr;
    unsigned int ArbitrationSize;
    unsigned int SourceIncr;
    unsigned int DestinationIncr;
    unsigned int DataSize;
    unsigned int TransferItemCount;
}DeviceDriver_UDMA_ChannelAttr,*DeviceDriver_UDMA_ChannelHandle;


enum StatusReturnCode DeviceDriver_UDMA_initHwAttr();

enum StatusReturnCode DeviceDriver_UDMA_init();







#endif /* DEVICEDRIVERS_DEVICEDRIVER_UDMA_H_ */
