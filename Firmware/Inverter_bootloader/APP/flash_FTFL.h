#ifndef _FLASH_FTFL_H_
#define _FLASH_FTFL_H_


/*FSL: new Kinetis/ColdFire+ devices might use this new FTFE implementation instead of FTFL*/

#define     FTFL_FSTAT_ACCERR_MASK       FTFE_FSTAT_ACCERR_MASK
#define     FTFL_FSTAT_FPVIOL_MASK       FTFE_FSTAT_FPVIOL_MASK
#define     FTFL_FSTAT_RDCOLERR_MASK     FTFE_FSTAT_RDCOLERR_MASK
#define     FTFL_FSTAT_CCIF_MASK         FTFE_FSTAT_CCIF_MASK
#define     FTFL_FSTAT_MGSTAT0_MASK      FTFE_FSTAT_MGSTAT0_MASK
#define     FTFL_FSTAT                   FTFE_FSTAT
#define     FTFL_FCCOB0                  FTFE_FCCOB0
#define     FTFL_FCCOB1                  FTFE_FCCOB1
#define     FTFL_FCCOB2                  FTFE_FCCOB2
#define     FTFL_FCCOB3                  FTFE_FCCOB3
#define     FTFL_FCCOB4                  FTFE_FCCOB4
#define     FTFL_FCCOB5                  FTFE_FCCOB5
#define     FTFL_FCCOB6                  FTFE_FCCOB6
#define     FTFL_FCCOB7                  FTFE_FCCOB7


/* error code */
#define Flash_OK           0x00
#define Flash_FACCERR      0x01
#define Flash_FPVIOL       0x02
#define Flash_MGSTAT0	   0x04
#define Flash_RDCOLERR	   0x08
#define Flash_NOT_ERASED   0x10
#define Flash_CONTENTERR   0x11

/* flash commands */
#define FLASH_CMD_PROGR_LWORD  	0x07
#define FLASH_CMD_SECTOR_ERASE  0x09


void Flash_Init(void);
unsigned char Flash_SectorErase(INT32U addr);
unsigned char Flash_ByteProgram(INT32U addr, INT8U *buff, INT32U size);

__ramfunc void SpSub(void);

#endif /*_FLASH_FTFL_H_*/
