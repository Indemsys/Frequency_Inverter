/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: hmi_uids.h$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*   This header defines HID tables.
*
*END************************************************************************/
#ifndef __hmi_uids_h__
#define __hmi_uids_h__ 1

#define HMI_HID_USAGE_PAGE_MASK             0xffff0000
#define HMI_HID_USAGE_ID_MASK               0x0000ffff

/* USAGE PAGES */
#define HMI_HID_USAGE_PAGE_UNDEFINED        0x00000000
#define HMI_HID_USAGE_PAGE_GENERIC          0x00010000
#define HMI_HID_USAGE_PAGE_SIMULATION       0x00020000
#define HMI_HID_USAGE_PAGE_VR               0x00030000
#define HMI_HID_USAGE_PAGE_SPORT            0x00040000
#define HMI_HID_USAGE_PAGE_GAME             0x00050000
#define HMI_HID_USAGE_PAGE_DEV_CONTROLS     0x00060000
#define HMI_HID_USAGE_PAGE_KEYBOARD         0x00070000
#define HMI_HID_USAGE_PAGE_LED              0x00080000
#define HMI_HID_USAGE_PAGE_BUTTON           0x00090000
#define HMI_HID_USAGE_PAGE_ORDINAL          0x000A0000
#define HMI_HID_USAGE_PAGE_TELEPHONY        0x000B0000
#define HMI_HID_USAGE_PAGE_CONSUMER         0x000C0000
#define HMI_HID_USAGE_PAGE_DIGITIZER        0x000D0000
#define HMI_HID_USAGE_PAGE_UNICODE          0x00100000
#define HMI_HID_USAGE_PAGE_ALPHANUMERIC     0x00140000
/* Reserved space in HID USAGE table used for LEDx */
#define HMI_HID_USAGE_PAGE_LED_OFFSET       0x0000004E    /* reserved space in LED page */

#define HMI_GET_BUTTON_ID(number)           (HMI_HID_USAGE_PAGE_BUTTON     + (number))
#define HMI_GET_LED_ID(number)              (HMI_HID_USAGE_PAGE_LED + HMI_HID_USAGE_PAGE_LED_OFFSET + (number))
#define HMI_GET_ORDINAL_ID(number)          (HMI_HID_USAGE_PAGE_ORDINAL    + (number))
#define HMI_GET_GENERIC_ID(number)          (HMI_HID_USAGE_PAGE_GENERIC    + (number))

#define HMI_GET_BUTTON_NUMBER(id)   (HMI_HID_USAGE_ID_MASK & (id))
#define HMI_GET_LED_NUMBER(id)      ((HMI_HID_USAGE_ID_MASK & (id)) - HMI_HID_USAGE_PAGE_LED_OFFSET)

/* BUTTONS */
#define HMI_BUTTON_1        HMI_GET_BUTTON_ID(1)
#define HMI_BUTTON_2        HMI_GET_BUTTON_ID(2)
#define HMI_BUTTON_3        HMI_GET_BUTTON_ID(3)
#define HMI_BUTTON_4        HMI_GET_BUTTON_ID(4)
#define HMI_BUTTON_5        HMI_GET_BUTTON_ID(5)
#define HMI_BUTTON_6        HMI_GET_BUTTON_ID(6)
#define HMI_BUTTON_7        HMI_GET_BUTTON_ID(7)
#define HMI_BUTTON_8        HMI_GET_BUTTON_ID(8)
#define HMI_BUTTON_9        HMI_GET_BUTTON_ID(9)
#define HMI_BUTTON_10       HMI_GET_BUTTON_ID(10)
#define HMI_BUTTON_11       HMI_GET_BUTTON_ID(11)
#define HMI_BUTTON_12       HMI_GET_BUTTON_ID(12)
#define HMI_BUTTON_13       HMI_GET_BUTTON_ID(13)
#define HMI_BUTTON_14       HMI_GET_BUTTON_ID(14)
#define HMI_BUTTON_15       HMI_GET_BUTTON_ID(15)
#define HMI_BUTTON_16       HMI_GET_BUTTON_ID(16)
#define HMI_BUTTON_17       HMI_GET_BUTTON_ID(17)
#define HMI_BUTTON_18       HMI_GET_BUTTON_ID(18)
#define HMI_BUTTON_19       HMI_GET_BUTTON_ID(19)
#define HMI_BUTTON_20       HMI_GET_BUTTON_ID(20)
#define HMI_BUTTON_21       HMI_GET_BUTTON_ID(21)
#define HMI_BUTTON_22       HMI_GET_BUTTON_ID(22)
#define HMI_BUTTON_23       HMI_GET_BUTTON_ID(23)
#define HMI_BUTTON_24       HMI_GET_BUTTON_ID(24)
#define HMI_BUTTON_25       HMI_GET_BUTTON_ID(25)
#define HMI_BUTTON_26       HMI_GET_BUTTON_ID(26)
#define HMI_BUTTON_27       HMI_GET_BUTTON_ID(27)
#define HMI_BUTTON_28       HMI_GET_BUTTON_ID(28)
#define HMI_BUTTON_29       HMI_GET_BUTTON_ID(29)
#define HMI_BUTTON_30       HMI_GET_BUTTON_ID(30)
#define HMI_BUTTON_31       HMI_GET_BUTTON_ID(31)
#define HMI_BUTTON_32       HMI_GET_BUTTON_ID(32)
#define HMI_BUTTON_33       HMI_GET_BUTTON_ID(33)
#define HMI_BUTTON_34       HMI_GET_BUTTON_ID(34)
#define HMI_BUTTON_35       HMI_GET_BUTTON_ID(35)
#define HMI_BUTTON_36       HMI_GET_BUTTON_ID(36)
#define HMI_BUTTON_37       HMI_GET_BUTTON_ID(37)
#define HMI_BUTTON_38       HMI_GET_BUTTON_ID(38)
#define HMI_BUTTON_39       HMI_GET_BUTTON_ID(39)
#define HMI_BUTTON_40       HMI_GET_BUTTON_ID(40)
#define HMI_BUTTON_41       HMI_GET_BUTTON_ID(41)
#define HMI_BUTTON_42       HMI_GET_BUTTON_ID(42)
#define HMI_BUTTON_43       HMI_GET_BUTTON_ID(43)
#define HMI_BUTTON_44       HMI_GET_BUTTON_ID(44)
#define HMI_BUTTON_45       HMI_GET_BUTTON_ID(45)
#define HMI_BUTTON_46       HMI_GET_BUTTON_ID(46)
#define HMI_BUTTON_47       HMI_GET_BUTTON_ID(47)
#define HMI_BUTTON_48       HMI_GET_BUTTON_ID(48)
#define HMI_BUTTON_49       HMI_GET_BUTTON_ID(49)
#define HMI_BUTTON_50       HMI_GET_BUTTON_ID(50)

/* LEDS */
#define HMI_LED_1           HMI_GET_LED_ID(1)
#define HMI_LED_2           HMI_GET_LED_ID(2)
#define HMI_LED_3           HMI_GET_LED_ID(3)
#define HMI_LED_4           HMI_GET_LED_ID(4)
#define HMI_LED_5           HMI_GET_LED_ID(5)
#define HMI_LED_6           HMI_GET_LED_ID(6)
#define HMI_LED_7           HMI_GET_LED_ID(7)
#define HMI_LED_8           HMI_GET_LED_ID(8)
#define HMI_LED_9           HMI_GET_LED_ID(9)
#define HMI_LED_10          HMI_GET_LED_ID(10)

/* SLIDERS 
*  first slider - generic page, other instances are in ordinal page
*/
#define HMI_SLIDER_1        HMI_GET_GENERIC_ID(36)      /* slider = 36 */
#define HMI_SLIDER_2        HMI_GET_ORDINAL_ID(1)       /* ordinal 1-4 for sliders */
#define HMI_SLIDER_3        HMI_GET_ORDINAL_ID(2)
#define HMI_SLIDER_4        HMI_GET_ORDINAL_ID(3)
#define HMI_SLIDER_5        HMI_GET_ORDINAL_ID(4)

/* ROTARIES 
*  first rotary - generic page, other instances are in ordinal page
*/
#define HMI_ROTARY_1        HMI_GET_GENERIC_ID(38)    /* wheel = 38*/
#define HMI_ROTARY_2        HMI_GET_ORDINAL_ID(10)    /* ordinal 10-13 for rotary */
#define HMI_ROTARY_3        HMI_GET_ORDINAL_ID(11)
#define HMI_ROTARY_4        HMI_GET_ORDINAL_ID(12)
#define HMI_ROTARY_5        HMI_GET_ORDINAL_ID(13)

#endif
