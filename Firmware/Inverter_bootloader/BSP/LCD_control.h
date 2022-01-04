#ifndef __LCD_CONTROL
  #define __LCD_CONTROL

void LCD_init(void);
void LCD_wr_data(int data);
void LCD_wr_cmd(int cmd);
void LCD_print_char(char ch, int x, int y);
void LCD_set_pos(int x, int y);
void LCD_print_str(char *str, int x, int y);
void LCD_clear_row(int row);
void LCD_clear_all(void);

#endif
