#ifndef __usbhost_cnfg_h__
#define __usbhost_cnfg_h__

#include <bsp.h>        // also includes user_config.h

/** MGCT: <category name="USB Host Settings"> */

#ifndef USBCFG_REGISTER_ENDIANNESS
#define USBCFG_REGISTER_ENDIANNESS MQX_BIG_ENDIAN
#endif

#ifndef USBCFG_MEMORY_ENDIANNESS
#define USBCFG_MEMORY_ENDIANNESS MQX_BIG_ENDIAN
#endif

/*
** Maximum number of pipes.
** MGCT: <option type="number"/>
*/  
#ifndef USBCFG_MAX_PIPES
    #ifdef USBCFG_KHCI
    #define USBCFG_MAX_PIPES                   (16)
    #else
        #define USBCFG_MAX_PIPES               (24)
    #endif
#endif

/*
** Maximum number of usb interfaces.
** MGCT: <option type="number"/>
*/  
#ifndef USBCFG_MAX_INTERFACE
#define USBCFG_MAX_INTERFACE                    (1) //deprecated, should be removed (TODO), stays here for comaptibility
#endif

#ifndef USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS
#define USBCFG_HOST_NUM_ISO_PACKET_DESCRIPTORS  (0)
#endif

#ifndef USBCFG_MAX_DRIVERS
extern const pointer _bsp_usb_table[];
#define USBCFG_MAX_DRIVERS                      (sizeof(_bsp_usb_table) / sizeof(_bsp_usb_table[0]))
#endif

/*
** MGCT: <option type="number"/>
*/  
#ifndef USBCFG_DEFAULT_MAX_NAK_COUNT
#define USBCFG_DEFAULT_MAX_NAK_COUNT            (15)
#endif

/*
** MGCT: <option type="string"/>
*/  
#ifndef USBCFG_MFS_OPEN_READ_CAPACITY_RETRY_DELAY
#define USBCFG_MFS_OPEN_READ_CAPACITY_RETRY_DELAY   (_time_get_ticks_per_sec() * 1)
#endif

/*
** MGCT: <option type="string"/>
*/  
#ifndef USBCFG_MFS_LWSEM_TIMEOUT
#define USBCFG_MFS_LWSEM_TIMEOUT                    (_time_get_ticks_per_sec() * 20)
#endif

/*
** MGCT: <option type="number"/>
*/  
#ifndef USBCFG_MFS_MAX_RETRIES
#define USBCFG_MFS_MAX_RETRIES                  (5)
#endif

/*
** MGCT: <option type="number"/>
*/  
#ifndef USBCFG_CTRL_RETRY
#define USBCFG_CTRL_RETRY                       (4)
#endif

/*
** Maximum number of selected interfaces (= with open pipes).
** MGCT: <option type="number"/>
*/
#ifndef USBCFG_MAX_DEV_INTFS
#define USBCFG_MAX_DEV_INTFS                    (2)
#endif

/** MGCT: </category> */

#ifdef USBCFG_KHCI

    #define __USB_HOST_KHCI__

    /** MGCT: <category name="KHCI Driver Specific Settings"> */
    
    /*
    ** KHCI task priority
    ** MGCT: <option type="number"/>
    */  
    #ifndef USBCFG_KHCI_TASK_PRIORITY
    #define USBCFG_KHCI_TASK_PRIORITY               (8)
    #endif

    /*
    ** Wait time in tick for events - must be above 1.
    ** MGCT: <option type="string"/>
    */
    #ifndef USBCFG_KHCI_WAIT_TICK
    #define USBCFG_KHCI_WAIT_TICK                   (_time_get_ticks_per_sec() / 10)    // 100ms
    #endif

    /*
    ** Maximum message count for KHCI.
    ** MGCT: <option type="number"/>
    */
    #ifndef USBCFG_KHCI_TR_QUE_MSG_CNT
    #define USBCFG_KHCI_TR_QUE_MSG_CNT              (10)
    #endif

    /*
    ** Maximum interrupt transaction count.
    ** MGCT: <option type="number"/>
    */
    #ifndef USBCFG_KHCI_MAX_INT_TR
    #define USBCFG_KHCI_MAX_INT_TR                  (10)
    #endif
    
    /** MGCT: </category> */

#endif // USBCFG_KHCI

#ifdef USBCFG_EHCI

    
    /** MGCT: <category name="EHCI Driver Specific Settings"> */
    /*
    ** MGCT: <option type="number"/>
    */
    #ifndef USBCFG_EHCI_USE_SW_TOGGLING
    #define USBCFG_EHCI_USE_SW_TOGGLING         (0)
    #endif

    /*
    ** Maximum number of allocated QH descriptors.
    ** MGCT: <option type="number"/>
    */
    #ifndef USBCFG_EHCI_MAX_QH_DESCRS
    #define USBCFG_EHCI_MAX_QH_DESCRS           (8)
    #endif

    /*
    ** Maximum number of allocated QTD descriptors.
    ** MGCT: <option type="number"/>
    */
    #ifndef USBCFG_EHCI_MAX_QTD_DESCRS
    #define USBCFG_EHCI_MAX_QTD_DESCRS          (16)
    #endif

    /*
    ** Maximum number of allocated ISO descriptors.
    ** MGCT: <option type="number"/>
    */
    #ifndef USBCFG_EHCI_MAX_ITD_DESCRS
    #define USBCFG_EHCI_MAX_ITD_DESCRS          (16)
    #endif

    /*
    ** Maximum number of allocated SSPLIT descriptors.
    ** MGCT: <option type="number"/>
    */
    #ifndef USBCFG_EHCI_MAX_SITD_DESCRS
    #define USBCFG_EHCI_MAX_SITD_DESCRS         (32)
    #endif

    /*
    ** MGCT: <option type="number"/>
    */
    #ifndef USBCFG_EHCI_PIPE_TIMEOUT
    #define USBCFG_EHCI_PIPE_TIMEOUT            (300)
    #endif
    
    /** MGCT: </category> */

#endif // USBCFG_EHCI

#endif
