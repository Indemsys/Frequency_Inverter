#include "app_cfg.h"
#include "UART.h"
#include "Cpu.h"

//-------------------------------------------------------------------------------------------------------
//   sysclk - частота ядра в килогерцах
//-------------------------------------------------------------------------------------------------------
void UART_Init(UART_MemMapPtr UART, int sysclk, int baud)
{
  register INT16U sbr, brfa;
  INT8U temp;

  /* Enable the clock to the selected UART */
  if ( UART == UART0_BASE_PTR ) SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
  else if ( UART == UART1_BASE_PTR ) SIM_SCGC4 |= SIM_SCGC4_UART1_MASK;
  else if ( UART == UART2_BASE_PTR ) SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
  else if ( UART == UART3_BASE_PTR ) SIM_SCGC4 |= SIM_SCGC4_UART3_MASK;
  else if ( UART == UART4_BASE_PTR ) SIM_SCGC1 |= SIM_SCGC1_UART4_MASK;
  else SIM_SCGC1 |= SIM_SCGC1_UART5_MASK;

  /* Make sure that the transmitter and receiver are disabled while we  change settings. */
  UART->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

  /* Configure the UART for 8-bit mode, no parity */
  UART->C1 = 0;    /* We need all default settings, so entire register is cleared */

  /* Calculate baud settings */
  sbr = (INT16U)((sysclk * 1000) / (baud * 16));

  /* Save off the current value of the UARTx_BDH except for the SBR field */
  temp = UART->BDH & ~(UART_BDH_SBR(0x1F));

  UART->BDH = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
  UART->BDL = (INT8U)(sbr & UART_BDL_SBR_MASK);

  /* Determine if a fractional divider is needed to get closer to the baud rate */
  brfa = (((sysclk * 32000) / (baud * 16)) - (sbr * 32));

  /* Save off the current value of the UARTx_C4 register except for the BRFA field */
  temp = UART->C4 & ~(UART_C4_BRFA(0x1F));

  UART->C4 = temp |  UART_C4_BRFA(brfa);

  /* Enable receiver and transmitter */
  UART->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
}


/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void Send_byte(unsigned char byte)
{
   UART0_putchar(byte);
}

//-------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------
void UART0_putchar(char ch)
{
  /* Wait until space is available in the FIFO */
  while (!(UART0_BASE_PTR->S1 & UART_S1_TDRE_MASK))
  {
    __no_operation();
  }

  /* Send the character */
  UART0_BASE_PTR->D = (INT8U)ch;
}

//-------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------
int Wait_byte(unsigned char *ptr_c, unsigned int timeout)
{
  int cnt = 0;
  do
  {

    if (UART0_BASE_PTR->S1 & UART_S1_RDRF_MASK)
    {
      *ptr_c = UART0_BASE_PTR->D;
      return SUCCESS;
    }  
    cnt++;
    if ( cnt>500 )
    {
      timeout--;
      cnt = 0;
    }
    DELAY_2us;
  }
  while (timeout != 0);
  
  return ERROR;
}  

//-------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------
char UART0_getchar(void)
{
  /* Wait until character has been received */
  while (!(UART0_BASE_PTR->S1 & UART_S1_RDRF_MASK))
  {;}

  /* Return the 8-bit data from the receiver */
  return UART0_BASE_PTR->D;
}

//-------------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------------
void Out_char(char ch)
{
  UART0_putchar(ch);
}
