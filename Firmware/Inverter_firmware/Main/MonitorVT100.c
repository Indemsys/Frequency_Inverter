#include "app.h"

#define CNTLQ      0x11
#define CNTLS      0x13
#define DEL        0x7F
#define BACKSPACE  0x08
//#define CR         0x0D  конфликт имен с регистрами периферии
#define LF         0x0A
#define ESC        0x1B

#define COL        80   /* Maximum column size       */

#define EDSTLEN 20



T_VT100_cb  vt100_cb;

static void  Do_motor_test(INT8U keycode);
static void  Do_I2C_test(INT8U keycode);
static void  Do_LCD_test(INT8U keycode);
static void  Do_CAN_log_view(INT8U keycode);
//static void  Do_ADC_test(INT8U keycode);
static void  Do_ADC_read_smpls(INT8U keycode);
static void  Do_Meas_values_view(INT8U keycode);

extern const T_VT100_Menu MENU_MAIN;
extern const T_VT100_Menu MENU_PARAMETERS;
extern const T_VT100_Menu MENU_SPEC;

static INT32S       Lookup_menu_item(T_VT100_Menu_item **item, INT8U b);


int          special_access;
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      Пункты имеющие свое подменю распологаются на следующем уровне вложенности
      Их функция подменяет в главном цикле обработчик нажатий по умолчанию и должна 
      отдавать управление периодически в главный цикл 

      Пункты не имеющие функции просто переходят на следующий уровень подменю

      Пункты не имеющие подменю полностью захватывают управление и на следующий уровень не переходят 

      Пункты без подменю и функции означают возврат на предыдущий уровень
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//-------------------------------------------------------------------------------------
const T_VT100_Menu_item MENU_MAIN_ITEMS[] =
{
  { '1', 0,                         (void *)&MENU_PARAMETERS },
  { '2', Do_motor_test,              0 },
  { '3', Do_CAN_log_view,            0 },
//  { '3', Do_I2C_test,                0 },
//  { '4', Do_LCD_test,                0 },
//  { '5', 0,                         (void *)&MENU_SPEC },
//  { '6', Do_ADC_test,                0 },
//  { '7', Do_ADC_read_smpls,          0 },
  { '8', Do_Meas_values_view,        0 },
  { 'R', 0,                          0 },
  { 'M', 0,                         (void *)&MENU_MAIN }
};

const T_VT100_Menu MENU_MAIN =
{
  "Inverter 1.0 MONITOR Ver.130904",
  "\033[5C MAIN MENU \r\n"
  "\033[5C <1> - Adjustable parameters and settings\r\n"
  "\033[5C <2> - Motor test\r\n"
  "\033[5C <3> - CAN test\r\n"
//  "\033[5C <4> - LCD test\r\n"
//  "\033[5C <5> - Special menu\r\n"
//  "\033[5C <6> - ADC test\r\n"
//  "\033[5C <7> - Read ADC samples\r\n"
  "\033[5C <8> - Measured values view\r\n",
  MENU_MAIN_ITEMS,
  sizeof(MENU_MAIN_ITEMS) / sizeof(MENU_MAIN_ITEMS[0])
};

//-------------------------------------------------------------------------------------
const T_VT100_Menu MENU_PARAMETERS =
{
  "",
  "",
  0,
  0
};

const T_VT100_Menu_item MENU_SPEC_ITEMS[] =
{
  { '1', 0,                          0 },
  { 'R', 0,                          0 },
  { 'M', 0,                         (void *)&MENU_MAIN }
};

const T_VT100_Menu MENU_SPEC =
{
  "Inverter 1.0 MONITOR Ver.130904",
  "\033[5C SPECIAL MENU \r\n"
  "\033[5C <1> - Signals diagnostic\r\n"
  "\033[5C <R> - Display previous menu\r\n"
  "\033[5C <M> - Display main menu\r\n",
  MENU_SPEC_ITEMS,
  sizeof(MENU_SPEC_ITEMS) / sizeof(MENU_SPEC_ITEMS[0])
};



/*-------------------------------------------------------------------------------------------------------------
   Задача монитора
-------------------------------------------------------------------------------------------------------------*/
void VT100_task(uint_32 initial_data)
{
  INT8U    b;

  printf(VT100_CLEAR_AND_HOME);
  Goto_main_menu();

  do
  {
    if ( Mon_wait_byte(&b, 100) == MQX_OK )
    {
      if ( b != 0 )
      {
        if ( vt100_cb.Monitor_func ) vt100_cb.Monitor_func(b);  // Обработчик нажатий главного цикла
      }
    }


    if ( vt100_cb.menu_trace[vt100_cb.menu_nesting] == &MENU_MAIN )
    {
      // Вывод на дисплей счетчиков
      VT100_set_cursor_pos(20, 0);
      //VT100_set_cursor_pos(25, 0);
      //Mon_printf("Date/Time: %02d%02d.%02d.%02d %02d:%02d:%02d",vt100_cb.date.century,vt100_cb.date.year,vt100_cb.date.month,vt100_cb.date.day,vt100_cb.tim.hours,vt100_cb.tim.minutes,vt100_cb.tim.seconds);
    }


  }
  while (1);

}


/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void Set_monitor_func(void (*func)(unsigned char))
{
  vt100_cb.Monitor_func = func;
}



/*-------------------------------------------------------------------------------------------------------------
  Вывести на экран текущее меню
  1. Вывод заголовока меню
  2. Вывод содержания меню
-------------------------------------------------------------------------------------------------------------*/
void Display_menu(void)
{

  printf(VT100_CLEAR_AND_HOME);

  if ( vt100_cb.menu_trace[vt100_cb.menu_nesting] == 0 ) return;

  VT100_send_str_to_pos((INT8U *)vt100_cb.menu_trace[vt100_cb.menu_nesting]->menu_header, 1, Find_str_center((INT8U *)vt100_cb.menu_trace[vt100_cb.menu_nesting]->menu_header));
  VT100_send_str_to_pos(DASH_LINE, 2, 0);
  VT100_send_str_to_pos((INT8U *)vt100_cb.menu_trace[vt100_cb.menu_nesting]->menu_body, 3, 0);
  printf("\r\n");
  printf(DASH_LINE);

}
/*-------------------------------------------------------------------------------------------------------------
  Поиск в текущем меню пункта вызываемого передаваемым кодом
  Параметры:
    b - код команды вазывающей пункт меню
  Возвращает:
    Указатель на соответствующий пункт в текущем меню
-------------------------------------------------------------------------------------------------------------*/
INT32S Lookup_menu_item(T_VT100_Menu_item **item, INT8U b)
{
  INT16S i;

  if ( isalpha(b) != 0 ) b = toupper(b);
  for (i = 0; i < vt100_cb.menu_trace[vt100_cb.menu_nesting]->menu_items_cnt; i++)
  {
    *item = (T_VT100_Menu_item *)vt100_cb.menu_trace[vt100_cb.menu_nesting]->menu_items + i;
    if ( (*item)->but == b ) return (1);
  }
  return (0);
}


/*----------------------------------------------------------------------------
 *      Line Editor
 *---------------------------------------------------------------------------*/
static int Get_string(char *lp, int n)
{
  int cnt = 0;
  char c;

  do
  {
    Mon_wait_byte((unsigned char *)&c, 0xFFFF);
    switch (c)
    {
    case CNTLQ:                          /* ignore Control S/Q             */
    case CNTLS:
      break;

    case BACKSPACE:
    case DEL:
      if ( cnt == 0 )
      {
        break;
      }
      cnt--;                             /* decrement count                */
      lp--;                              /* and line pointer               */
      /* echo backspace                 */
      printf("\x008 \x008");
      break;
    case ESC:
      *lp = 0;                           /* ESC - stop editing line        */
      return (MQX_ERROR);
    default:
      printf("*");
      *lp = c;                           /* echo and store character       */
      lp++;                              /* increment line pointer         */
      cnt++;                             /* and count                      */
      break;
    }
  }
  while (cnt < n - 1  &&  c != 0x0d);      /* check limit and line feed      */
  *lp = 0;                                 /* mark end of string             */
  return (MQX_OK);
}

/*-------------------------------------------------------------------------------------------------------------
  Ввод кода для доступа к специальному меню
-------------------------------------------------------------------------------------------------------------*/
int Enter_special_code(void)
{
  char str[32];
  if ( special_access != 0 )
  {
    return MQX_OK;
  }
  printf(VT100_CLEAR_AND_HOME"Enter access code>");
  if ( Get_string(str, 31) == MQX_OK )
  {
    if ( strcmp(str, "4321\r") == 0 )
    {
      special_access = 1;
      return MQX_OK;
    }
  }

  return MQX_ERROR;
}
/*-------------------------------------------------------------------------------------------------------------
  Поиск пункта меню по коду вызова (в текущем меню)
  и выполнение соответствующей ему функции
  Параметры:
    b - код команды вазывающей пункт меню

-------------------------------------------------------------------------------------------------------------*/
void Execute_menu_func(INT8U b)
{
  T_VT100_Menu_item *menu_item;

  if ( Lookup_menu_item(&menu_item, b) != 0 )
  {
    // Нашли соответствующий пункт меню
    if ( menu_item->psubmenu != 0 )
    {
      // Если присутствует субменю, то вывести его

      if ( (T_VT100_Menu *)menu_item->psubmenu == &MENU_SPEC )
      {
        if ( Enter_special_code() != MQX_OK )
        {
          Display_menu();
          return;
        }
      }

      vt100_cb.menu_nesting++;
      vt100_cb.menu_trace[vt100_cb.menu_nesting] = (T_VT100_Menu *)menu_item->psubmenu;

      Display_menu();
      // Если есть функция у пункта меню, то передать ей обработчик нажатий в главном цикле и выполнить функцию.
      if ( menu_item->func != 0 )
      {
        vt100_cb.Monitor_func = (T_menu_func)(menu_item->func); // Установить обработчик нажатий главного цикла на функцию из пункта меню
        menu_item->func(0);                // Выполнить саму функцию меню
      }
    }
    else
    {
      if ( menu_item->func == 0 )
      {
        // Если нет ни субменю ни функции, значит это пункт возврата в предыдущее меню
        // Управление остается в главном цикле у обработчика по умолчанию
        Return_to_prev_menu();
        Display_menu();
      }
      else
      {
        // Если у пункта нет своего меню, то перейти очистить экран и перейти к выполению  функции выбранного пункта
        printf(VT100_CLEAR_AND_HOME);
        menu_item->func(0);
        // Управление возвращается в главный цикл обработчику по умолчанию
        Display_menu();
      }
    }

  }
}


/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void Goto_main_menu(void)
{

  vt100_cb.menu_nesting = 0;
  vt100_cb.menu_trace[vt100_cb.menu_nesting] = (T_VT100_Menu *)&MENU_MAIN;
  Display_menu();
  vt100_cb.Monitor_func = Execute_menu_func; // Назначение функции
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void Return_to_prev_menu(void)
{

  if ( vt100_cb.menu_nesting > 0 )
  {
    vt100_cb.menu_trace[vt100_cb.menu_nesting] = 0;
    vt100_cb.menu_nesting--;
  }
  vt100_cb.Monitor_func = Execute_menu_func; // Назначение функции
}



/*-------------------------------------------------------------------------------------------------------------
  Очистка экрана монитора
-------------------------------------------------------------------------------------------------------------*/
void VT100_clr_screen(void)
{
  printf(VT100_CLEAR_AND_HOME);
}

/*-------------------------------------------------------------------------------------------------------------
     Установка курсора в заданную позицию
-------------------------------------------------------------------------------------------------------------*/
void VT100_set_cursor_pos(INT8U row, INT8U col)
{
  printf("\033[%.2d;%.2dH", row, col);
}

/*-------------------------------------------------------------------------------------------------------------
     Вывод строки в заданную позицию
-------------------------------------------------------------------------------------------------------------*/
void VT100_send_str_to_pos(INT8U *str, INT8U row, INT8U col)
{
  printf("\033[%.2d;%.2dH", row, col);
  Mon_send_buf(str, strlen((char *)str));
}

/*-------------------------------------------------------------------------------------------------------------
    Находим позицию начала строки для расположения ее по центру экрана
-------------------------------------------------------------------------------------------------------------*/
INT8U Find_str_center(INT8U *str)
{
  INT16S l = 0;
  while (*(str + l) != 0) l++; // Находим длину строки
  return(COLCOUNT - l) / 2;
}



/*-------------------------------------------------------------------------------------------------------------
  Прием строки 
-------------------------------------------------------------------------------------------------------------*/
INT32S Mon_input_line(char *buf, int buf_len, int row, char *instr)
{

  int  i;
  INT8U b;
  int  res;
  INT8U  bs_seq[] = { 0x08, 0x020, 0x08, 0 };

  i = 0;
  VT100_set_cursor_pos(row, 0);
  printf(VT100_CLL_FM_CRSR);
  printf(">");

  if ( instr != 0 )
  {
    i = strlen(instr);
    if ( i < buf_len )
    {
      printf(instr);
    }
    else i = 0;
  }
  do
  {
    if ( Mon_wait_byte(&b, 1000) != MQX_OK )
    {
      res = MQX_ERROR;
      goto exit_;
    };

    if ( b == 0x08 )
    {
      if ( i > 0 )
      {
        i--;
        Mon_send_buf(bs_seq, sizeof(bs_seq));
      }
    }
    else if ( b != 0x0D && b != 0x0A && b != 0 )
    {
      Mon_send_buf(&b, 1);
      buf[i] = b;           /* String[i] value set to alpha */
      i++;
      if ( i >= buf_len )
      {
        res = MQX_ERROR;
        goto exit_;
      };
    }
  }
  while ((b != 0x0D) && (i < COL));

  res = MQX_OK;
  buf[i] = 0;                     /* End of string set to NUL */
exit_:

  VT100_set_cursor_pos(row, 0);
  printf(VT100_CLL_FM_CRSR);

  return (res);
}



/*-------------------------------------------------------------------------------------------------------------
  Вывод форматированной строки в коммуникационный канал порта
-------------------------------------------------------------------------------------------------------------*/
INT32S  Mon_send_buf(INT8U *buf, INT32U len)
{
  int i;
  for (i = 0; i < len; i++)
  {
    printf("%c", buf[i]);
  }
  return MQX_OK;
}

/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
INT32S Mon_wait_byte(INT8U *b, INT32U timeout)
{
  for (;;)
  {
    while (status() == 0)
    {
      if ( timeout == 0 )
      {
        return MQX_ERROR;
      }
      _time_delay_ticks(2);
      timeout--;
    }
    *b = getchar();
    return MQX_OK;
  }
}

static void FixStrerr(int len)
{
  static volatile int dummy;

  dummy = len;
}
/*-----------------------------------------------------------------------------------------------------
 
-----------------------------------------------------------------------------------------------------*/
static void  Do_motor_test(INT8U keycode)
{
  INT8U              b;
  T_MC_CBL           cbl;
  float              temp;
  unsigned long long cnt;
  char               str[64];
  T_meas_results     mres[MEAS_RES_ARR_SZ];

  printf("Motor algorithm test.\n\r");
  printf("Press 'R' to exit. '<'-down, '>'-up, 'F'-Fun on,'G'-Fun off, 'Q'-stop \n\r");

  do
  {
    //__ITM_EVENT32(1, 100);
    //ITM_EVENT32_WITH_PC(3, cnt);

    VT100_set_cursor_pos(3, 0);
    printf(DASH_LINE"\r\n");
    printf(VT100_CLR_LINE"%lld\r\n", cnt);

    MC_get_CBL(&cbl);
    printf(VT100_CLR_LINE"(0) Moving up accel.PWM scale factor  = %0.3f\r\n", cbl.up_accel_pwm_scale);
    printf(VT100_CLR_LINE"(1) Moving up PWM scale factor        = %0.3f\r\n", cbl.up_pwm_scale);
    printf(VT100_CLR_LINE"(2) Moving up decel.PWM scale factor  = %0.3f\r\n", cbl.up_decel_pwm_scale);
    printf(VT100_CLR_LINE"(3) Moving up rot.speed               = %d\r\n",    cbl.up_move_freq);
    printf(VT100_CLR_LINE"(4) Moving up acceler.time            = %d\r\n",    cbl.up_acceler_time);
    printf(VT100_CLR_LINE"(5) Moving up deceler.time            = %d\r\n",    cbl.up_deceler_time);

    printf(VT100_CLR_LINE"(6) Moving down accel.PWM scale factor= %0.3f\r\n", cbl.down_accel_pwm_scale);
    printf(VT100_CLR_LINE"(7) Moving down PWM scale factor      = %0.3f\r\n", cbl.down_pwm_scale);
    printf(VT100_CLR_LINE"(8) Moving down decel.PWM scale factor= %0.3f\r\n", cbl.down_decel_pwm_scale);
    printf(VT100_CLR_LINE"(9) Moving down rot.speed             = %d\r\n",    cbl.down_move_freq);
    printf(VT100_CLR_LINE"(A) Moving down acceler.time          = %d\r\n",    cbl.down_acceler_time);
    printf(VT100_CLR_LINE"(B) Moving down deceler.time          = %d\r\n",    cbl.down_deceler_time);
    printf(VT100_CLR_LINE"\r\n");
    printf(VT100_CLR_LINE"Motor cur.rot.speed = %d\r\n", cbl.mot_freq);
    printf(VT100_CLR_LINE"Rotation dir        = %d\r\n", cbl.direction);
    printf(VT100_CLR_LINE"skew_cnt = %06d, step = %016llX, ll_freq = %016llX\r\n", cbl.skew_cnt, cbl.ll_step, cbl.ll_mot_freq);
    printf(VT100_CLR_LINE"PWM scale= %0.3f\r\n", cbl.pwm_scale);

    if ( TempCtrl_read_IGBT_temperature(&temp) == MQX_OK )
    {
      snprintf(str, 63, "%0.3lf", temp);
      if ( strlen(str) > 6 ) FixStrerr(strlen(str));
      printf(VT100_CLR_LINE"IGBT temperature = %s\r\n", str);
    }
    else
    {
      printf(VT100_CLR_LINE"IGBT temperature reading error!\r\n");
    }

    if ( cbl.motor_drv_fail )
    {
      printf(VT100_CLR_LINE"Detected VFO = 0. Motor driver fail!\r\n");
    }
    else
    {
      printf(VT100_CLR_LINE"Motor driver Ok!\r\n");
    }
    Get_copy_meas_results(mres);
/* 
  ii_w         
  ii_v         
  ii_u         
  v_bus        
  smpl_temper  
  smpl_15v     
  smpl_5v       
  smpl_ii_u  
*/
    printf(VT100_CLR_LINE"Currents(w,v,u). Max  : %05.2f , %05.2f , %05.2f\r\n", mres[0].fmax, mres[1].fmax, mres[2].fmax);
    printf(VT100_CLR_LINE"Currents(w,v,u). Min  : %05.2f , %05.2f , %05.2f\r\n", mres[0].fmin, mres[1].fmin, mres[2].fmin);
    printf(VT100_CLR_LINE"Currents(w,v,u). Aver : %05.2f , %05.2f , %05.2f\r\n", mres[0].favr, mres[1].favr, mres[2].favr);
    printf(VT100_CLR_LINE"Currents(w,v,u). RMS  : %05.2f , %05.2f , %05.2f\r\n", mres[0].frms, mres[1].frms, mres[2].frms);
    printf("\r\n");
    printf(VT100_CLR_LINE"DC BUS. : max=%06.2f , min=%06.2f , avr=%06.2f, RMS=%06.2f\r\n", mres[3].fmax, mres[3].fmin, mres[3].favr, mres[3].frms);
    printf(VT100_CLR_LINE"Temper. : max=%06.2f , min=%06.2f , avr=%06.2f, RMS=%06.2f\r\n", mres[4].fmax, mres[4].fmin, mres[4].favr, mres[4].frms);



    if ( Mon_wait_byte(&b, 50) == MQX_OK )
    {
      switch (b)
      {
      case '0':
        sprintf(str, "%0.3f", cbl.up_accel_pwm_scale);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%f", &cbl.up_accel_pwm_scale) == 1 )
          {
            if ( (cbl.up_accel_pwm_scale <= 1) && (cbl.up_accel_pwm_scale >= 0) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;
      case '1':
        sprintf(str, "%0.3f", cbl.up_pwm_scale);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%f", &cbl.up_pwm_scale) == 1 )
          {
            if ( (cbl.up_pwm_scale <= 1) && (cbl.up_pwm_scale >= 0) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;
      case '2':
        sprintf(str, "%0.3f", cbl.up_decel_pwm_scale);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%f", &cbl.up_decel_pwm_scale) == 1 )
          {
            if ( (cbl.up_decel_pwm_scale <= 1) && (cbl.up_decel_pwm_scale >= 0) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;

      case '3':
        sprintf(str, "%d", cbl.up_move_freq);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%d", &cbl.up_move_freq) == 1 )
          {
            if ( (cbl.up_move_freq < 101) && (cbl.up_move_freq >= 1) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;
      case '4':
        sprintf(str, "%d", cbl.up_acceler_time);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%d", &cbl.up_acceler_time) == 1 )
          {
            if ( (cbl.up_acceler_time < 10000) && (cbl.up_acceler_time >= 0) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;
      case '5':
        sprintf(str, "%d", cbl.up_deceler_time);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%d", &cbl.up_deceler_time) == 1 )
          {
            if ( (cbl.up_deceler_time < 10000) && (cbl.up_deceler_time >= 0) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;

      case '6':
        sprintf(str, "%0.3f", cbl.down_accel_pwm_scale);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%f", &cbl.down_accel_pwm_scale) == 1 )
          {
            if ( (cbl.down_accel_pwm_scale <= 1) && (cbl.down_accel_pwm_scale >= 0) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;

      case '7':
        sprintf(str, "%0.3f", cbl.down_pwm_scale);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%f", &cbl.down_pwm_scale) == 1 )
          {
            if ( (cbl.down_pwm_scale <= 1) && (cbl.down_pwm_scale >= 0) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;

      case '8':
        sprintf(str, "%0.3f", cbl.down_decel_pwm_scale);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%f", &cbl.down_decel_pwm_scale) == 1 )
          {
            if ( (cbl.down_decel_pwm_scale <= 1) && (cbl.down_decel_pwm_scale >= 0) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;


      case '9':
        sprintf(str, "%d", cbl.down_move_freq);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%d", &cbl.down_move_freq) == 1 )
          {
            if ( (cbl.down_move_freq < 101) && (cbl.down_move_freq >= 1) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;
      case 'A':
        sprintf(str, "%d", cbl.down_acceler_time);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%d", &cbl.down_acceler_time) == 1 )
          {
            if ( (cbl.down_acceler_time < 10000) && (cbl.down_acceler_time >= 0) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;
      case 'B':
        sprintf(str, "%d", cbl.down_deceler_time);
        if ( Mon_input_line(str, 30, 24, str) == MQX_OK )
        {
          if ( sscanf(str, "%d", &cbl.down_deceler_time) == 1 )
          {
            if ( (cbl.down_deceler_time < 10000) && (cbl.down_deceler_time >= 0) )
            {
              MC_set_CBL(&cbl);
            }
          }
        }
        break;



      case '<':
        MC_set_events(MOTOR_START_DOWN);
        break;
      case '>':
        MC_set_events(MOTOR_START_UP);
        break;
      case 'q':
      case 'Q':
        MC_set_events(MOTOR_STOP);
        break;
      case 'f':
      case 'F':
          Fun_control(1);
        break;
      case 'g':
      case 'G':
          Fun_control(0);
        break;
      case 'R':
      case 'r':
        return;
      }
    }
    cnt++;
  }
  while (1);
}

/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
#define I2C_DEVICE_POLLED    "i2c0:"
#define I2C_DEVICE_INTERRUPT "ii2c0:"
#define I2C_MAX31725_ADDRESS  (0x90>>1)
#define BUFF_SZ               32

static void  Do_I2C_test(INT8U keycode)
{
  INT8U          b;
  MQX_FILE_PTR   fd;
  _mqx_int                     param, result;
  I2C_STATISTICS_STRUCT        stats;
  _mqx_int   n = BUFF_SZ;
  uchar  buffer[BUFF_SZ];

  printf("I2C test.\n\r");
  printf("Press 'R' to exit. \n\r");

  fd = fopen(I2C_DEVICE_POLLED, NULL);
  if ( fd == NULL )
  {
    printf("ERROR opening the I2C driver!\n\r");
  }
  else
  {
    /* Test ioctl commands */
    param = 100000;
    printf("Set current baud rate to %d ... ", param);
    if ( I2C_OK == ioctl(fd, IO_IOCTL_I2C_SET_BAUD, &param) )
    {
      printf("OK\n\r");
    }
    else
    {
      printf("ERROR\n\r");
    }

    printf("Set master mode ... ");
    if ( I2C_OK == ioctl(fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL) )
    {
      printf("OK\n\r");
    }
    else
    {
      printf("ERROR\n\r");
    }

    printf("Get statistics ... ");
    if ( I2C_OK == ioctl(fd, IO_IOCTL_I2C_GET_STATISTICS, (pointer) & stats) )
    {
      printf("OK\n\r  Interrupts:  %d\n\r", stats.INTERRUPTS);
      printf("  Rx packets:  %d\n\r", stats.RX_PACKETS);
      printf("  Tx packets:  %d\n\r", stats.TX_PACKETS);
      printf("  Tx lost arb: %d\n\r", stats.TX_LOST_ARBITRATIONS);
      printf("  Tx as slave: %d\n\r", stats.TX_ADDRESSED_AS_SLAVE);
      printf("  Tx naks:     %d\n\r", stats.TX_NAKS);
    }
    else
    {
      printf("ERROR\n\r");
    }

  }

  printf("Get current state ... ");
  if ( I2C_OK == ioctl(fd, IO_IOCTL_I2C_GET_STATE, &param) )
  {
    printf("0x%02x\n\r", param);
  }
  else
  {
    printf("ERROR\n\r");
  }

  param = I2C_MAX31725_ADDRESS;
  printf("Set destination address to 0x%02x ... ", param);
  if ( I2C_OK == ioctl(fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param) )
  {
    printf("OK\n\r");
  }
  else
  {
    printf("ERROR\n\r");
  }

  /* Initiate start and send I2C bus address */
  printf("  I2C start, send address and get ack ... ");
  fwrite(buffer, 1, 0, fd);

  /* Check ack (device exists) */
  if ( I2C_OK == ioctl(fd, IO_IOCTL_FLUSH_OUTPUT, &param) )
  {
    printf("OK ... ack == %d\n\r", param);
    if ( param )
    {
      /* Stop I2C transfer */
      printf("  Stop transfer ... ");
      if ( I2C_OK == ioctl(fd, IO_IOCTL_I2C_STOP, NULL) )
      {
        printf("OK\n\r");
      }
      else
      {
        printf("ERROR\n\r");
      }
    }
  }
  else
  {
    printf("ERROR\n\r");
  }

  buffer[0] = 0;
  printf("  Write to address 0x%02x ... ", buffer[0]);
  result = fwrite(buffer, 1, 1, fd);
  if ( 1 == result )
  {
    printf("OK\n\r");
  }
  else
  {
    printf("ERROR\n\r");
  }


  /* Restart I2C transfer for reading */
  printf("  Initiate repeated start ... ");
  if ( I2C_OK == ioctl(fd, IO_IOCTL_I2C_REPEATED_START, NULL) )
  {
    printf("OK\n\r");
  }
  else
  {
    printf("ERROR\n\r");
  }

  /* Set read request */
  param = n;
  printf("  Set number of bytes requested to %d ... ", param);
  if ( I2C_OK == ioctl(fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param) )
  {
    printf("OK\n\r");
  }
  else
  {
    printf("ERROR\n\r");
  }


  /* Read all data */
  printf("  Read %d bytes ... ", n);
  result = fread(buffer, 1, n, fd);
  if ( result == n )
  {
    printf("OK\n\r");
  }
  else
  {
    printf("ERROR\n\r");
  }

  /* Stop I2C transfer - initiate EEPROM write cycle */
  printf("  Stop transfer ... ");
  if ( I2C_OK == ioctl(fd, IO_IOCTL_I2C_STOP, NULL) )
  {
    printf("OK\n\r");
  }
  else
  {
    printf("ERROR\n\r");
  }

  /* Close the driver */
  result = fclose(fd);
  if ( result )
  {
    printf("ERROR during close, returned: %d\n\r", result);
  }

  {
    int i, j;

    printf("\r\nDump:\r\n");

    for (j = 0; j < 2; j++)
    {
      for (i = 0; i < 16; i++)
      {
        printf("%02X ", buffer[i + j * 16]);
      }
      printf("\r\n");
    }

  }

  do
  {
    if ( Mon_wait_byte(&b, 50) == MQX_OK )
    {
      switch (b)
      {
      case 'R':
      case 'r':
        return;
      }
    }
  }
  while (1);

}

/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
static void  Do_LCD_test(INT8U keycode)
{
  INT8U          b;

  printf("LCD test.\n\r");
  printf("Press 'R' to exit. \n\r");

  do
  {
    int r;
    LCD_init();

    _time_delay_ticks(1);
    r = rand();
    LCD_wr_data(r);

    if ( Mon_wait_byte(&b, 100) == MQX_OK )
    {
      switch (b)
      {
      case 'R':
      case 'r':
        return;
      }
    }
  }
  while (1);


}

/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/

static void  Do_CAN_log_view(INT8U keycode)
{
  INT8U          b;
  T_can_rx       rx;
  INT32U         i;

  printf("CAN log.\n\r");
  printf("Press 'R' to exit. \n\r");

  do
  {
    // Ожидаем сообщения о помещении в лог нового сообщения
    if ( CAN_wait_log_rec(10) == MQX_OK )
    {
      // Отобразить записи из лога
      while (CAN_pop_log_rec(&rx) == MQX_OK)
      {

        printf("%02X %08X %01X %01X %01X - ",  rx.code, rx.canid, rx.ext, rx.rtr, rx.len);
        for (i = 0; i < rx.len; i++)
        {
          printf("%02X ", rx.data[i]);
        }
        for (; i < 8; i++)
        {
          printf("   ");
        }
        printf("\r\n");
      }
    }
    if ( Mon_wait_byte(&b, 0) == MQX_OK )
    {
      switch (b)
      {
      case 'R':
      case 'r':
        return;
      }
    }
  }
  while (1);


}


/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
/*
static void  Do_ADC_test(INT8U keycode)
{
  INT8U           b;
  MQX_TICK_STRUCT start_time;
  MQX_TICK_STRUCT end_time;
  boolean         ovrfl;
  int_32          dt;
  signed short    res;
  int             i;

  PDB_deactivate_ADC_triggers();
  ADC_activate_soft_trig(ADC0_BASE_PTR);
  ADC_activate_soft_trig(ADC1_BASE_PTR);
  ADC_activate_soft_trig(ADC2_BASE_PTR);
  ADC_activate_soft_trig(ADC3_BASE_PTR);

  printf("ADC test.\n\r");
  printf("Press 'R' to exit. \n\r");


  printf("\r\nADC0 registers:\r\n");
  ADC_print_regs(ADC0_BASE_PTR);
  printf("\r\nADC1 registers:\r\n");
  ADC_print_regs(ADC1_BASE_PTR);
  printf("\r\nADC2 registers:\r\n");
  ADC_print_regs(ADC2_BASE_PTR);
  printf("\r\nADC3 registers:\r\n");
  ADC_print_regs(ADC3_BASE_PTR);

  printf("\r\n---==== CALIBRATING ====---\r\n");
  _time_get_ticks(&start_time);
  res = ADC_calibrating(ADC0_BASE_PTR);
  _time_get_ticks(&end_time);
  dt = _time_diff_microseconds(&end_time, &start_time, &ovrfl);
  printf("t=%d us, res=%d, ", dt, res);

  _time_get_ticks(&start_time);
  res = ADC_calibrating(ADC1_BASE_PTR);
  _time_get_ticks(&end_time);
  dt = _time_diff_microseconds(&end_time, &start_time, &ovrfl);
  printf("t=%d us, res=%d, ", dt, res);

  _time_get_ticks(&start_time);
  res = ADC_calibrating(ADC2_BASE_PTR);
  _time_get_ticks(&end_time);
  dt = _time_diff_microseconds(&end_time, &start_time, &ovrfl);
  printf("t=%d us, res=%d, ", dt, res);

  _time_get_ticks(&start_time);
  res = ADC_calibrating(ADC3_BASE_PTR);
  _time_get_ticks(&end_time);
  dt = _time_diff_microseconds(&end_time, &start_time, &ovrfl);
  printf("t=%d us, res=%d, ", dt, res);
  printf("\r\n");

  printf("\r\nADC0 registers:\r\n");
  ADC_print_regs(ADC0_BASE_PTR);
  printf("\r\nADC1 registers:\r\n");
  ADC_print_regs(ADC1_BASE_PTR);
  printf("\r\nADC2 registers:\r\n");
  ADC_print_regs(ADC2_BASE_PTR);
  printf("\r\nADC3 registers:\r\n");
  ADC_print_regs(ADC3_BASE_PTR);


  ADC_init(ADC0_BASE_PTR);
  ADC_init(ADC1_BASE_PTR);
  ADC_init(ADC2_BASE_PTR);
  ADC_init(ADC3_BASE_PTR);


  VT100_clr_screen();
  printf("Test ADC sampling (Press 'R' to exit):");
  do
  {
    VT100_set_cursor_pos(2, 0);

    for (i = 0; i < TEST_CH_ARR_SZ; i++)
    {
      int n;
      unsigned short r;
      _time_get_ticks(&start_time);
      for (n = 0; n < 100; n++)
      {
        res = ADC_get_sample(adca[i].padc, adca[i].ch, adca[i].diff, adca[i].aver);
      }
      _time_get_ticks(&end_time);
      dt = _time_diff_microseconds(&end_time, &start_time, &ovrfl) / 100;
      r = res;
      printf(VT100_CLR_LINE"%s(%02d) : R=%05d (%04X), Time=%03d us, val=%0.2f\r\n", adca[i].name, adca[i].ch, r, r,  dt, adca[i].converter(res));
    }


    if ( Mon_wait_byte(&b, 100) == MQX_OK )
    {
      switch (b)
      {
      case 'R':
      case 'r':

        ADC_activate_hrdw_trig(ADC0_BASE_PTR);
        ADC_activate_hrdw_trig(ADC1_BASE_PTR);
        ADC_activate_hrdw_trig(ADC2_BASE_PTR);
        ADC_activate_hrdw_trig(ADC3_BASE_PTR);
        PDB_activate_ADC_triggers();

        return;
      }
    }
  }
  while (1);
}
*/


/*-----------------------------------------------------------------------------------------------------
   Здесь пытаюсь определить как зависит шум измерения АЦП от момента когда сработает триггер на следующую выборку
    
   Похоже шум зависит случайно от калибровки
-----------------------------------------------------------------------------------------------------*/
/*
static void  Do_ADC_test(INT8U keycode)
{
  INT8U           b;
  int             i, k, n;
  signed short   *arr[8];
  float           avr[8];
  float           disp[8];
  T_ADC_state    *adc_state;

  printf("ADC test.\n\r");
  printf("Press 'R' to exit. \n\r\n\r");

  adc_state = ADC_get_state();

  for (i = 0; i < 100; i++)
  {
    int delay;
    delay = 100 + i;

    ADCs_prepare();
    PDB_set_delays(delay, delay);

    ADC_reset_smpl_arr_pos();
    do
    {
      _time_delay_ticks(1);
    }
    while (ADC_is_smpl_arr_ready() == 0);

    ADC_get_smpl_arr(arr);


    for (n = 0; n < 8; n++)
    {
      avr[n] = 0;
      for (k = 0; k < TST_SMPLS_ARR_SZ; k++)
      {
        // Находим среднее
        avr[n] += arr[n][k];
      }
      avr[n] = avr[n] / TST_SMPLS_ARR_SZ;

      disp[n] = 0;
      for (k = 0; k < TST_SMPLS_ARR_SZ; k++)
      {
        // Находим отклонение
        disp[n] += (arr[n][k] - avr[n]) * (arr[n][k] - avr[n]);
      }
      disp[n] = sqrt(disp[n] / TST_SMPLS_ARR_SZ);
    }
//    printf("%03d, %05.0f %05.0f %05.0f %05.0f %05.0f %05.0f %05.0f %05.0f\r\n",delay,avr[0],avr[1],avr[2],avr[3],avr[4],avr[5],avr[6],avr[7]);
//    printf("      %05.0f %05.0f %05.0f %05.0f %05.0f %05.0f %05.0f %05.0f\r\n",     disp[0],disp[1],disp[2],disp[3],disp[4],disp[5],disp[6],disp[7]);
    printf("%03d, %05.0f %05.0f %05.0f %05.0f  -  %06.2f %06.2f %06.2f %06.2f :%d%d%d%d\r\n", delay, avr[0], avr[2], avr[4], avr[1], disp[0], disp[2], disp[4], disp[1], adc_state->adc0_cal_res, adc_state->adc1_cal_res, adc_state->adc2_cal_res, adc_state->adc3_cal_res);
  }


  do
  {
    if ( Mon_wait_byte(&b, 100) == MQX_OK )
    {
      switch (b)
      {
      case 'R':
      case 'r':
        return;
      }
    }
  }
  while (1);
}
*/
static void Send_short(unsigned short val)
{
  putchar(val & 0xFF);
  putchar((val >> 8) & 0xFF);
}
/*-----------------------------------------------------------------------------------------------------
  Чтение из массива отсчетов ADC
-----------------------------------------------------------------------------------------------------*/
/*
static void  Do_ADC_read_smpls(INT8U keycode)
{
  INT8U          b;
  unsigned int   i;
  signed short   arr[16];


  // Ожидаем стартового символа кроме 'R'
  do
  {
    if ( Mon_wait_byte(&b, 100) == MQX_OK )
    {
      switch (b)
      {
      case 'R':
      case 'r':
        return;

      default:
        {
          _mqx_uint events;

          Lock_meas_mutex();
          MC_get_events(&events, 1000, SMPL_ARR1_FULL);
          if ( b == '9' )
          {
            for (i = 0; i < TST_SMPLS_ARR_SZ; i++)
            {
              ADC_get_arr_smp(i, arr);
              Send_short(i);
              Send_short(arr[0]);
              Send_short(arr[1]);
              Send_short(arr[2]);
              Send_short(arr[3]);
              Send_short(arr[4]);
              Send_short(arr[5]);
              Send_short(arr[6]);
              Send_short(arr[7]);
              _time_delay_ticks(3);

            }
          }
          else
          {
            for (i = 0; i < TST_SMPLS_ARR_SZ; i++)
            {
              ADC_get_arr_smp(i, arr);
              printf("%0.7f,%05d,%05d,%05d,%05d,%05d,%05d,%05d\r\n", (float)i / (float)PWM_FREQ, arr[0], arr[1], arr[2], arr[3], arr[8], arr[9], arr[10]);
            }
          }
          ADC_reset_smpl_arr_pos();
          Unlock_meas_mutex();
        }
        break;
      }
    }
  }
  while (1);
}
*/
/*-----------------------------------------------------------------------------------------------------
 
-----------------------------------------------------------------------------------------------------*/
static void  Do_Meas_values_view(INT8U keycode)
{
  INT8U          b;
  T_meas_results     mres[MEAS_RES_ARR_SZ]; 


  // Ожидаем стартового символа кроме 'R'
  do
  {
    if ( Mon_wait_byte(&b, 100) == MQX_OK )
    {
      switch (b)
      {
      case 'R':
      case 'r':
        return;

      case '0':
        {
          _mqx_uint events;
          MC_get_events(&events, 2, MEAS_RES_READY);
          for (;;)
          {
            if (MC_get_events(&events, 2, MEAS_RES_READY) == MQX_OK )
            {
              float aver = Get_aver_curr();
              Get_copy_meas_results(mres);
              printf("%0.1f, %0.1f\r\n", (mres[0].frms + mres[1].frms + mres[2].frms)/3, aver);
            }
            if (Mon_wait_byte(&b, 0) == MQX_OK) 
            {
              if ( (b=='R') || (b=='r') )
              {
                return;
              }
            }
          }
        }
        break;
      }
    }
  }
  while (1);


}
