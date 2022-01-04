/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
* $FileName: phy_ksz8041.h$
* $Version : 3.0.3.0$
* $Date    : Jul-8-2009$
*
* Comments:
*
*   This file contains definitions that belongs to the PHY chip
*   ksz8041.
*
*END************************************************************************/
#ifndef _phy_ksz8041_h_
#define _phy_ksz8041_h_ 1

#ifdef __cplusplus
extern "C" {
#endif

/*
** Register definitions for the PHY.
*/

enum phy_reg {
   PHY_MII_REG_CR       = 0,  // Control Register
   PHY_MII_REG_SR       = 1,  // Status Register
   PHY_MII_REG_PHYIR1   = 2,  // PHY Identification Register 1
   PHY_MII_REG_PHYIR2   = 3,  // PHY Identification Register 2
   PHY_MII_REG_ANAR     = 4,  // A-N Advertisement Register
   PHY_MII_REG_ANLPAR   = 5,  // A-N Link Partner Ability Register
   PHY_MII_REG_ANER     = 6,  // A-N Expansion Register
   PHY_MII_REG_ANNPTR   = 7,  // A-N Next Page Transmit Register
   PHY_MII_REG_ANLPRNPR = 8,  // A-N Link Partner Received Next Page Reg.

   PHY_MII_REG_RXER     = 0x15,  // RX Error counter 
   PHY_MII_REG_ICS      = 0x1b,  //  Interrupt Control Status Register 
   PHY_MII_REG_PHYC1    = 0x1e,  // Phy control 1
   PHY_MII_REG_PHYC2    = 0x1f   // Phy control 2
};


// values for PHY_MII_REG_CR Status Register

#define PHY_MII_REG_CR_AN_ENABLE   0x1000
#define PHY_MII_REG_CR_LOOP        0x4000
#define PHY_MII_REG_CR_RESET       0x8000


// values for PHY_MII_REG_SR Status Register

#define PHY_MII_REG_SR_EXTCAP        0x0001
#define PHY_MII_REG_SR_JABBER        0x0002
#define PHY_MII_REG_SR_LINK_STATUS   0x0004
#define PHY_MII_REG_SR_AN_ABLE       0x0008 // Auto-negotiate ability
#define PHY_MII_REG_SR_REMOTE_FAULT  0x0010
#define PHY_MII_REG_SR_AN_COMPLETE   0x0020 // Auto-negotiate completed


// values for PHY_MII_REG_SR2 Status Register

#define PHY_MII_REG_PHYC2_OP_MODE_MASK    0x001c 
#define PHY_MII_REG_PHYC2_OP_MODE_AN      0x0000 
#define PHY_MII_REG_PHYC2_OP_MODE_10_HD   0x0004 
#define PHY_MII_REG_PHYC2_OP_MODE_100_HD  0x0008 
#define PHY_MII_REG_PHYC2_OP_MODE_10_FD   0x0014 
#define PHY_MII_REG_PHYC2_OP_MODE_100_FD  0x0018 


extern const ENET_PHY_IF_STRUCT phy_ksz8041_IF;
 
#define MII_TIMEOUT                     (0x10000)

#ifdef __cplusplus
}
#endif

#endif /* _phy_ksz8041_h_ */
/* EOF */
