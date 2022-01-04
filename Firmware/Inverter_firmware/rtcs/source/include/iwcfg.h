#ifndef __iwcfg_h__
#define __iwcfg_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2009 Freescale Semiconductor;
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
* $FileName: iwcfg.h$
* $Version : 3.6.3.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   Definitions for WIFI device configurations.
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <enet_wifi.h>


#define         MAX_RSSI_TABLE_SZ    5


typedef struct _iw_rssi_quanta{
  uint_8 max;
  uint_8 min;
} RSSI_QUANTA,* RSSI_QUANTA_PTR;


/***************************************
**
** Prototypes
**
*/
uint_32 iwcfg_set_essid
    (
        uint_32 dev_num,
        char_ptr essid
    );

uint_32 iwcfg_commit
    (
        uint_32 dev_num
    );

uint_32 iwcfg_set_mode
    (
        uint_32 dev_num,
        char_ptr mode
    );

uint_32 iwcfg_set_wep_key 
    (
        uint_32 dev_num,
        char_ptr wep_key,
        uint_32 key_len,
        uint_32 key_index
        
    );
    
uint_32 iwcfg_set_sec_type 
    (
        uint_32 dev_num,
        char_ptr  sec_type
    );

uint_32 iwcfg_set_passphrase
    (
        uint_32 dev_num,
        char_ptr passphrase
    );
uint_32 iwcfg_set_power 
    (
        uint_32 dev_num,
        uint_32 pow_val,
        uint_32 flags
        
    );

uint_32 iwcfg_set_scan
    (
        uint_32 dev_num,
        char_ptr ssid
    );

uint_32 iwcfg_get_essid
    (
        uint_32 dev_num,
        char_ptr essid /*[OUT]*/
    );

uint_32 iwcfg_get_mode
    (
        uint_32 dev_num,
        char_ptr essid /*[OUT]*/
    );
    
uint_32 iwcfg_get_sectype
    (
        uint_32 dev_num,
        char_ptr sectype /*[OUT]*/
    );  
uint_32 iwcfg_get_wep_key 
    (
        uint_32 dev_num,
        char_ptr wep_key,
        uint_32_ptr key_index
  
    );
uint_32 iwcfg_get_passphrase 
    (
        uint_32 dev_num,
        char_ptr passphrase
         
    );          
#endif
/* EOF */
