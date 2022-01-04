#ifndef _DFU_MOUSE_H
  #define _DFU_MOUSE_H



  #define  CONTROLLER_ID   (0)   /* ID to identify USB CONTROLLER */

/* #define UP_LEFT   This macro enables up and left mouse movement via PTG2-3*/
  #ifdef UP_LEFT
    #define SHIFT_VAL  (0xF8)    /* send -8 for up and left movement */
  #else
    #define SHIFT_VAL  (0x08)    /* send +8 for down and right movement */
  #endif

  #define  MOUSE_BUFF_SIZE   (4)    /* report buffer size */
  #define  REQ_DATA_SIZE     (1)
  #define  KBI_STAT_MASK     (0x0F)
  #define  LEFT_CLICK        (0x01) /* Left click */
  #define  RIGHT_CLICK       (0x02) /* Right click */
  #define  MOVE_LEFT_RIGHT   (0x04) /* Left-Right movement*/
  #define  MOVE_UP_DOWN      (0x08) /* Up-Down movement*/

extern INT8U g_dfu_mode;
extern void USB_App_init(void);
extern void USB_Class_DFU_task(void);
extern unsigned long *manifest_address;
#endif
