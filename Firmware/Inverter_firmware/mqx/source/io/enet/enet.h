#ifndef __enet_h__
#define __enet_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: enet.h$
* $Version : 3.8.40.0$
* $Date    : Sep-20-2012$
*
* Comments:
*
*   This file contains the defines, externs and data
*   structure definitions required by application
*   programs in order to use the Ethernet packet driver.
*
*END************************************************************************/

#include <pcb.h>
#include <ethernet.h>

/*--------------------------------------------------------------------------*/
/*                        
**                            CONSTANT DEFINITIONS
*/

/* Error codes */

#define ENET_OK                     (0)
#define ENET_ERROR                  (ENET_ERROR_BASE | 0xff)  /* general ENET error */

#define ENETERR_INVALID_DEVICE      (ENET_ERROR_BASE | 0x00)   /* Device number out of range  */
#define ENETERR_INIT_DEVICE         (ENET_ERROR_BASE | 0x01)   /* Device already initialized  */
#define ENETERR_ALLOC_CFG           (ENET_ERROR_BASE | 0x02)   /* Alloc state failed          */
#define ENETERR_ALLOC_PCB           (ENET_ERROR_BASE | 0x03)   /* Alloc PCBs failed           */
#define ENETERR_ALLOC_BD            (ENET_ERROR_BASE | 0x04)   /* Alloc BDs failed            */
#define ENETERR_INSTALL_ISR         (ENET_ERROR_BASE | 0x05)   /* Install ISR failed          */
#define ENETERR_FREE_PCB            (ENET_ERROR_BASE | 0x06)   /* PCBs in use                 */
#define ENETERR_ALLOC_ECB           (ENET_ERROR_BASE | 0x07)   /* Alloc ECB failed            */
#define ENETERR_OPEN_PROT           (ENET_ERROR_BASE | 0x08)   /* Protocol not open           */
#define ENETERR_CLOSE_PROT          (ENET_ERROR_BASE | 0x09)   /* Protocol already open       */
#define ENETERR_SEND_SHORT          (ENET_ERROR_BASE | 0x0A)   /* Packet too short            */
#define ENETERR_SEND_LONG           (ENET_ERROR_BASE | 0x0B)   /* Packet too long             */
#define ENETERR_JOIN_MULTICAST      (ENET_ERROR_BASE | 0x0C)   /* Not a multicast address     */
#define ENETERR_ALLOC_MCB           (ENET_ERROR_BASE | 0x0D)   /* Alloc MCB failed            */
#define ENETERR_LEAVE_GROUP         (ENET_ERROR_BASE | 0x0E)   /* Not a joined group          */
#define ENETERR_SEND_FULL           (ENET_ERROR_BASE | 0x0F)   /* Transmit ring full          */
#define ENETERR_IP_TABLE_FULL       (ENET_ERROR_BASE | 0x10)   /* IP Table full of IP pairs   */
#define ENETERR_ALLOC               (ENET_ERROR_BASE | 0x11)   /* Generic alloc failed        */
#define ENETERR_INIT_FAILED         (ENET_ERROR_BASE | 0x12)   /* Device failed to initialize */
#define ENETERR_DEVICE_TIMEOUT      (ENET_ERROR_BASE | 0x13)   /* Device read/write timeout   */
#define ENETERR_ALLOC_BUFFERS       (ENET_ERROR_BASE | 0x14)   /* Buffer alloc failed         */
#define ENETERR_ALLOC_MAC_CONTEXT   (ENET_ERROR_BASE | 0x15)   /* Buffer alloc failed         */
#define ENETERR_NO_TX_BUFFER        (ENET_ERROR_BASE | 0x16)   /* TX Buffer alloc failed      */
#define ENETERR_INVALID_INIT_PARAM  (ENET_ERROR_BASE | 0x17)   /* Invalid init. parameter     */
#define ENETERR_DEVICE_IN_USE       (ENET_ERROR_BASE | 0x18)   /* Shutdown failed, dev. in use*/
#define ENETERR_INITIALIZED_DEVICE  (ENET_ERROR_BASE | 0x19)   /* Device already initialized  */
#define ENETERR_INPROGRESS          (ENET_ERROR_BASE | 0x1A)   /* In Wifi Device Setting of ESSID in progress*/
#define ENETERR_1588_LWEVENT        (ENET_ERROR_BASE | 0x1B)   /* 1588driver lwevent creation failed */
#define ENETERR_INVALID_MODE        (ENET_ERROR_BASE | 0x1C)   /* Invalid mode for this ethernet driver */
#define ENETERR_INVALID_OPTION      (ENET_ERROR_BASE | 0x1D)   /* Invalid option for this ethernet driver */

#define ENETERR_MIN                 (ENETERR_INVALID_DEVICE)
#define ENETERR_MAX                 (ENETERR_INVALID_OPTION)

/* Other constants */


/* MODES */
#define ENET_HALF_DUPLEX    0x1
#define ENET_FULL_DUPLEX    0x0
#define ENET_DUPLEX_MASK    0x1

#define ENET_10M            0x02
#define ENET_100M           0x04
#define ENET_1G             0x06
#define ENET_SPEED_MASK     0x0e

#define ENET_AUTONEGOTIATE  0x10

/* OPTIONS */
#define ENET_OPTION_MII             0x00000000
#define ENET_OPTION_VLAN            0x00000001
#define ENET_OPTION_PHY_DISCOVER    0x00000002
#define ENET_OPTION_MAC_LOOPBACK    0x00000004
#define ENET_OPTION_PHY_LOOPBACK    0x00000008
#define ENET_OPTION_NO_PREAMBLE     0x00000010
#define ENET_OPTION_RMII            0x00000020
#define ENET_OPTION_7WIRE           0x00000040
#define ENET_OPTION_STORE_AND_FORW  0x00000080
#define ENET_OPTION_PTP_MASTER_CLK  0x00000100
#define ENET_OPTION_PTP_INBAND      0x00000200

/* NOTE: Not all MAC drivers will support all modes and options. */

/* Media control definition types */
#define IO_TYPE_MEDIACTL_WIFI   0x00
#define IO_TYPE_MEDIACTL_PTP    0x10


#if BSPCFG_ENABLE_ENET_STATS

   #ifndef BSPCFG_ENABLE_ENET_HISTOGRAM
      #define BSPCFG_ENABLE_ENET_HISTOGRAM 1
   #endif
   
   #define ENET_INC_STATS(x)   { enet_ptr->STATS.x++; }
#define ENET_INC_STATS_IF(c,x)   { if (c) ENET_INC_STATS(x) }
#else
   #define ENET_INC_STATS(x)
#define ENET_INC_STATS_IF(c,x)   
#endif

#define ENET_HISTOGRAM_SHIFT       (6)
#define ENET_HISTOGRAM_ENTRIES     (((ENET_FRAMESIZE)>>ENET_HISTOGRAM_SHIFT)+1)


/*--------------------------------------------------------------------------*/
/*                        
**                            TYPE DEFINITIONS
*/

typedef pointer _enet_handle;
  
typedef enum {
  Half_Duplex_10M   = (ENET_HALF_DUPLEX | ENET_10M),
  Full_Duplex_10M   = (ENET_FULL_DUPLEX | ENET_10M),
  Half_Duplex_100M  = (ENET_HALF_DUPLEX | ENET_100M),
  Full_Duplex_100M  = (ENET_FULL_DUPLEX | ENET_100M),
  Half_Duplex_1G    = (ENET_HALF_DUPLEX | ENET_1G),
  Full_Duplex_1G    = (ENET_FULL_DUPLEX | ENET_1G),
  Auto_Negotiate    = ENET_AUTONEGOTIATE
} ENET_mode;
  
#if BSPCFG_ENABLE_ENET_STATS
typedef struct enet_commom_stats_struct {
   uint_32     ST_RX_TOTAL;         /* Total number of received packets    */
   uint_32     ST_RX_MISSED;        /* Number of missed packets            */
   uint_32     ST_RX_DISCARDED;     /* Discarded -- unrecognized protocol  */
   uint_32     ST_RX_ERRORS;        /* Discarded -- error during reception */

   uint_32     ST_TX_TOTAL;         /* Total number of transmitted packets */
   uint_32     ST_TX_MISSED;        /* Discarded -- transmit ring full     */
   uint_32     ST_TX_DISCARDED;     /* Discarded -- bad packet             */
   uint_32     ST_TX_ERRORS;        /* Error during transmission           */
} ENET_COMMON_STATS_STRUCT, _PTR_ ENET_COMMON_STATS_STRUCT_PTR;

typedef struct enet_stats {
   ENET_COMMON_STATS_STRUCT   COMMON;

   /* Following stats are physical errors/conditions */
   uint_32     ST_RX_ALIGN;         /* Frame Alignment error    */
   uint_32     ST_RX_FCS;           /* CRC error                */
   uint_32     ST_RX_RUNT;          /* Runt packet received     */
   uint_32     ST_RX_GIANT;         /* Giant packet received    */
   uint_32     ST_RX_LATECOLL;      /* Late collision           */
   uint_32     ST_RX_OVERRUN;       /* DMA overrun              */

   uint_32     ST_TX_SQE;           /* Heartbeat lost           */
   uint_32     ST_TX_DEFERRED;      /* Transmission deferred    */
   uint_32     ST_TX_LATECOLL;      /* Late collision           */
   uint_32     ST_TX_EXCESSCOLL;    /* Excessive collisions     */
   uint_32     ST_TX_CARRIER;       /* Carrier sense lost       */
   uint_32     ST_TX_UNDERRUN;      /* DMA underrun             */

   /* Following stats are collected by the ethernet driver  */
   uint_32     ST_RX_COPY_SMALL;     /* Driver had to copy packet */
   uint_32     ST_RX_COPY_LARGE;     /* Driver had to copy packet */
   uint_32     ST_TX_COPY_SMALL;     /* Driver had to copy packet */
   uint_32     ST_TX_COPY_LARGE;     /* Driver had to copy packet */

   uint_32     RX_FRAGS_EXCEEDED;
   uint_32     RX_PCBS_EXHAUSTED;
   uint_32     RX_LARGE_BUFFERS_EXHAUSTED;

   uint_32     TX_ALIGNED;
   uint_32     TX_ALL_ALIGNED;

#if BSPCFG_ENABLE_ENET_HISTOGRAM
   uint_32     RX_HISTOGRAM[ENET_HISTOGRAM_ENTRIES];  
   uint_32     TX_HISTOGRAM[ENET_HISTOGRAM_ENTRIES];  
#endif
  
} ENET_STATS, _PTR_ ENET_STATS_PTR;
#endif


/* forward declarations */
struct enet_mcb_struct;
struct enet_context_struct;
struct enet_param_struct;

typedef struct {
   uint_32 (_CODE_PTR_  INIT) (struct enet_context_struct *);
   uint_32 (_CODE_PTR_  STOP)(struct enet_context_struct *);
   uint_32 (_CODE_PTR_  SEND) (struct enet_context_struct *, PCB_PTR, uint_32, uint_32, uint_32);
   
   uint_32 (_CODE_PTR_  PHY_READ) (struct enet_context_struct *, uint_32, uint_32_ptr, uint_32);
   uint_32 (_CODE_PTR_  PHY_WRITE) (struct enet_context_struct *,  uint_32, uint_32, uint_32);

#if BSPCFG_ENABLE_ENET_MULTICAST
   uint_32 (_CODE_PTR_  JOIN) (struct enet_context_struct *, struct enet_mcb_struct *);
   uint_32 (_CODE_PTR_  REJOIN)(struct enet_context_struct *);
#endif
   uint_32 (_CODE_PTR_  MEDIACTL) (struct enet_context_struct *, uint_32 command_id, pointer inout_param);
} ENET_MAC_IF_STRUCT, * ENET_MAC_IF_STRUCT_PTR;


typedef struct enet_pyh_if_struct {
   boolean (_CODE_PTR_  DISCOVER) (struct enet_context_struct *);
   boolean (_CODE_PTR_  INIT) (struct enet_context_struct *);
   uint_32 (_CODE_PTR_  SPEED)(struct enet_context_struct *);
   boolean (_CODE_PTR_  STATUS) (struct enet_context_struct *);
   boolean (_CODE_PTR_  DUPLEX)(struct enet_context_struct *);
} ENET_PHY_IF_STRUCT, * ENET_PHY_IF_STRUCT_PTR;



typedef struct enet_if_struct {
   const ENET_MAC_IF_STRUCT *    MAC_IF;        /* pointer to MAC interface struct */
   const ENET_PHY_IF_STRUCT *    PHY_IF;        /* pointer to PHY interface struct */
   uchar                         MAC_NUMBER;    /* MAC device number */
   uchar                         PHY_NUMBER;    /* MAC device number for communication with PHY */
   uchar                         PHY_ADDRESS;   /* PHY address */
   uint_32                       PHY_MII_SPEED; /* PHY MII Speed (MDC - Management Data Clock) */
} ENET_IF_STRUCT, * ENET_IF_STRUCT_PTR;



typedef struct enet_param_struct {
   const ENET_IF_STRUCT         *ENET_IF;
   ENET_mode                     MODE;
   uint_32                       OPTIONS;
   
   uint_16                       NUM_TX_ENTRIES;
   uint_16                       NUM_TX_BUFFERS;
   uint_16                       TX_BUFFER_SIZE;

   uint_16                       NUM_RX_ENTRIES;
   uint_16                       NUM_RX_BUFFERS;
   uint_16                       RX_BUFFER_SIZE;
   uint_16                       NUM_RX_PCBS;
   
   uint_16                       NUM_SMALL_BUFFERS;
   uint_16                       NUM_LARGE_BUFFERS;
   pointer                       MAC_PARAM;
    
} ENET_PARAM_STRUCT, * ENET_PARAM_STRUCT_PTR;


/*
 *      Generic format for most parameters that fit in an int
 */
typedef struct  _param
{
   int_32          value;          /* The value of the parameter itself */
   uint_8          fixed;          /* Hardware should not use auto select */
   uint_8          disabled;       /* Disable the feature */
   uint_32         flags;          /* Various specifc flags (if any) */
   pointer         data;
   uint_32         length;
}ENET_MEDIACTL_PARAM,* ENET_MEDIACTL_PARAM_PTR;

/*--------------------------------------------------------------------------*/
/*                        
**                            PROTOTYPES AND GLOBAL EXTERNS
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __TAD_COMPILE__

extern const ENET_PARAM_STRUCT ENET_default_params[];

extern uint_32        ENET_close(_enet_handle, uint_16);
extern uint_32        ENET_get_address(_enet_handle, _enet_address);
extern _mqx_uint      ENET_initialize(_mqx_uint, _enet_address, _mqx_uint,    _enet_handle _PTR_);
extern _mqx_uint      ENET_initialize_ex(const ENET_PARAM_STRUCT *,  _enet_address,  _enet_handle _PTR_);
extern uint_32        ENET_open(_enet_handle, uint_16, void (_CODE_PTR_)(PCB_PTR, pointer), pointer);
extern uint_32        ENET_send(_enet_handle, PCB_PTR, uint_16, _enet_address, uint_32);
extern uint_32        ENET_send_raw(_enet_handle, PCB_PTR);
extern uint_32        ENET_shutdown(_enet_handle);
extern const char * ENET_strerror(_mqx_uint);
extern uint_32        ENET_get_speed (_enet_handle);
extern uint_32        ENET_get_MTU(_enet_handle);
extern uint_32        ENET_get_mac_address(uint_32, uint_32,_enet_address);
extern boolean        ENET_link_status(_enet_handle);
extern boolean        ENET_read_mii(_enet_handle, uint_32, uint_32_ptr, uint_32);
extern boolean        ENET_write_mii(_enet_handle, uint_32, uint_32, uint_32);
extern uint_32        ENET_get_phy_addr(_enet_handle);
extern uint_32        ENET_mediactl(_enet_handle,uint_32,pointer);
extern boolean        ENET_phy_registers(_enet_handle,uint_32,uint_32_ptr);
extern _enet_handle   ENET_get_next_device_handle(_enet_handle handle);

#if BSPCFG_ENABLE_ENET_STATS
extern ENET_STATS_PTR ENET_get_stats(_enet_handle);
#endif

#if BSPCFG_ENABLE_ENET_MULTICAST
extern uint_32        ENET_join(_enet_handle, uint_16, _enet_address);
extern uint_32        ENET_leave(_enet_handle, uint_16, _enet_address);
#endif


#endif

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
