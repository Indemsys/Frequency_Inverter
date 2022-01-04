#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include "App.h"


/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void Set_VIC_chanel(unsigned char irq_ch, unsigned char preemp_prio, unsigned char sub_prio)
{
  NVIC_MemMapPtr NVIC = NVIC_BASE_PTR;
  SCB_MemMapPtr  SCB  = SystemControl_BASE_PTR;
  unsigned int prio;

  
  irq_ch -= 16; // Конвертировать индекс в массиве векторов в номер прерывания для регистров NVIC 

  /* Compute the Corresponding IRQ Priority --------------------------------*/
  prio = (SCB->AIRCR  >> 8) & 0x07; // Определим сколько бит выделяется под приоритет

  // Используються только 4-е старших бита
  switch (prio)
  {
  case 0:
    // bxxxxxxx.y
    prio = preemp_prio;
    break;
  case 1:
    // bxxxxxx.yy
    prio = preemp_prio;
    break;
  case 2:
    // bxxxxx.yyy
    prio = preemp_prio;
    break;
  case 3:
    // bxxxx.yyyy
    prio = preemp_prio;
    break;
  case 4:
    // bxxx.yyyyy
    prio = (preemp_prio << 1) | (sub_prio & 0x01);
    break;
  case 5:
    // bxx.yyyyyy
    prio = (preemp_prio << 2) | (sub_prio & 0x03);
    break;
  case 6:
    // bx.yyyyyyy
    prio = (preemp_prio << 3) | (sub_prio & 0x07);
    break;
  case 7:
    // b.yyyyyyyy
    prio = sub_prio & 0x0F;
    break;
  }

  prio = prio << 4;                 // Установим приоритет в нужной позиции в 8-и разрядном регистре с учетом имплементации только 4-х старших битов
  NVIC->IP[irq_ch] = prio;          // Записать в контроллер VIC

  /* Enable the Selected IRQ Channels --------------------------------------*/
  NVIC->ISER[(irq_ch >> 0x05)] = 0x01 << (irq_ch & 0x1F);
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void Disable_VIC_chanel(unsigned char irq_ch)
{
  NVIC_MemMapPtr NVIC = NVIC_BASE_PTR;
  /* Disable the Selected IRQ Channels -------------------------------------*/
  NVIC->ICER[(irq_ch >> 0x05)] = 0x01 << (irq_ch & 0x1F);
}

