#ifndef __PINS_CONTROL
  #define __PINS_CONTROL

#define LED1 1
#define LED2 2
#define LED3 3


void Led_control(int led_num, int state);
void Fun_control(int state);
int  Fun_state(void);
void IGBT_reg_control(int state);
void NTC_bypass_control(int state);
void Pin_PWM_OE_int_install(void);
void Pin_VFO_int_install(void);
int  Pin_PWM_OE_state(void);

void LCD_RST(int state);
void LCD_SCL(int state);
void LCD_SI(int state);
void LCD_RS(int state);
void LCD_SCB(int state);
#endif
