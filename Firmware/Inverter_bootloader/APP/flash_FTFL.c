/********************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
**************************************************************************/ /*!
  *
  * @file flash_FTFL.c
  *
  * @author
  *
  * @version
  *
  * @date
  *
  * @brief The file includes flash routines
  *
  *****************************************************************************/


#include "app_cfg.h"
#include <flash_FTFL.h> /* include flash driver header file */


/*******************************************************************************
 * Function:        Flash_Init
 *
 * Description:     Set the flash clock
 *
 * Returns:         none
 *
 * Notes:
 *
 *******************************************************************************/
void Flash_Init(void)
{

  /* checking access error */
  if ( FTFL_FSTAT & (FTFL_FSTAT_ACCERR_MASK | FTFL_FSTAT_FPVIOL_MASK | FTFL_FSTAT_RDCOLERR_MASK) )
  {
    /* clear error flag */
    FTFL_FSTAT |= (FTFL_FSTAT_ACCERR_MASK | FTFL_FSTAT_FPVIOL_MASK | FTFL_FSTAT_RDCOLERR_MASK);
  }
}

/*******************************************************************************
 * Function:        Flash_SectorErase
 *
 * Description:     erase a sector of the flash
 *
 * Returns:         Error Code
 *
 * Notes:
 *
 *******************************************************************************/
unsigned char Flash_SectorErase(INT32U addr)
{
  unsigned char ret = Flash_OK;

  /* Allocate space on stack to run flash command out of SRAM */
  /* wait till CCIF is set*/
  while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK))
  {;}

  /* Write command to FCCOB registers */
  FTFL_FCCOB0 = FLASH_CMD_SECTOR_ERASE;
  FTFL_FCCOB1 = (INT8U)(addr >> 16);
  FTFL_FCCOB2 = (INT8U)((addr >> 8) & 0xFF);
  FTFL_FCCOB3 = (INT8U)(addr & 0xFF);

  /* Launch command */
  SpSub();
  /* checking access error */
  if ( FTFL_FSTAT & FTFL_FSTAT_ACCERR_MASK )
  {
    /* clear error flag */
    FTFL_FSTAT |= FTFL_FSTAT_ACCERR_MASK;

    /* update return value*/
    ret |= Flash_FACCERR;
  }
  /* checking protection error */
  else if ( FTFL_FSTAT & FTFL_FSTAT_FPVIOL_MASK )
  {
    /* clear error flag */
    FTFL_FSTAT |= FTFL_FSTAT_FPVIOL_MASK;

    /* update return value*/
    ret |= Flash_FPVIOL;
  }
  else if ( FTFL_FSTAT & FTFL_FSTAT_RDCOLERR_MASK )
  {
    /* clear error flag */
    FTFL_FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;

    /* update return value*/
    ret |= Flash_RDCOLERR;
  }
  /* checking MGSTAT0 non-correctable error */
  else if ( FTFL_FSTAT & FTFL_FSTAT_MGSTAT0_MASK )
  {
    ret |= Flash_MGSTAT0;
  }
  /* function return */
  return  ret;
}

/*******************************************************************************
 * Function:        Flash_ByteProgram
 *
 * Description:     byte program the flash
 *
 * Returns:         Error Code
 *
 * Notes:
 *
 *******************************************************************************/
unsigned char Flash_ByteProgram(INT32U addr, INT8U *buff, INT32U sz)
{
  unsigned char res = Flash_OK;
 
  INT32U size_buffer;

  if ( sz == 0 )
  {
    return Flash_CONTENTERR;
  }
  else
  {
    size_buffer = (sz - 1) / 4 + 1;
  }
  
  while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK))  /* wait till CCIF is set*/
  {;}

  while ((size_buffer) && (res == Flash_OK))
  {
    /*FSL: for FTFL, 4-byte aligned, for FTFE, 8 byte aligned!*/

    /* Write command to FCCOB registers */
    FTFL_FCCOB0 = FLASH_CMD_PROGR_LWORD;
    FTFL_FCCOB1 = (INT8U)(addr >> 16);
    FTFL_FCCOB2 = (INT8U)((addr >> 8) & 0xFF);
    FTFL_FCCOB3 = (INT8U)(addr & 0xFF);
    FTFL_FCCOB4 = (INT8U)(*(buff + 3));
    FTFL_FCCOB5 = (INT8U)(*(buff + 2));
    FTFL_FCCOB6 = (INT8U)(*(buff + 1));
    FTFL_FCCOB7 = (INT8U)(*(buff + 0));
    FTFE_FCCOB8 = (INT8U)(*(buff + 7));
    FTFE_FCCOB9 = (INT8U)(*(buff + 6));
    FTFE_FCCOBA = (INT8U)(*(buff + 5));
    FTFE_FCCOBB = (INT8U)(*(buff + 4));
    SpSub(); /* Launch command */

    
    if ( FTFL_FSTAT & FTFL_FSTAT_ACCERR_MASK )       /* checking access error */
    {
      FTFL_FSTAT |= FTFL_FSTAT_ACCERR_MASK;          /* clear error flag */
      res |= Flash_FACCERR;                          /* update return value*/
    }
    else if ( FTFL_FSTAT & FTFL_FSTAT_FPVIOL_MASK )  /* checking protection error */
    {
      FTFL_FSTAT |= FTFL_FSTAT_FPVIOL_MASK;          /* clear error flag */
      res |= Flash_FPVIOL;                           /* update return value*/
    }
    else if ( FTFL_FSTAT & FTFL_FSTAT_RDCOLERR_MASK )
    {
      FTFL_FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;        /* clear error flag */
      res |= Flash_RDCOLERR;                         /* update return value*/
    }
    else if ( FTFL_FSTAT & FTFL_FSTAT_MGSTAT0_MASK ) /* checking MGSTAT0 non-correctable error */
    {
      res |= Flash_MGSTAT0;
    }
    
    size_buffer -= 2;  /* decrement byte count */
    buff += 8;
    addr += 8;
  }
  /* function return */
  return  res;
}

/*******************************************************************************
 * Function:        SpSub
 *
 * Description:     Execute the Flash write while running out of SRAM
 *
 * Returns:         none
 *
 * Notes:
 *
 *******************************************************************************/
__ramfunc void SpSub(void)
{
  /* Launch command */
  FTFL_FSTAT |= FTFL_FSTAT_CCIF_MASK;
  /* wait for command completion */
  while (!(FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK))
  {};
}

