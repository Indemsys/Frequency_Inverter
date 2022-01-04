/**HEADER**********************************************************************
*
* Copyright (c) 2009 Freescale Semiconductor;
* All Rights Reserved
*
*
*******************************************************************************
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
*******************************************************************************
*
* $FileName: enet_wifi.h$
* $Version : 3.6.3.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   This file contains the definitions of constants,IOCTL'S and structures
*   required for the Wifi operation.
*
*END**************************************************************************/
#ifndef enet_wifi_h
#define enet_wifi_h 1

#include "mqx.h"
#include "bsp.h"
#include "ioctl.h"

/* IOCTL's to set various features of WIFI device. */
#define ENET_SET_MEDIACTL_COMMIT                                 _IO(IO_TYPE_MEDIACTL_WIFI,0x01)
#define ENET_SET_MEDIACTL_FREQ                                   _IO(IO_TYPE_MEDIACTL_WIFI,0x02)
#define ENET_SET_MEDIACTL_MODE                                   _IO(IO_TYPE_MEDIACTL_WIFI,0x03)
#define ENET_SET_MEDIACTL_SCAN                                   _IO(IO_TYPE_MEDIACTL_WIFI,0x04)
#define ENET_SET_MEDIACTL_ESSID                                  _IO(IO_TYPE_MEDIACTL_WIFI,0x05)
#define ENET_SET_MEDIACTL_RATE                                   _IO(IO_TYPE_MEDIACTL_WIFI,0x06)
#define ENET_SET_MEDIACTL_RTS                                    _IO(IO_TYPE_MEDIACTL_WIFI,0x07)
#define ENET_SET_MEDIACTL_RETRY                                  _IO(IO_TYPE_MEDIACTL_WIFI,0x08)
#define ENET_SET_MEDIACTL_ENCODE                                 _IO(IO_TYPE_MEDIACTL_WIFI,0x09)
#define ENET_SET_MEDIACTL_POWER                                  _IO(IO_TYPE_MEDIACTL_WIFI,0x0A)
#define ENET_SET_MEDIACTL_SEC_TYPE                               _IO(IO_TYPE_MEDIACTL_WIFI,0x0B)
#define ENET_SET_MEDIACTL_PASSPHRASE                             _IO(IO_TYPE_MEDIACTL_WIFI,0x0C)
#define ENET_SET_MEDIACTL_DEBUG                                  _IO(IO_TYPE_MEDIACTL_WIFI,0x0D)

/* IOCTL's to get various features of WIFI device. */
#define ENET_GET_MEDIACTL_NAME                                   _IO(IO_TYPE_MEDIACTL_WIFI,0x0E)
#define ENET_GET_MEDIACTL_FREQ                                   _IO(IO_TYPE_MEDIACTL_WIFI,0x0F)
#define ENET_GET_MEDIACTL_MODE                                   _IO(IO_TYPE_MEDIACTL_WIFI,0x10)
#define ENET_GET_MEDIACTL_RANGE                                  _IO(IO_TYPE_MEDIACTL_WIFI,0x11)
#define ENET_GET_MEDIACTL_WAP                                    _IO(IO_TYPE_MEDIACTL_WIFI,0x12)
#define ENET_GET_MEDIACTL_SCAN                                   _IO(IO_TYPE_MEDIACTL_WIFI,0x13)
#define ENET_GET_MEDIACTL_ESSID                                  _IO(IO_TYPE_MEDIACTL_WIFI,0x14)
#define ENET_GET_MEDIACTL_RATE                                   _IO(IO_TYPE_MEDIACTL_WIFI,0x15)
#define ENET_GET_MEDIACTL_RTS                                    _IO(IO_TYPE_MEDIACTL_WIFI,0x16)
#define ENET_GET_MEDIACTL_RETRY                                  _IO(IO_TYPE_MEDIACTL_WIFI,0x17)
#define ENET_GET_MEDIACTL_ENCODE                                 _IO(IO_TYPE_MEDIACTL_WIFI,0x18)
#define ENET_GET_MEDIACTL_POWER                                  _IO(IO_TYPE_MEDIACTL_WIFI,0x19)
#define ENET_GET_MEDIACTL_SEC_TYPE                               _IO(IO_TYPE_MEDIACTL_WIFI,0x20)
#define ENET_MEDIACTL_IS_INITIALIZED                             _IO(IO_TYPE_MEDIACTL_WIFI,0x21)
#define ENET_GET_MEDIACTL_PASSPHRASE                             _IO(IO_TYPE_MEDIACTL_WIFI,0x22)

#define ENET_MEDIACTL_FREQ_AUTO                                  (0x00)/* Let the driver decides */
#define ENET_MEDIACTL_FREQ_FIXED                                 (0x01)/* Force a specific value */
/* Flags for encoding (along with the token) */
#define ENET_MEDIACTL_ENCODE_INDEX                               0x00FF  /* Token index (if needed) */
#define ENET_MEDIACTL_ENCODE_FLAGS                               0xFF00  /* Flags defined below */
#define ENET_MEDIACTL_ENCODE_MODE                                0xF000  /* Modes defined below */
#define ENET_MEDIACTL_ENCODE_DISABLED                            0x8000  /* Encoding disabled */
#define ENET_MEDIACTL_ENCODE_ENABLED                             0x0000  /* Encoding enabled */
#define ENET_MEDIACTL_ENCODE_RESTRICTED                          0x4000  /* Refuse non-encoded packets */
#define ENET_MEDIACTL_ENCODE_OPEN                                0x2000  /* Accept non-encoded packets */
#define ENET_MEDIACTL_ENCODE_NOKEY                               0x0800  /* Key is write only, so not present */
#define ENET_MEDIACTL_ENCODE_TEMP                                0x0400  /* Temporary key */
/* Retry limits and lifetime flags available */
#define ENET_MEDIACTL_RETRY_ON                                   0x0000  /* No details... */
#define ENET_MEDIACTL_RETRY_TYPE                                 0xF000  /* Type of parameter */
#define ENET_MEDIACTL_RETRY_LIMIT                                0x1000  /* Maximum number of retries*/
#define ENET_MEDIACTL_RETRY_LIFETIME                             0x2000  /* Maximum duration of retries in us */
#define ENET_MEDIACTL_RETRY_MODIFIER                             0x00FF  /* Modify a parameter */
#define ENET_MEDIACTL_RETRY_MIN                                  0x0001  /* Value is a minimum  */
#define ENET_MEDIACTL_RETRY_MAX                                  0x0002  /* Value is a maximum */
#define ENET_MEDIACTL_RETRY_RELATIVE                             0x0004  /* Value is not in seconds/ms/us */
#define ENET_MEDIACTL_RETRY_SHORT                                0x0010  /* Value is for short packets  */
#define ENET_MEDIACTL_RETRY_LONG                                 0x0020  /* Value is for long packets */

/* Power management flags available (along with the value, if any) */
#define ENET_MEDIACTL_POWER_ON                                   0x0000  /* No details... */
#define ENET_MEDIACTL_POWER_TYPE                                 0xF000  /* Type of parameter */
#define ENET_MEDIACTL_POWER_PERIOD                               0x1000  /* Value is a period/duration of  */
#define ENET_MEDIACTL_POWER_TIMEOUT                              0x2000  /* Value is a timeout (to go asleep) */
#define ENET_MEDIACTL_POWER_MODE                                 0x0F00  /* Power Management mode */
#define ENET_MEDIACTL_POWER_UNICAST_R                            0x0100  /* Receive only unicast messages */
#define ENET_MEDIACTL_POWER_MULTICAST_R                          0x0200  /* Receive only multicast messages */
#define ENET_MEDIACTL_POWER_ALL_R                                0x0300  /* Receive all messages though PM */
#define ENET_MEDIACTL_POWER_FORCE_S                              0x0400  /* Force PM procedure for sending unicast */
#define ENET_MEDIACTL_POWER_REPEATER                             0x0800  /* Repeat broadcast messages in PM period */
#define ENET_MEDIACTL_POWER_MODIFIER                             0x000F  /* Modify a parameter */
#define ENET_MEDIACTL_POWER_MIN                                  0x0001  /* Value is a minimum  */
#define ENET_MEDIACTL_POWER_MAX                                  0x0002  /* Value is a maximum */
#define ENET_MEDIACTL_POWER_RELATIVE                             0x0004  /* Value is not in seconds/ms/us */

/* Security types */
#define ENET_MEDIACTL_SECURITY_TYPE_NONE                         (0x00)
#define ENET_MEDIACTL_SECURITY_TYPE_WEP                          (0x01)
#define ENET_MEDIACTL_SECURITY_TYPE_WPA                          (0x02)
#define ENET_MEDIACTL_SECURITY_TYPE_WPA2                         (0x03)

#define ENET_MEDIACTL_MODE_AUTO                                  (0)
/* Single cell network */
#define ENET_MEDIACTL_MODE_ADHOC                                 (1)
/* Multi cell network, roaming, ... */
#define ENET_MEDIACTL_MODE_INFRA                                 (2)
/* Synchronisation master or Access Point */
#define ENET_MEDIACTL_MODE_MASTER                                (3)
/* Wireless Repeater (forwarder) */
#define ENET_MEDIACTL_MODE_REPEAT                                (4)
 /* Secondary master/repeater (backup) */
#define ENET_MEDIACTL_MODE_SECOND                                (5)
/* Passive monitor (listen only) */
#define ENET_MEDIACTL_MODE_MONITOR                               (6)
/* Mesh (IEEE 802.11s) network */
#define ENET_MEDIACTL_MODE_MESH                                  (7)


typedef struct __essid
{
    char_ptr  essid;
    uint_16 flags;
    uint_32 length;
}ENET_ESSID,* ENET_ESSID_PTR;

typedef struct _scan_info 
{
    uint_8 channel;
    uint_8 ssid_len;
    uint_8 rssi;
    uint_8 security_enabled;
    uint_16 beacon_period;
    uint_8 preamble;
    uint_8 bss_type;
    uint_8 bssid[6];
    uint_8 ssid[32];
} ENET_SCAN_INFO, * ENET_SCAN_INFO_PTR;

typedef struct _scan_list 
{
    int_32 num_scan_entries;
    ENET_SCAN_INFO_PTR scan_info_list;  
}ENET_SCAN_LIST,* ENET_SCAN_LIST_PTR;
#endif
/* EOF */

