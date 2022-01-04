
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: crc_kn.c$
* $Version : 3.8.1.1$
* $Date    : Aug-10-2012$
*
* Comments:
*
*   The file contains the functions which use to calculate CRC 
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "crc_kn.h"


/********************************************************************/
uint_32 CRC_Config(uint_32 poly,uint_32 tot,uint_32 totr,uint_32 fxor,uint_32 tcrc)
{
  CRC_MemMapPtr CRC_ADDR = CRC_BASE_PTR ;   
  uint_32       error = CRC_ERR_SUCCESS;
     
/*
*  Configure CRC_CTRL Register
*/
  if( tot > 3 ) 
  {
	  	error = CRC_ERR_TOT_VAL;
  }
  else if( totr > 3 ) 
  {
  		error = CRC_ERR_TOTR_VAL;
  }
  else if( fxor > 1 ) 
  {
  		error = CRC_ERR_FXOR_VAL;
  }
  else if( tcrc > 1 ) 
  {
  		error = CRC_ERR_TCRC_VAL;
  } 
  else 
  {
  		CRC_ADDR->CTRL = ((tot<<30)|(totr<<28)|(fxor<<26)|(tcrc<<24));
  }

/*  
*   Configure Polynomial Register
*/   
  CRC_ADDR->GPOLY = poly;

 return(error);
}


 /********************************************************************  
  * 16bit data CRC Calculation
 ********************************************************************/  
uint_32 CRC_Cal_16(uint_32 seed,uint_8 *msg, uint_32 sizeBytes)
{
   CRC_MemMapPtr CRC_ADDR = CRC_BASE_PTR;   
  uint_32 	ctrl_reg,data_out,data_in;
  uint_8  	*pCRCBytes;
  uint_32 	sizeWords;
  uint_32 	i,j;
  
/*
*	Input seed, Set WaS=1
*/
  ctrl_reg = CRC_ADDR->CTRL;
  CRC_ADDR->CTRL = ctrl_reg | 0x02000000;
  CRC_ADDR->CRC  = seed;
/*  
*   Input data, Set WaS=0
*/
  CRC_ADDR->CTRL = ctrl_reg & 0xFD000000;
  
/*
*   Wait for calculation completion
*/
  sizeWords = sizeBytes>>1;
  j = 0;
  for(i=0;i<sizeWords;i++){ 
      data_in = (msg[j] << 8) | (msg[j+1]);
      j += 2;
      CRC_ADDR->ACCESS16BIT.CRCL = data_in; 
  }
  if(j<sizeBytes)
  { 
     pCRCBytes = (uint_8*)&(CRC_ADDR->CRC);
     *pCRCBytes++ = msg[j];
  }
  data_out=CRC_ADDR->CRC;

 return(data_out);
}
/********************************************************************
* 32bit data CRC Calculation
********************************************************************/  
uint_32 CRC_Cal_32(uint_32 seed,uint_8 *msg, uint_32 sizeBytes)
{
   CRC_MemMapPtr CRC_ADDR = CRC_BASE_PTR;   
  uint_32  ctrl_reg,data_out,data_in;
  uint_32  sizeDwords;
  uint_8   *pCRCBytes;
  int i,j;
/*  
*   Input seed, Set WaS=1
*/
  ctrl_reg = CRC_ADDR->CTRL;
  CRC_ADDR->CTRL = ctrl_reg | 0x02000000;
  CRC_ADDR->CRC = seed;
 
/* 
*   Input data, Set WaS=0
*/
  CRC_ADDR->CTRL = ctrl_reg & 0xFD000000;
  
/*  
*   Wait for calculation completion
*/
  
  sizeDwords = sizeBytes>>2;
  j = 0;
  for(i=0;i<sizeDwords;i++){ 
      data_in = ((msg[j] << 24) | (msg[j+1] << 16) | (msg[j+2] << 8) | msg[j+3]);
      j += 4;
      CRC_ADDR->CRC = data_in; 
  }
  if(j<sizeBytes)
  {
    pCRCBytes = (uint8_t*)&CRC_ADDR->CRC;

#if  defined(BYTE_ENABLES_1_2_4_8)   
/*
*  write single byte  
*/
    for(;j<sizeBytes;j++)
    {     
       *pCRCBytes++ = msg[j];
    }
#elif  defined(BYTE_ENABLES_3_6_C)     
/*
*  write two bytes
*/
    data_in = 0;
    i = 0;
    for(;j<sizeBytes;j++)
    {     
      data_in = (data_in <<8) | msg[j];
      i++;
      if(i==2)
      {
        i = 0;
        CRC_ADDR->ACCESS16BIT.CRCL = data_in;
      }
    }
    if(i==1)
    {
    	 CRC_ADDR->ACCESS16BIT.CRCLL = data_in; 	
    }
#elif  defined(BYTE_ENABLES_7_E)        
/*
* write three bytes
*/
    data_in = 0;
    i = 0;
    for(;j<sizeBytes;j++)
    {     
      data_in = (data_in <<8) | msg[j];
      i++;
      if(i==3)
      {
        i = 0;
        /* write first  char */
        CRC_ADDR->ACCESS16BIT.CRCHL = (data_in>>16) & 0xff;
        /*  write last two chars */
        CRC_ADDR->ACCESS16BIT.CRCL  = data_in & 0x00ffff;     /*  write low word */
       }
    }
    if( i == 2)
    {
       CRC_ADDR->ACCESS16BIT.CRCL = (data_in); /* write last byte */
    }
    else if (i == 1)
    {
       CRC_ADDR->ACCESS16BIT.CRCLL = data_in; /* write last byte      */
    }
#else                          /* write low byte only */
    for(;j<sizeBytes;j++)
    {     
       *pCRCBytes = msg[j];
    }
#endif            
  }
  data_out=CRC_ADDR->CRC;

 return(data_out);
}
 /********************************************************************/ 
uint_32 CRC_init(void)
{
	/* Enable clock gating to CRC */
#if defined BSP_TWRMCF51JF || defined BSP_TWRMCF51QM 
	SIM_SCGC3 |= SIM_SCGC3_CRC_MASK;
#else	
	SIM_SCGC6 |= SIM_SCGC6_CRC_MASK;
#endif
	return 0;
}

