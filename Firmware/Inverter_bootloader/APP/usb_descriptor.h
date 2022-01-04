/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
**************************************************************************/ /*!
  *
  * @file usb_descriptor.h
  *
  * @author
  *
  * @version
  *
  * @date
  *
  * @brief The file is a header file for USB Descriptors required for Mouse
  *        Application
  *****************************************************************************/

#ifndef _USB_DESCRIPTOR_H
  #define _USB_DESCRIPTOR_H

/******************************************************************************
 * Includes
 *****************************************************************************/

  #include "usb_class.h"
  #include "usb_dfu.h"
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
  #define REMOTE_WAKEUP_SHIFT               (5)
  #define REMOTE_WAKEUP_SUPPORT             (TRUE)

/* Various descriptor sizes */
  #define DEVICE_DESCRIPTOR_SIZE            (18)
  #define CONFIG_DESC_SIZE_RUNTIME          (52)
  #define DEVICE_QUALIFIER_DESCRIPTOR_SIZE  (10)
  #define REPORT_DESC_SIZE                  (52)
  #define CONFIG_ONLY_DESC_SIZE             (9)
  #define IFACE_ONLY_DESC_SIZE              (9)
  #define HID_ONLY_DESC_SIZE                (9)
  #define ENDP_ONLY_DESC_SIZE               (7)

/* HID buffer size */
  #define HID_BUFFER_SIZE                   (8)
/* Max descriptors provided by the Application */
  #define USB_MAX_STD_DESCRIPTORS               (8)
  #define USB_MAX_CLASS_SPECIFIC_DESCRIPTORS    (2)
/* Max configuration supported by the Application */
  #define USB_MAX_CONFIG_SUPPORTED          (1)

/* Max string descriptors supported by the Application */
  #define USB_MAX_STRING_DESCRIPTORS        (4)

/* Max language codes supported by the USB */
  #define USB_MAX_LANGUAGES_SUPPORTED       (1)


  #define HID_DESC_ENDPOINT_COUNT  (1)
  #define HID_ENDPOINT             (1)
  #define HID_ENDPOINT_PACKET_SIZE (8)

  #define DFU_MAX_BLOCK_SIZE       (63) // Если сделать 64 то запись 64-х битных DFU OUT пакетов сбивается.
                                        // Какая-то проблема с приемом пакетов в функции USB_Packet_RXTX_Handler в строках 1184-1220 

/* only control enpoint */
  #define DFU_DESC_ENDPOINT_COUNT  (0)
  #define FUNCTIONAL_DESC_SIZE     (9)

  #define CONFIG_DESC_SIZE_DFU  (CONFIG_ONLY_DESC_SIZE + \
                                IFACE_ONLY_DESC_SIZE + \
                                FUNCTIONAL_DESC_SIZE + \
                                ENDP_ONLY_DESC_SIZE * DFU_DESC_ENDPOINT_COUNT)
/* string descriptors sizes */
  #define USB_STR_DESC_SIZE         (2)
  #define USB_STR_0_SIZE            (2)
  #define USB_STR_1_SIZE           (56)
  #define USB_STR_2_SIZE           (24)
  #define USB_STR_3_SIZE           (20)
  #define USB_STR_n_SIZE           (32)

/* descriptors codes */
  #define USB_DEVICE_DESCRIPTOR     (1)
  #define USB_CONFIG_DESCRIPTOR     (2)
  #define USB_STRING_DESCRIPTOR     (3)
  #define USB_IFACE_DESCRIPTOR      (4)
  #define USB_ENDPOINT_DESCRIPTOR   (5)
  #define USB_DEVQUAL_DESCRIPTOR    (6)
  #define USB_HID_DESCRIPTOR        (0x21)
  #define USB_REPORT_DESCRIPTOR     (0x22)

  #define USB_MAX_SUPPORTED_INTERFACES     (1)
/* DFU functional codes */
  #define USB_DFU_FUNCTIONAL_DESCRIPTOR     (0x21)


/* DFU functional codes */
  #define DETACH               (1)
  #define MANIFESTATION        (1)
  #define BITCANUPLOAD         (1)
  #define BITCANDOWNLOAD       (1)
/* DFU_EP is control enpoint*/
  #define DFU_EP               0x00

/******************************************************************************
 * Types
 *****************************************************************************/
typedef const struct _USB_LANGUAGE
{
  INT16U const language_id;      /* Language ID */
  INT8U const **lang_desc;      /* Language Descriptor String */
  INT8U const *lang_desc_size;  /* Language Descriptor Size */
} USB_LANGUAGE;

typedef const struct _USB_ALL_LANGUAGES
{
  /* Pointer to Supported Language String */
  INT8U const *languages_supported_string;
  /* Size of Supported Language String */
  INT8U const languages_supported_size;
  /* Array of Supported Languages */
  USB_LANGUAGE usb_language[USB_MAX_SUPPORTED_INTERFACES];
}USB_ALL_LANGUAGES;

typedef const struct _USB_ENDPOINTS
{
  /* Number of non control Endpoints */
  INT8U count;
  /* Array of Endpoints Structures */
  USB_EP_STRUCT ep[HID_DESC_ENDPOINT_COUNT];
}USB_ENDPOINTS;

/******************************************************************************/
  #define STORAGE_FIRMWARE FALSE

/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern INT8U USB_Desc_Get_Descriptor(INT8U controller_ID,INT8U type,INT8U str_num,INT16U index,INT8U_ptr *descriptor,INT16U *size);
extern INT8U USB_Desc_Get_Interface(INT8U controller_ID,INT8U interface,INT8U_ptr alt_interface);
extern INT8U USB_Desc_Set_Interface(INT8U controller_ID,INT8U interface,INT8U alt_interface);
extern BOOLEAN USB_Desc_Valid_Configation(INT8U controller_ID,INT16U config_val);
extern BOOLEAN USB_Desc_Valid_Interface(INT8U controller_ID,INT8U interface);
extern BOOLEAN USB_Desc_Remote_Wakeup(INT8U controller_ID);
extern void* USB_Desc_Get_Endpoints(INT8U controller_ID);
#endif
