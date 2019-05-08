/*
 * DeviceDriver_COMMON.h
 *
 *  Created on: May 8, 2019
 *      Author: expleo-user
 */

#ifndef DEVICEDRIVERS_DEVICEDRIVER_COMMON_H_
#define DEVICEDRIVERS_DEVICEDRIVER_COMMON_H_

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC32XX.h>
#include <ti/drivers/dma/UDMACC32XX.h>
#include <CC3220SF_LAUNCHXL.h>
#include <ti/drivers/power/PowerCC32XX.h>

#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/uart.h>
#include <ti/devices/cc32xx/driverlib/udma.h>

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Error.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum StatusReturnCode { Return_OK };

#endif /* DEVICEDRIVERS_DEVICEDRIVER_COMMON_H_ */
