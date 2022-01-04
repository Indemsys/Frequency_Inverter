#ifndef __INIT_PINS
  #define __INIT_PINS

int Init_pins(void);
void Led_control(int led_num, int state);
int  Led_state(int led_num);
void LCD_RST(int state);
void LCD_SCL(int state);
void LCD_SI(int state);
void LCD_RS(int state);
void LCD_SCB(int state);

#endif