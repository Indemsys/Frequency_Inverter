/**HEADER**********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: macnet_1588.h$
* $Version : 3.8.7.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   This file contains the definitions of constants,IOCTL'S and structures
*   required for the PTP functionality.
*
*END**************************************************************************/
#ifndef macnet_1588_h
#define macnet_1588_h 1

/* IOCTL calls */
#define MACNET_PTP_GET_TX_TIMESTAMP             _IO(IO_TYPE_MEDIACTL_PTP,0x01)
#define MACNET_PTP_GET_RX_TIMESTAMP             _IO(IO_TYPE_MEDIACTL_PTP,0x02)
#define MACNET_PTP_GET_CURRENT_TIME             _IO(IO_TYPE_MEDIACTL_PTP,0x03)
#define MACNET_PTP_SET_RTC_TIME                 _IO(IO_TYPE_MEDIACTL_PTP,0x04)
#define MACNET_PTP_SET_COMPENSATION             _IO(IO_TYPE_MEDIACTL_PTP,0x05)
#define MACNET_PTP_FLUSH_TIMESTAMP              _IO(IO_TYPE_MEDIACTL_PTP,0x06)
#define MACNET_PTP_GET_ORIG_COMP                _IO(IO_TYPE_MEDIACTL_PTP,0x07)
#define MACNET_PTP_REGISTER_ETHERTYPE_PTPV2     _IO(IO_TYPE_MEDIACTL_PTP,0x08)
#define MACNET_PTP_SEND_ETHERTYPE_PTPV2_PCK     _IO(IO_TYPE_MEDIACTL_PTP,0x09)
#define MACNET_PTP_RECV_ETHERTYPE_PTPV2_PCK     _IO(IO_TYPE_MEDIACTL_PTP,0x0A)
#define MACNET_PTP_IS_IN_INBAND_MODE            _IO(IO_TYPE_MEDIACTL_PTP,0x0B)
#define MACNET_PTP_UNREGISTER_ETHERTYPE_PTPV2   _IO(IO_TYPE_MEDIACTL_PTP,0x0C)

#define MACNET_PTP_RX_TIMESTAMP_SYNC            0x0
#define MACNET_PTP_RX_TIMESTAMP_DEL_REQ         0x1
#define MACNET_PTP_RX_TIMESTAMP_PDELAY_REQ      0x2
#define MACNET_PTP_RX_TIMESTAMP_PDELAY_RESP     0x3

#define MQX1588_PTP_ETHERTYPE_1588              0x88F7

#define MACNET_PTP_EVENT_MSG_FRAME_SIZE         0x56
#define MACNET_PTP_EVENT_MSG_PTP_SIZE           0x2C
#define MACNET_PTP_INBAND_SEC_OFFS              0x00
#define MACNET_PTP_INBAND_NANOSEC_OFFS          0x08

/* PTP message version */
#define MACNET_PTP_MSG_VER_1                    1
#define MACNET_PTP_MSG_VER_2                    2

/* PTP standard time representation structure */
typedef struct {
	uint_64 SEC;	/* seconds */
	uint_32 NSEC;	/* nanoseconds */
}MACNET_PTP_TIME;

/* interface for PTP driver command MACNET_PTP_GET_RX_TIMESTAMP/MACNET_PTP_GET_TX_TIMESTAMP */
typedef struct {
	/* PTP version */
	uint_8 VERSION;
	/* PTP source port ID */
	uint_8 SPID[10];
	/* PTP sequence ID */
	uint_16 SEQ_ID;
	/* PTP message type */
	uint_8 MESSAGE_TYPE;
	/* PTP timestamp */
	MACNET_PTP_TIME TS;
}MACNET_PTP_TS_DATA;

/* interface for PTP driver command MACNET_PTP_SET_RTC_TIME/MACNET_PTP_GET_CURRENT_TIME */
typedef struct {
	MACNET_PTP_TIME RTC_TIME;
}MACNET_PTP_RTC_TIME;

/* interface for PTP driver command MACNET_PTP_SET_COMPENSATION */
typedef struct {
	int_32 DRIFT;
}MACNET_PTP_SET_COMP;

/* interface for PTP driver command MACNET_PTP_GET_ORIG_COMP */
typedef struct {
	/* the initial compensation value */
	uint_32 DW_ORIGCOMP;
	/* the minimum compensation value */
	uint_32 DW_MINCOMP;
	/*the max compensation value*/
	uint_32 DW_MAXCOMP;
	/*the min drift applying min compensation value in ppm*/
	uint_32 DW_MINDRIFT;
	/*the max drift applying max compensation value in ppm*/
	uint_32 DW_MAXDRIFT;
}MACNET_PTP_GET_COMP;

/* interface for PTP driver command MACNET_PTP_SEND_ETHERTYPE_PTPV2_PCK */
typedef struct {
	/* pointer to the PTP message */
	uint_8* PTP_MSG;
	/* size of the PTP message */
	uint_16 LENGTH;
	/* destination MAC address */
	_enet_address DEST_MAC;
}MACNET_PTP_ETHERTYPE_PCK;


#endif
/* EOF */

