#ifndef __MONITORVT100
  #define __MONITORVT100

  #define COLCOUNT 80
  #define ROWCOUNT 24

/* cursor motion */
  #define VT100_CURSOR_DN   "\033D"  //
  #define VT100_CURSOR_DN_L "\033E"
  #define VT100_CURSOR_UP   "\033M"
  #define VT100_CURSOR_HOME "\033[H"
  #define VT100_CURSOR_N_UP "\033[%dA"  /* printf argument: lines */
  #define VT100_CURSOR_N_RT "\033[%dC"  /* printf argument: cols  */
  #define VT100_CURSOR_N_LT "\033[%dD"  /* printf argument: cols  */
  #define VT100_CURSOR_N_DN "\033[%dB"  /* printf argument: lines */
  #define VT100_CURSOR_SET  "\033[%d;%dH" /* printf arguments: row, col */

/* erasing the screen */
  #define VT100_CLR_FM_CRSR "\033[J"
  #define VT100_CLR_TO_CRSR "\033[1J"
  #define VT100_CLR_SCREEN  "\033[2J"

/* erasing current line */
  #define VT100_CLL_FM_CRSR "\033[K"
  #define VT100_CLL_TO_CRSR "\033[1K"
  #define VT100_CLR_LINE    "\033[2K"

/* inserting and deleting */
  #define VT100_INS_CHARS   "\033[%d"   /* printf argument: cols */
  #define VT100_DEL_CHARS   "\033[%dP"  /* printf argument: cols */
  #define VT100_INS_LINES   "\033[%dL"  /* printf argument: cols */
  #define VT100_DEL_LINES   "\033[%dM"  /* printf argument: cols */

/* character attributes */
  #define VT100_NORMAL      "\033[m"
  #define VT100_ALL_OFF     "\033[0m"
  #define VT100_BOLD_ON     "\033[1m"
  #define VT100_BOLD_OFF    "\033[22m"
  #define VT100_UNDERL_ON   "\033[4m"
  #define VT100_UNDERL_OFF  "\033[24m"
  #define VT100_BLINK_ON    "\033[5m"
  #define VT100_BLINK_OFF   "\033[25m"
  #define VT100_REVERSE_ON  "\033[7m"
  #define VT100_REVERSE_OFF "\033[27m"
  #define VT100_INVIS_ON    "\033[8m"
  #define VT100_INVIS_OFF   "\033[28m"

/* screen attributes */
  #define VT100_ECHO_ON     "\033[12l"
  #define VT100_ECHO_OFF    "\033[12h"
  #define VT100_WRAP_ON     "\033[?7l"
  #define VT100_WRAP_OFF    "\033[?7h"
  #define VT100_CURSOR_ON   "\033[?25h"
  #define VT100_CURSOR_OFF  "\033[?25l"
  #define VT100_ENQ_SIZE    "\033[?92l" /* response: "\033[?%d,%dc" rows, cols */

  #define VT100_CLEAR_AND_HOME "\033[2J\033[H\033[m\033[?25l"


  #define DASH_LINE "--------------------------------------------------------------------------------"
  #define SCR_ITEMS_VERT_OFFS 8
  #define SCR_ITEMS_HOR_OFFS  1


typedef void (*T_menu_func)(INT8U  keycode);

typedef struct
{
  INT8U but;             // Кнопка нажатие которой вызывает данный пункт
  T_menu_func func;      // Функция вызываемая данным пунктом меню
  void *psubmenu;        // Аргумент. Для подменю указатель на запись подменю
} T_VT100_Menu_item;

typedef struct
{
  const INT8U *menu_header;           // Заголовок вверху экрана посередине
  const INT8U *menu_body;          // Содержание меню
  const T_VT100_Menu_item *menu_items;       // Массив структур с аттрибутами пунктов меню
  const INT16S menu_items_cnt;        // Количество пунктов меню

} T_VT100_Menu;

  #define MENU_MAX_DEPTH 20
typedef struct
{
  void           *ptr;
  //NU_TASK         tsk;
  //NU_DRIVER      *driver;
  T_VT100_Menu   *menu_trace[MENU_MAX_DEPTH];
  INT32U          menu_nesting;
  INT8U           curr_pos;                      // Текущая позиция в области редактирования
  INT8U           beg_pos;                       // Начальная позиция области редактирования
                                                 //RTC_DATE        date;
                                                 //RTC_TIME        tim;
  void            (*Monitor_func)(unsigned char);

  volatile INT8U  current_param_category;
  volatile INT8U  current_categories_count;      // Количество подуровней в текущем меню
  volatile INT8U  current_category_items_count;  // Общее количество пунктов в текущем меню
  volatile int    par_indx;                      // Индекс редактируемого параметра
  INT8U           param_str[512];
  INT8U           firstrow;                      // Позиция первой  строки области редактирования переменной
  INT8U           rowcount;                      // Количество строк в области редактирования
  INT8U           current_row;
  INT8U           current_col;
  INT8U           current_pos;

} T_VT100_cb;


void         VT100_task(uint_32 initial_data);

void         VT100_clr_screen(void);
void         VT100_send_str_to_pos(INT8U *str, INT8U row, INT8U col);
INT8U        Find_str_center(INT8U *str);
void         VT100_send_str_to_pos(INT8U *str, INT8U row, INT8U col);

void         VT100_set_cursor_pos(INT8U row, INT8U col);

void         Set_monitor_func(void (*func)(unsigned char));
void         Goto_main_menu(void);
void         Return_to_prev_menu(void);
void         Execute_menu_func(INT8U b);
void         Display_menu(void);

INT32S       Mon_input_line(char *buf, int buf_len, int row, char *instr);

INT32S       Mon_send_buf(INT8U *buf, INT32U len);
INT32S       Mon_wait_byte(INT8U *b, INT32U timeout);

#endif
