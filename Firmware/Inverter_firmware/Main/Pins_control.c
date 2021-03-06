#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include "App.h"

/*-------------------------------------------------------------------------------------------------------------
  Установка состояния светодиодов
  led_num - номер светодиода (1...3)
  state   - состояние ( 1 - включен, 0 - выключен)
-------------------------------------------------------------------------------------------------------------*/
void Led_control(int led_num, int state)
{
  switch (led_num)
  {
  case 1:
    if ( (state & BIT(0)) != 0 ) PTA_BASE_PTR->PCOR = LSHIFT(1, 24);
    else PTA_BASE_PTR->PSOR = LSHIFT(1, 24);
    break;
  case 2:
    if ( (state & BIT(0)) != 0 ) PTA_BASE_PTR->PCOR = LSHIFT(1, 25);
    else PTA_BASE_PTR->PSOR = LSHIFT(1, 25);
    break;
  case 3:
    if ( (state & BIT(0)) != 0 ) PTA_BASE_PTR->PCOR = LSHIFT(1, 26);
    else PTA_BASE_PTR->PSOR = LSHIFT(1, 26);
    break;
  }
}

/*-------------------------------------------------------------------------------------------------------------
  1 - Вентилятор включен
  0 - Вентилятор выключен
-------------------------------------------------------------------------------------------------------------*/
void Fun_control(int state)
{
  if ( state == 0 ) PTE_BASE_PTR->PCOR = LSHIFT(1, 7);
  else PTE_BASE_PTR->PSOR = LSHIFT(1, 7);

}

/*-------------------------------------------------------------------------------------------------------------
  Возвращаем 1 если вентилятор включен
-------------------------------------------------------------------------------------------------------------*/
int Fun_state(void)
{
  if ( GPIOE_PDIR & BIT(7) ) return 1;
  else return 0;
}

/*-------------------------------------------------------------------------------------------------------------
  Управление регистром сигналов IGBT модуля   
  1 - Буффер включен, IGBT подключен к микроконтроллеру
  0 - Буффер выключен, IGBT отключен от микроконтроллера
-------------------------------------------------------------------------------------------------------------*/
void IGBT_reg_control(int state)
{
  if ( state == 1 ) PTA_BASE_PTR->PCOR = LSHIFT(1, 13);
  else PTA_BASE_PTR->PSOR = LSHIFT(1, 13);
}


/*-------------------------------------------------------------------------------------------------------------
  1 - NTC зашунтирован
  0 - NTC включен в цепь заряда
-------------------------------------------------------------------------------------------------------------*/
void NTC_bypass_control(int state)
{
  if ( state == 0 ) PTC_BASE_PTR->PCOR = LSHIFT(1, 13);
  else PTC_BASE_PTR->PSOR = LSHIFT(1, 13);
}

/*-------------------------------------------------------------------------------------------------------------
  Active LOW Reset Signal
-------------------------------------------------------------------------------------------------------------*/
void LCD_RST(int state)
{
  if ( state == 0 ) PTD_BASE_PTR->PCOR = LSHIFT(1, 11);
  else PTD_BASE_PTR->PSOR = LSHIFT(1, 11);
}

/*-------------------------------------------------------------------------------------------------------------
  Serial clock
-------------------------------------------------------------------------------------------------------------*/
void LCD_SCL(int state)
{
  if ( state == 0 ) PTD_BASE_PTR->PCOR = LSHIFT(1, 12);
  else PTD_BASE_PTR->PSOR = LSHIFT(1, 12);
}
/*-------------------------------------------------------------------------------------------------------------
  Input data
-------------------------------------------------------------------------------------------------------------*/
void LCD_SI(int state)
{
  if ( state == 0 ) PTD_BASE_PTR->PCOR = LSHIFT(1, 13);
  else PTD_BASE_PTR->PSOR = LSHIFT(1, 13);
}
/*-------------------------------------------------------------------------------------------------------------
  Register Select Signal. RS=0: instruction; RS=1: data
-------------------------------------------------------------------------------------------------------------*/
void LCD_RS(int state)
{
  if ( state == 0 ) PTD_BASE_PTR->PCOR = LSHIFT(1, 14);
  else PTD_BASE_PTR->PSOR = LSHIFT(1, 14);
}
/*-------------------------------------------------------------------------------------------------------------
  Active LOW Chip Select signal
-------------------------------------------------------------------------------------------------------------*/
void LCD_SCB(int state)
{
  if ( state == 0 ) PTD_BASE_PTR->PCOR = LSHIFT(1, 15);
  else PTD_BASE_PTR->PSOR = LSHIFT(1, 15);
}



/*-------------------------------------------------------------------------------------------------------------
  Возвращаем 1 если PWM разрешен
-------------------------------------------------------------------------------------------------------------*/
int Pin_PWM_OE_state(void)
{
  if ( GPIOA_PDIR & BIT(12) ) return 0;
  else return 1;
}

/*-------------------------------------------------------------------------------------------------------------
  Процедура обслуживания прерывания от порта A
  Используеться для слежения за состоянием сигнала PWM_OE
-------------------------------------------------------------------------------------------------------------*/
static void PORTA_isr(pointer user_isr_ptr)
{
  if ( PORTA_PCR(12) & BIT(24) )
  {
    // Прерывание от сигнала PWM_OE
    PORTA_PCR(12) |= BIT(24); // Сбрасываем флаг прерывания
    if ( GPIOA_PDIR & BIT(12) )
    {
      // Лог. 1.
      // PWM блокируется
      MC_set_events(PWM_OE_RISE);
    }
    else
    {
      // Лог. 0.
      // PWM разрешается
      MC_set_events(PWM_OE_FALL);
    }
  }
}

/*-------------------------------------------------------------------------------------------------------------
  Процедура обслуживания прерывания от порта B
  Используеться для слежения за состоянием сигнала VFO (уведомление о нештатной ситуации от драйвера двигателя)
-------------------------------------------------------------------------------------------------------------*/
static void PORTB_isr(pointer user_isr_ptr)
{
  if ( PORTB_PCR(3) & BIT(24) )
  {
    // Прерывание от сигнала PWM_OE
    PORTB_PCR(3) |= BIT(24); // Сбрасываем флаг прерывания
    if ( GPIOB_PDIR & BIT(3) )
    {
      // Лог. 1.
      // Уведомление снято
      MC_set_events(VFO_RISE);
    }
    else
    {
      // Лог. 0.
      // Уведомление активно
      MC_set_events(VFO_FALL);
    }
  }
}


/*-------------------------------------------------------------------------------------------------------------
  Установка прерывания для наблюдения за входом PWM_OE 
-------------------------------------------------------------------------------------------------------------*/
void Pin_PWM_OE_int_install(void)
{
  // Настроить фильтацию на порту A
  PORTA_PCR(12) |= LSHIFT(0xB, 16); // Тип прерывания на пине 12 - по фронту и по спаду
  PORTA_PCR(12) |= BIT(4);          // Включить фильтрацию на пине 12
  PORTA_DFER     = 0;               // Обнулим чтобы сконфигурировать тактирование фильтров
  PORTA_DFCR     = 1;               // Для тактирования фильтра на пине 12 используем частоту 1 КГц
  PORTA_DFWR     = 30;              // Не воспринимаем импульсы короче 30 мс
  PORTA_DFER    |= BIT(12);         // Включить фильтрацию на пине 12

  // Установить прерывание на порт
  _int_install_isr(INT_PORTA, PORTA_isr, 0);
  // Разрешить прерывание только после установки вектора! Иначе можем уйти в непрерывный вызов ISR по дефолтному вектору
  _bsp_int_init(INT_PORTA, 3, 0, TRUE);

}

/*-------------------------------------------------------------------------------------------------------------
  Установка прерывания для наблюдения за входом VFO 
-------------------------------------------------------------------------------------------------------------*/
void Pin_VFO_int_install(void)
{
  // Настроить фильтацию на порту B
  PORTB_PCR(3) |= LSHIFT(0xB, 16); // Тип прерывания на пине 3 - по фронту и по спаду
  PORTB_PCR(3) |= BIT(4);          // Включить фильтрацию на пине 3
  PORTB_DFER     = 0;               // Обнулим чтобы сконфигурировать тактирование фильтров
  PORTB_DFCR     = 0;               // Для тактирования фильтра на пине 3 используем частоту шины (60 МГц)
  PORTB_DFWR     = 12;              // Не воспринимаем импульсы короче 200 нс
  PORTB_DFER    |= BIT(3);          // Включить фильтрацию на пине 3

  // Установить прерывание на порт
  _int_install_isr(INT_PORTB, PORTB_isr, 0);
  // Разрешить прерывание только после установки вектора! Иначе можем уйти в непрерывный вызов ISR по дефолтному вектору
  _bsp_int_init(INT_PORTB, 3, 0, TRUE);

}
