/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2010
 *
 *    File name   : low_level_init.c
 *    Description : Low level init procedure
 *
 *    History :
 *    1. Date        : 08, September 2010
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *
 *    $Revision: #2 $
 **************************************************************************/
#include "app_cfg.h"

/*************************************************************************
 * Function Name: low_level_init
 * Parameters: none
 *
 * Return: none
 *
 * Description: This function is used for low level initialization
 *
 *************************************************************************/
int __low_level_init(void)
{
  WDOG_MemMapPtr WDOG = WDOG_BASE_PTR;

  // Выключать WATCHDOG надо сразу, иначе может возникнуть нестабильный запуск кода после холодного рестарта 
  //--------------------------------------------------------------------------------------------------------------------------------------
  WDOG->UNLOCK = 0xC520; // Откроем доступ на запись в регитры управления WDOG
  WDOG->UNLOCK = 0xD928;
  WDOG->STCTRLH = 0
                  + LSHIFT(0x00, 14) // DISTESTWDOG | Allows the WDOG’s functional test mode to be disabled permanently| 0 WDOG functional test mode is not disabled.
                  + LSHIFT(0x00, 12) // BYTESEL[1:0]| This 2-bit field select the byte to be tested ...                | 00 Byte 0 selected
                  + LSHIFT(0x00, 11) // TESTSEL     | Selects the test to be run on the watchdog timer                 | 0 Quick test
                  + LSHIFT(0x00, 10) // TESTWDOG    | Puts the watchdog in the functional test mode                    |
                  + LSHIFT(0x01, 8)  // Reserved    |
                  + LSHIFT(0x01, 7)  // WAITEN      | Enables or disables WDOG in wait mode.                           | 1 WDOG is enabled in CPU wait mode.
                  + LSHIFT(0x01, 6)  // STOPEN      | Enables or disables WDOG in stop mode                            | 1 WDOG is enabled in CPU stop mode.
                  + LSHIFT(0x00, 5)  // DBGEN       | Enables or disables WDOG in Debug mode                           | 0 WDOG is disabled in CPU Debug mode.
                  + LSHIFT(0x01, 4)  // ALLOWUPDATE | Enables updates to watchdog write once registers                 | 1 WDOG write once registers can be unlocked for updating
                  + LSHIFT(0x00, 3)  // WINEN       | Enable windowing mode.                                           | 0 Windowing mode is disabled.
                  + LSHIFT(0x00, 2)  // IRQRSTEN    | Used to enable the debug breadcrumbs feature                     | 0 WDOG time-out generates reset only.
                  + LSHIFT(0x01, 1)  // CLKSRC      | Selects clock source for the WDOG                                | 1 WDOG clock sourced from alternate clock source
                  + LSHIFT(0x00, 0)  // WDOGEN      | Enables or disables the WDOG’s operation                         | 1 WDOG is enabled.
  ;


  Init_pins();
  Init_cpu();

  return 1;
}

