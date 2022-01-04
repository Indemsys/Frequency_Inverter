#ifndef __ethernet_h__
#define __ethernet_h__
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
* $FileName: ethernet.h$
* $Version : 3.8.2.0$
* $Date    : Jun-14-2012$
*
* Comments:
*
*   This file contains the defines and data structure definitions that
*   reperesent ethernet packets.
*
*END************************************************************************/



#define ENET_FRAMESIZE_HEAD       sizeof(ENET_HEADER)
#define ENET_FRAMESIZE_HEAD_VLAN  (sizeof(ENET_HEADER)+sizeof(ENET_8021QTAG_HEADER))
#define ENET_FRAMESIZE_MAXDATA    (1500)
#define ENET_FRAMESIZE_MIN        (64)
#define ENET_FRAMESIZE_TAIL       (4)

#define ENET_FRAMESIZE            ENET_FRAMESIZE_HEAD+ENET_FRAMESIZE_MAXDATA+ENET_FRAMESIZE_TAIL
#define ENET_FRAMESIZE_VLAN       ENET_FRAMESIZE_HEAD_VLAN+ENET_FRAMESIZE_MAXDATA+ENET_FRAMESIZE_TAIL

/***************************************
**
** Ethernet protocols
**
*/
#define ENETPROT_LENGTH_TYPE_BOUNDARY ENET_FRAMESIZE_MAXDATA
#define ENETPROT_IP     0x800
#define ENETPROT_ARP    0x806
#define ENETPROT_8021Q  0x8100
#define ENETPROT_IP6    0x86DD


#define ENET_OPT_8023             0x0001
#define ENET_OPT_8021QTAG         0x0002
#define ENET_SETOPT_8021QPRIO(p)  (ENET_OPT_8021QTAG | (((uint_32)(p) & 0x7) << 2))
#define ENET_GETOPT_8021QPRIO(f)  (((f) >> 2) & 0x7)

#define eaddrcpy(p,x)   ((p)[0] = (x)[0], \
                         (p)[1] = (x)[1], \
                         (p)[2] = (x)[2], \
                         (p)[3] = (x)[3], \
                         (p)[4] = (x)[4], \
                         (p)[5] = (x)[5]  \
                        )
                        
#define htone(p,x) eaddrcpy(p,x)

#define ntohe(p,x)   ((x)[0] = (p)[0] & 0xFF, \
                      (x)[1] = (p)[1] & 0xFF, \
                      (x)[2] = (p)[2] & 0xFF, \
                      (x)[3] = (p)[3] & 0xFF, \
                      (x)[4] = (p)[4] & 0xFF, \
                      (x)[5] = (p)[5] & 0xFF  \
                     )
                     

typedef uchar   _enet_address[6];

/*
** Ethernet packet header
*/
typedef struct enet_header {
   _enet_address    DEST;
   _enet_address    SOURCE;
   uchar            TYPE[2];
} ENET_HEADER, _PTR_ ENET_HEADER_PTR;

typedef struct enet_8021qtag_header {
   uchar    TAG[2];
   uchar    TYPE[2];
} ENET_8021QTAG_HEADER, _PTR_ ENET_8021QTAG_HEADER_PTR;

typedef struct enet_8022_header {
   uchar    DSAP[1];
   uchar    SSAP[1];
   uchar    COMMAND[1];
   uchar    OUI[3];
   uchar    TYPE[2];
} ENET_8022_HEADER, _PTR_ ENET_8022_HEADER_PTR;


#endif
/* EOF */
