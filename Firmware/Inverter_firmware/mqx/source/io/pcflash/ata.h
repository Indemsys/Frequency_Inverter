#ifndef _ata_h_
#define _ata_h_
/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: ata.h$
* $Version : 3.8.6.0$
* $Date    : Aug-1-2011$
*
* Comments:
*
*   This file contains 
*
*END************************************************************************/


/* -------------------------------------------------------------------------*/
/*
**                              CONSTANT DEFINITIONS
*/

/* The size of one sector (in bytes), as defined by the ATA standard */
#define ATA_SECTOR_SIZE       512

#define ATA_REG_SIZE          (15)

/*
 * Byte Offsets to ATA Register Set
 */
#define ATA_REG_BASE          (0)
#define ATA_ERROR             (1)   /* read only */
#define ATA_FEATURES          (1)   /* write only */
#define ATA_SECTOR_COUNT      (2)
#define ATA_SECTOR            (3)
#define ATA_CYLINDER_LOW      (4)
#define ATA_CYLINDER_HIGH     (5)
#define ATA_CARD_DRIVE_HEAD   (6)
#define ATA_STATUS            (7)   /* read only: reading clears interrupt */
#define ATA_COMMAND           (7)   /* write only */
#define ATA_DUP_RDE_DATA      (8)
#define ATA_DUP_RDO_DATA      (9)
#define ATA_ALT_STATUS_CMD    (0xe) /* reading does NOT clear interrupt */
#define ATA_DEV_CNTRL         (0xe) /* write only */
#define ATA_DATA_BUF          (0x400)

/*
 * Word Offsets to ATA Register Set (16-bit access)
 */
#define ATA_REG16_SECTOR      (1)
#define ATA_REG16_CYLINDER    (2)
#define ATA_REG16_DHC         (3)
#define ATA_REG16_STATUS      (3) /* read only */
#define ATA_REG16_DATA        (4)
#define ATA_REG16_ERROR       (6) /* high byte */
#define ATA_REG16_DEV_CNTRL   (7)


/*
 * ATA Commands -- See ATA Command/Status Register
 */
#define ATA_NOP                                    (0x00)
#define ATA_RECALIBRATE                            (0x10) /* 0x1x */
#define ATA_READ_SECTOR_WITH_RETRY                 (0x20)
#define ATA_READ_SECTOR                            (0x21)
#define ATA_READ_LONG_WITH_RETRY                   (0x22)
#define ATA_READ_LONG                              (0x23)
#define ATA_WRITE_SECTOR_WITH_RETRY                (0x30)
#define ATA_WRITE_SECTOR                           (0x31)
#define ATA_WRITE_LONG_WITH_RETRY                  (0x32)
#define ATA_WRITE_LONG                             (0x33)
#define ATA_WRITE_VERIFY                           (0x3C)
#define ATA_READ_VERIFY_SECTOR_WITH_RETRY          (0x40)
#define ATA_READ_VERIFY_SECTOR                     (0x41)
#define ATA_FORMAT_TRACK                           (0x50)
#define ATA_SEEK                                   (0x70) /* 0x7x */
#define ATA_VENDOR_SPECIFIC_8X                     (0x80) /* 0x8x */
#define ATA_EXECUTE_DEVICE_DIAGNOSTIC              (0x90)
#define ATA_INITIALIZE_DEVICE_PARAMETERS           (0x91)
#define ATA_DOWNLOAD_MICROCODE                     (0x92)
#define ATA_VENDOR_SPECIFIC_9A                     (0x9A)
#define ATA_VENDOR_SPECIFIC_C0                     (0xC0)
#define ATA_VENDOR_SPECIFIC_C1                     (0xC1)
#define ATA_VENDOR_SPECIFIC_C2                     (0xC2)
#define ATA_VENDOR_SPECIFIC_C3                     (0xC3)
#define ATA_READ_MULTIPLE                          (0xC4)
#define ATA_WRITE_MULTIPLE                         (0xC5)
#define ATA_SET_MULTIPLE_MODE                      (0xC6)
#define ATA_READ_DMA_WITH_RETRY                    (0xC8)
#define ATA_READ_DMA                               (0xC9)
#define ATA_WRITE_DMA_WITH_RETRY                   (0xCA)
#define ATA_WRITE_DMA                              (0xCB)
#define ATA_ACKNOWLEDGE_MEDIA_CHANGE               (0xDB)
#define ATA_BOOT_POST_BOOT                         (0xDC)
#define ATA_BOOT_PRE_BOOT                          (0xDD)
#define ATA_DOOR_LOCK                              (0xDE)
#define ATA_DOOR_UNLOCK                            (0xDF)
#define ATA_STANDBY_IMMEDIATE                      (0xE0)   /* 94h */
#define ATA_IDLE_IMMEDIATE                         (0xE1)   /* 95h */
#define ATA_STANDBY                                (0xE2)   /* 96h */
#define ATA_IDLE                                   (0xE3)   /* 97h */
#define ATA_READ_BUFFER                            (0xE4)
#define ATA_CHECK_POWER_MODE                       (0xE5)   /* 98h */
#define ATA_SLEEP                                  (0xE6)   /* 99h */
#define ATA_WRITE_BUFFER                           (0xE8)
#define ATA_WRITE_SAME                             (0xE9)
#define ATA_IDENTIFY_DEVICE                        (0xEC)
#define ATA_MEDIA_EJECT                            (0xED)
#define ATA_SET_FEATURES                           (0xEF)
#define ATA_VENDOR_SPECIFIC_FX                     (0xf0)  /* 0xfx */  

/*
 * Defines for the ATA drive/head register
 */
#define ATA_DRIVE_REG_LBA       (0x40)
#define ATA_DRIVE_REG_CARD0     (0x10)
#define ATA_DRIVE_REG_ADDR_MASK (0x0F)

/*
 * Defines for the ATA status register
 */
#define ATA_STAT_BSY   (0x80)
#define ATA_STAT_DRDY  (0x40)
#define ATA_STAT_DWF   (0x20)
#define ATA_STAT_DSC   (0x10)
#define ATA_STAT_DRQ   (0x08)
#define ATA_STAT_CORR  (0x04)
#define ATA_STAT_IDX   (0x02)
#define ATA_STAT_ERR   (0x01)

/*
 * Defines for the ATA error register
 */
#define ATA_ERR_RES    (0x80)
#define ATA_ERR_UNC    (0x40)
#define ATA_ERR_MC     (0x20)
#define ATA_ERR_IDNF   (0x10)
#define ATA_ERR_MCR    (0x08)
#define ATA_ERR_ABRT   (0x04)
#define ATA_ERR_TKONF  (0x02)
#define ATA_ERR_AMNF   (0x01)

#define ATA_ERR_MASK \
   ( ATA_ERR_RES | ATA_ERR_UNC | ATA_ERR_IDNF | ATA_ERR_ABRT | \
     ATA_ERR_TKONF | ATA_ERR_AMNF )

/*
 * Defines for the ATA device control register
 */
#define ATA_DEV_CNTL_SRST (0x04)

/*
 * Busy timeout values
 */

/* The maximum time to wait for reset to complete in seconds */
#define ATA_RST_TIMEOUT (31)

/* Default timeout in seconds */
#define ATA_BSY_DEFAULT_TIMEOUT (5)


/* -------------------------------------------------------------------------*/
/*
**                              DATATYPE DECLARATIONS
*/

/*
** ATA DATA RECORD STRUCT
*/
typedef struct ata_data_record_struct {
    uint_32    CARD_DATA;  /* PC Card Data */
    uchar      DRIVE;     /* Drive 0 -- Card A; Drive 1 Card B */
} ATA_DATA_RECORD_STRUCT, _PTR_ ATA_DATA_PTR ;

/*
** ATA IDENTIFY DEVICE RESULTS STRUCTURE
*/
typedef struct ata_identify_device_struct {
   uint_16     GENERAL_CONFIG;           /* Word 0 */
   uint_16     NUMBER_CYLINDERS;         /* Word 1 */
   uint_16     RESERVED1;                /* Word 2 */
   uint_16     NUMBER_HEADS;             /* Word 3 */
   uint_16     VENDOR1;                  /* Word 4 */
   uint_16     VENDOR2;                  /* Word 5 */
   uint_16     SECTORS_PER_TRACK;        /* Word 6 */
   uint_16     VENDOR3[3];               /* Word 7-9 */
   uint_16     SERIAL_NUMBER[10];        /* Word 10-19 */
   uint_16     VENDOR4;                  /* Word 20 */
   uint_16     VENDOR5;                  /* Word 21 */
   uint_16     BYTES_AVAIL_RW_LONG;      /* Word 22 */
   uint_16     FIRMWARE_REV[4];          /* Word 23-26 */
   uint_16     MODEL_NUMBER[20];         /* Word 27-46 */
   uint_16     VENDOR_MAX_SEC_NUM;       /* Word 47 */
   uint_16     RESERVED2;                /* Word 48 */
   uint_16     CAPABILITIES;             /* Word 49 */
   uint_16     RESERVED3;                /* Word 50 */
   uint_16     PIO_TIMING_MODE;          /* Word 51 */
   uint_16     DMA_TIMING_MODE;          /* Word 52 */
   uint_16     FIELD_64_70_VALID;        /* Word 53 */
   uint_16     CURRENT_CYLS;             /* Word 54 */
   uint_16     CURRENT_HEADS;            /* Word 55 */
   uint_16     CURRENT_SECTS_PER_TR;     /* Word 56 */
   uint_16     CURRENT_SECT_CAPACITY[2]; /* Word 57-58 */
   uint_16     MULTIPLE_SECTOR_SETTING;  /* Word 59 */
   uint_16     TOTAL_SECTORS_LBA[2];     /* Word 60-61 */
   uint_16     SINGLE_WRD_DMA;           /* Word 62 */
   uint_16     MULTI_WRD_DMA;            /* Word 63 */
   uint_16     ADVANCED_PIO_MODE;        /* Word 64 */
   uint_16     MIN_MW_DMA_CYCLE_TIME;    /* Word 65 */
   uint_16     REC_MW_DMA_CYCLE_TIME;    /* Word 66 */
   uint_16     MIN_PIO_TIME_NO_FLOW;     /* Word 67 */
   uint_16     MIN_PIO_TIME_IORDY_FLOW;  /* Word 68 */
   uint_16     RESERVED4[2];             /* Word 69-70 */
   uint_16     RESERVED5[57];            /* Word 71-127 */
   uint_16     VENDOR6[32];              /* Word 128-159 */
   uint_16     RESERVED6[96];            /* Word 160-255 */
} ATA_IDENTIFY_DEVICE_STRUCT, _PTR_ ATA_IDENTIFY_DEVICE_STRUCT_PTR;

#endif
