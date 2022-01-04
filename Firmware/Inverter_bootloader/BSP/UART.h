#ifndef UART0_H_
  #define UART0_H_

  #include "PE_Error.h"
  #include "Cpu.h"


void UART_Init2(void);
void UART_Init(UART_MemMapPtr uartch, int sysclk, int baud);

void UART0_putchar(char ch);
char UART0_getchar(void);
void Out_char(char ch);

void Send_byte(unsigned char byte);
int Wait_byte(unsigned char *ptr_c, unsigned int timeout);

#endif

