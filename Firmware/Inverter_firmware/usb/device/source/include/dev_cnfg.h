#ifndef __usbdev_cnfg_h__
#define __usbdev_cnfg_h__

#include "mqx.h"
#include "bsp.h"        // bsp.h include user_config.h, too

#ifndef USBCFG_DEFAULT_DEVICE_CONTROLLER
#define USBCFG_DEFAULT_DEVICE_CONTROLLER  (0)
#endif

#ifndef USBCFG_MAX_DRIVERS
extern const pointer _bsp_usb_table[];
#define USBCFG_MAX_DRIVERS                (sizeof(_bsp_usb_table) / sizeof(_bsp_usb_table[0]))
#endif

#endif
