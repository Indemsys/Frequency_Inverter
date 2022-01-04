#include "app_cfg.h"

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Print_reset_source(void)
{
  RCM_MemMapPtr RCM = RCM_BASE_PTR;
  /* Determine the last cause(s) of reset */
  if ( RCM->SRS1 & BIT(2) )  printf_uart("Software Reset\r\n");
  if ( RCM->SRS1 & BIT(1) )  printf_uart("Core Lockup Event Reset\r\n");
  if ( RCM->SRS1 & BIT(0) )  printf_uart("JTAG Reset\r\n");
  if ( RCM->SRS0 & BIT(7) )	 printf_uart("Power-on Reset\r\n");
  if ( RCM->SRS0 & BIT(6) )	 printf_uart("External Pin Reset\r\n");
  if ( RCM->SRS0 & BIT(5) )	 printf_uart("Watchdog(COP) Reset\r\n");
  if ( RCM->SRS0 & BIT(2) )	 printf_uart("Loss of Clock Reset\r\n");
  if ( RCM->SRS0 & BIT(1) )	 printf_uart("Low-voltage Detect Reset\r\n");
  if ( RCM->SRS0 & BIT(0) )  printf_uart("LLWU Reset\r\n");
}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Print_Flash_identify(void)
{
  FTFE_MemMapPtr FTFE = FTFE_BASE_PTR;
  int i;

  printf_uart("P-Flash IFR dump:\r\n");
  for (i = 0; i < 8; i++)
  {
    __disable_interrupt();
    /* Write the flash FCCOB registers with the values for a read resource command */
    FTFE->FCCOB0 = 0x03;
    FTFE->FCCOB1 = 0x00;
    FTFE->FCCOB2 = 0x00;
    FTFE->FCCOB3 = 0x00 + i * 8;
    FTFE->FCCOB4 = 0x00;

    FTFE->FSTAT = BIT(5);
    /* All required FCCOBx registers are written, so launch the command */
    FTFE->FSTAT = BIT(7);

    /* Wait for the command to complete */
    while (!(FTFE->FSTAT & BIT(7)));
    __enable_interrupt();

    printf_uart(" %02X %02X %02X %02X %02X %02X %02X %02X\r\n", FTFE->FCCOB4, FTFE->FCCOB5, FTFE->FCCOB6, FTFE->FCCOB7, FTFE->FCCOB8, FTFE->FCCOB9, FTFE->FCCOBA, FTFE->FCCOBB);
  }

  /* Get the flash version ID */

  return;

  // Команда ниже вызывает Core Lockup Event Reset
  __disable_interrupt();
  /* Write the flash FCCOB registers with the values for a read resource command */
  FTFE->FCCOB0 = 0x03;
  FTFE->FCCOB1 = 0x00;
  FTFE->FCCOB2 = 0x00;
  FTFE->FCCOB3 = 0x00;
  FTFE->FCCOB4 = 0x01;

  FTFE->FSTAT = BIT(5);
  /* All required FCCOBx registers are written, so launch the command */
  FTFE->FSTAT = BIT(7);
  ;

  /* Wait for the command to complete */
  while (!(FTFE->FSTAT & BIT(7)));
  __enable_interrupt();

  printf_uart("FSTAT=%02X. Flash version ID %d.%d.%d.%d.%d.%d.%d.%d\r\n", FTFE->FSTAT, FTFE->FCCOB4, FTFE->FCCOB5, FTFE->FCCOB6, FTFE->FCCOB7, FTFE->FCCOB8, FTFE->FCCOB9, FTFE->FCCOBA, FTFE->FCCOBB);
  /* Clear all of the flags in the flash status register */
  FTFE->FSTAT = BIT(5);
  FTFE->FSTAT = BIT(7);
}
//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Print_CPU_identify(void)
{
  SIM_MemMapPtr  SIM  = SIM_BASE_PTR;
  unsigned int val;

  printf_uart("Chip: ");
  /* Determine the Kinetis family */
  switch ((SIM->SDID >> 4) & 0x7)
  {
  case 0x0:
    printf_uart("K10-");
    break;
  case 0x1:
    printf_uart("K20-");
    break;
  case 0x2:
    printf_uart("K30-");
    break;
  case 0x3:
    printf_uart("K40-");
    break;
  case 0x4:
    printf_uart("K60-");
    break;
  case 0x5:
    printf_uart("K70-");
    break;
  case 0x6:
    printf_uart("K50-");
    break;
  case 0x7:
    printf_uart("K53-");
    break;
  default:
    printf_uart("Unrecognized Kinetis family device. \r\n");
    break;
  }


  /* Determine the package size */
  switch (SIM->SDID & 0xF)
  {
  case 0x2:
    printf_uart(" 32pin , ");
    break;
  case 0x4:
    printf_uart(" 48pin , ");
    break;
  case 0x5:
    printf_uart(" 64pin , ");
    break;
  case 0x6:
    printf_uart(" 80pin , ");
    break;
  case 0x7:
    printf_uart(" 81pin , ");
    break;
  case 0x8:
    printf_uart(" 100pin, ");
    break;
  case 0x9:
    printf_uart(" 104pin, ");
    break;
  case 0xA:
    printf_uart(" 144pin, ");
    break;
  case 0xC:
    printf_uart(" 196pin, ");
    break;
  case 0xE:
    printf_uart(" 256pin, ");
    break;
  default:
    printf_uart("Unrecognized Kinetis package code. \r\n");
    break;
  }

  val = (SIM->SDID >> 12) & 0xF;
  printf_uart("Silicon rev.=%02X\r\n", val);


  /* Determine the P-flash size */
  switch ((SIM->FCFG1 >> 24) & 0x0F)
  {
  case 0x7:
    printf_uart("128 kBytes P-flash	\r\n");
    break;
  case 0x9:
    printf_uart("256 kBytes P-flash	\r\n");
    break;
  case 0xB:
    printf_uart("512 kBytes P-flash	\r\n");
    break;
  case 0xD:
    printf_uart("1024 kBytes P-flash	\r\n");
    break;
  case 0xF:
    printf_uart("1024 kBytes P-flash	\r\n");
    break;
  default:
    printf_uart("ERR!! Undefined P-flash size\r\n");
    break;
  }

  /* if part has FlexNVM determine the FlexNVM size*/
  switch ((SIM->FCFG1 >> 28) & 0xF)
  {
  case 0x0:
    printf_uart("0 kBytes of FlexNVM   \r\n");
    break;
  case 0x7:
    printf_uart("128 kBytes of FlexNVM \r\n");
    break;
  case 0x9:
    printf_uart("256 kBytes of FlexNVM \r\n");
    break;
  case 0xF:
    printf_uart("256 kBytes of FlexNVM \r\n");
    break;
  default:
    printf_uart("ERR!! Undefined FlexNVM size\r\n");
    break;
  }
  /* Determine the RAM size */
  switch ((SIM->SOPT1 >> 12) & 0xF)
  {
  case 0x5:
    printf_uart("32 kBytes of RAM  \n\n");
    break;
  case 0x7:
    printf_uart("64 kBytes of RAM  \n\n");
    break;
  case 0x8:
    printf_uart("96 kBytes of RAM  \n\n");
    break;
  case 0x9:
    printf_uart("128 kBytes of RAM \n\n");
    break;
  default:
    printf_uart("ERR!! Undefined RAM size \n\n");
    break;
  }

}

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------
void Print_USB_FS_regs(void)
{
  USB_MemMapPtr USB = USB0_BASE_PTR;

  printf_uart("USB0 registers -------------------------------\n\n");
  printf_uart("PERID       (Peripheral ID Register             ) = %02X  \r\n", USB->PERID);
  printf_uart("IDCOMP      (Peripheral ID Complement Register  ) = %02X  \r\n", USB->IDCOMP);
  printf_uart("REV         (Revision number                    ) = %02X  \r\n", USB->REV);
  printf_uart("ADDINFO     (Peripheral Additional info Register) = %02X  \r\n", USB->ADDINFO);
  printf_uart("OTGISTAT    (OTG Interrupt Status Register      ) = %02X  \r\n", USB->OTGISTAT);
  printf_uart("OTGICR      (OTG Interrupt Control Register     ) = %02X  \r\n", USB->OTGICR);
  printf_uart("OTGSTAT     (OTG Status Register                ) = %02X  \r\n", USB->OTGSTAT);
  printf_uart("OTGCTL      (OTG Control Register               ) = %02X  \r\n", USB->OTGCTL);
  printf_uart("ISTAT       (Interrupt Status Register          ) = %02X  \r\n", USB->ISTAT);
  printf_uart("INTEN       (Interrupt Enable Register          ) = %02X  \r\n", USB->INTEN);
  printf_uart("ERRSTAT     (Error Interrupt Status Register    ) = %02X  \r\n", USB->ERRSTAT);
  printf_uart("STAT        (Status Register                    ) = %02X  \r\n", USB->STAT);
  printf_uart("CTL         (Control Register                   ) = %02X  \r\n", USB->CTL);
  printf_uart("ADDR        (Address Register                   ) = %02X  \r\n", USB->ADDR);
  printf_uart("BDTPAGE1    (BDT Page Register 1                ) = %02X  \r\n", USB->BDTPAGE1);
  printf_uart("FRMNUML     (Frame Number Register Low          ) = %02X  \r\n", USB->FRMNUML);
  printf_uart("FRMNUMH     (Frame Number Register High         ) = %02X  \r\n", USB->FRMNUMH);
  printf_uart("TOKEN       (Token Register                     ) = %02X  \r\n", USB->TOKEN);
  printf_uart("SOFTHLD     (SOF Threshold Register             ) = %02X  \r\n", USB->SOFTHLD);
  printf_uart("BDTPAGE2    (BDT Page Register 2                ) = %02X  \r\n", USB->BDTPAGE2);
  printf_uart("BDTPAGE3    (BDT Page Register 3                ) = %02X  \r\n", USB->BDTPAGE3);
  printf_uart("USBCTRL     (USB Control Register               ) = %02X  \r\n", USB->USBCTRL);
  printf_uart("OBSERVE     (USB OTG Observe Register           ) = %02X  \r\n", USB->OBSERVE);
  printf_uart("CONTROL     (USB OTG Control Register           ) = %02X  \r\n", USB->CONTROL);
  printf_uart("USBTRC0     (USB Transceiver Control Register 0 ) = %02X  \r\n", USB->USBTRC0);
  printf_uart("USBFRMADJUST(Frame Adjust Register              ) = %02X  \r\n", USB->USBFRMADJUST);
  printf_uart("ENDPT0      (Endpoint0  Control Registers       ) = %02X  \r\n", USB->ENDPOINT[0].ENDPT);
  printf_uart("ENDPT1      (Endpoint1  Control Registers       ) = %02X  \r\n", USB->ENDPOINT[1].ENDPT);
  printf_uart("ENDPT2      (Endpoint2  Control Registers       ) = %02X  \r\n", USB->ENDPOINT[2].ENDPT);
  printf_uart("ENDPT3      (Endpoint3  Control Registers       ) = %02X  \r\n", USB->ENDPOINT[3].ENDPT);
  printf_uart("ENDPT4      (Endpoint4  Control Registers       ) = %02X  \r\n", USB->ENDPOINT[4].ENDPT);
  printf_uart("ENDPT5      (Endpoint5  Control Registers       ) = %02X  \r\n", USB->ENDPOINT[5].ENDPT);
  printf_uart("ENDPT6      (Endpoint6  Control Registers       ) = %02X  \r\n", USB->ENDPOINT[6].ENDPT);
  printf_uart("ENDPT7      (Endpoint7  Control Registers       ) = %02X  \r\n", USB->ENDPOINT[7].ENDPT);
  printf_uart("ENDPT8      (Endpoint8  Control Registers       ) = %02X  \r\n", USB->ENDPOINT[8].ENDPT);
  printf_uart("ENDPT9      (Endpoint9  Control Registers       ) = %02X  \r\n", USB->ENDPOINT[9].ENDPT);
  printf_uart("ENDPT10     (Endpoint10 Control Registers       ) = %02X  \r\n", USB->ENDPOINT[10].ENDPT);
  printf_uart("ENDPT11     (Endpoint11 Control Registers       ) = %02X  \r\n", USB->ENDPOINT[11].ENDPT);
  printf_uart("ENDPT12     (Endpoint12 Control Registers       ) = %02X  \r\n", USB->ENDPOINT[12].ENDPT);
  printf_uart("ENDPT13     (Endpoint13 Control Registers       ) = %02X  \r\n", USB->ENDPOINT[13].ENDPT); 
  printf_uart("ENDPT14     (Endpoint14 Control Registers       ) = %02X  \r\n", USB->ENDPOINT[14].ENDPT);
  printf_uart("ENDPT15     (Endpoint15 Control Registers       ) = %02X  \r\n", USB->ENDPOINT[15].ENDPT);
}
