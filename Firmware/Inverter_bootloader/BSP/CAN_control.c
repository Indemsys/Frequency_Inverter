#include "app_cfg.h"




const T_can_rx_config rx_mboxes[RX_MBOX_CNT] =
{
  { CAN_RX_MB1, ALL_ENTER_TO_WRK_MOD,  0xFFFFFFFF },
  { CAN_RX_MB2, ALL_PROLONG_BOOT_MODE, 0xFFFFFFFF },
  { CAN_RX_MB3, ALL_ENTER_TO_BOOT_MOD, 0xFFFFFFFF },
  { CAN_RX_MB4, INVERT_UPGRADE_TX_ID,  0xFFFFFFFF },
  { CAN_RX_MB5, INVERT_UPGRADE_TX_ID,  0xFFFFFFFF },
  { CAN_RX_MB6, INVERT_REQ,            0xFFFFFFFF },
  { CAN_RX_MB7, INVERT_REQ,            0xFFFFFFFF },
  { CAN_RX_MB8, INVERT_REQ,            0xFFFFFFFF },
};

volatile INT32U       g_can_flags;
INT32U                rx_events_mask = 0;
extern unsigned int   g_prolong_boot;
/*-------------------------------------------------------------------------------------------------------------
  Обслуживание прерываний CAN шины при отправке и получении данных
-------------------------------------------------------------------------------------------------------------*/
void CAN_isr(CAN_MemMapPtr ptr)
{
  volatile CAN_MemMapPtr   CAN;
  INT32U                   reg;

  CAN = ptr;

  reg = CAN->IFLAG1; // Получить флаги прерываний (всего в работе 16-ть младших флагов).
                     // Если не разрешен FIFO то каждый флаг отмечает прерывание от соответствующего буфера.
                     // Снимаються флаги записью 1
                     // Каждый флаг может отмечать, как прерывание по приему так и по передаче.
  if ( reg )
  {
    g_can_flags   = reg;
    CAN->IFLAG1 = reg;             // Сбрасываем флаги, но только те о которых мы сообщили
  }
}
/*-------------------------------------------------------------------------------------------------------------
  Обслуживание прерываний CAN шины в результате ошибок м других событий
-------------------------------------------------------------------------------------------------------------*/
void CAN_err_isr(CAN_MemMapPtr ptr)
{
  volatile CAN_MemMapPtr   CAN;
  INT32U                   reg;

  CAN = (CAN_MemMapPtr)ptr;

  // Получить флаги ошибок и сбросить их
  reg = CAN->ESR1 & (
                     BIT(17)   // TWRNINT. 1 The Tx error counter transitioned from less than 96 to greater than or equal to 96.
                     + BIT(16) // RWRNINT. 1 The Rx error counter transitioned from less than 96 to greater than or equal to 96.
                     + BIT(2)  // BOFFINT. ‘Bus Off’ Interrupt
                     + BIT(1)  // ERRINT . Error Interrupt
                     + BIT(0)  // WAKINT . Wake-Up Interrupt
                     );
  if ( reg  )
  {
    CAN->ESR1 = reg; // Сбрасываем флаги
  }
}


/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN0_MESS_IRQHandler(void)
{
  CAN_isr(CAN0_BASE_PTR);
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN0_BUS_OFF_IRQHandler(void)
{
  CAN_err_isr(CAN0_BASE_PTR);
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN0_ERR_IRQHandler(void)
{
  CAN_err_isr(CAN0_BASE_PTR);
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN0_TW_IRQHandler(void)
{
  CAN_err_isr(CAN0_BASE_PTR);
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN0_RW_IRQHandler(void)
{
  CAN_err_isr(CAN0_BASE_PTR);
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN0_WAKE_UP_IRQHandler(void)
{
  CAN_err_isr(CAN0_BASE_PTR);
}

/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN1_MESS_IRQHandler(void)
{
  CAN_isr(CAN1_BASE_PTR);
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN1_BUS_OFF_IRQHandler(void)
{
  CAN_err_isr(CAN1_BASE_PTR);
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN1_ERR_IRQHandler(void)
{
  CAN_err_isr(CAN1_BASE_PTR);
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN1_TW_IRQHandler(void)
{
  CAN_err_isr(CAN1_BASE_PTR);
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN1_RW_IRQHandler(void)
{
  CAN_err_isr(CAN1_BASE_PTR);
}
/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
void    CAN1_WAKE_UP_IRQHandler(void)
{
  CAN_err_isr(CAN1_BASE_PTR);
}


/*-------------------------------------------------------------------------------------------------------------
 
-------------------------------------------------------------------------------------------------------------*/
int Solve_can_timings(INT32U clk, INT32U bitrate, T_can_ctrl *ctrl)
{
  INT32U TQ_x_Prescaler = clk / bitrate;
  INT32U TQ;
  INT32U lowest_diff;
  INT32U diff;
  INT32U best_TQ;
  INT32U actual_freq;


  ctrl->pseg1    = 0;
  ctrl->pseg2    = 0;
  ctrl->propseg  = 0;
  ctrl->rjw      = 0;
  ctrl->presdiv  = 0;


  // If BSP_CANPE_CLOCK is defined, then we will calculate the CAN bit timing parameters
  // using the method outlined in AN1798, section 4.1. A maximum time for PROP_SEG will be used,
  // the remaining TQ will be split equally between PSEG1 and PSEG2, provided PSEG2 >=2. RJW is
  // set to the minimum of 4 or PSEG1.


  if ( TQ_x_Prescaler < (CAN_MIN_TQ - 1) )
  {
    // We will be off by more than 12.5%
    return ERROR;
  }

  // First, find the best TQ and pre-scaler to use for the desired frequency. If any exact matches
  // is found, we use the match that gives us the lowest pre-scaler and highest TQ, otherwise we pick
  // the TQ and prescaler that generates the closest frequency to the desired frequency.

  lowest_diff = bitrate;
  best_TQ = 0;
  for (TQ = CAN_MAX_TQ; TQ >= CAN_MIN_TQ; TQ--)
  {
    ctrl->presdiv = TQ_x_Prescaler / TQ;
    if ( ctrl->presdiv <= 256 )
    {
      if ( TQ_x_Prescaler == TQ * ctrl->presdiv )
      {
        best_TQ = TQ;
        break;
      }
      actual_freq = (clk / ctrl->presdiv) / TQ;

      if ( actual_freq > bitrate )
      {
        diff = actual_freq - bitrate;
      }
      else
      {
        diff = bitrate - actual_freq;
      }

      if ( diff < lowest_diff )
      {
        lowest_diff = diff;
        best_TQ = TQ;
      }
    }
  }
  if ( (best_TQ >= CAN_MIN_TQ) && (ctrl->presdiv <= 256) )
  {
    ctrl->pseg2 = (best_TQ - CAN_MAX_PROPSEG) / 2;
    if ( ctrl->pseg2 < CAN_MIN_PSEG2 ) ctrl->pseg2 = CAN_MIN_PSEG2;

    if ( best_TQ == CAN_MIN_TQ )
    {
      ctrl->pseg1 = 1;
    }
    else
    {
      ctrl->pseg1 = ctrl->pseg2;
    }

    ctrl->propseg = best_TQ - ctrl->pseg1 - ctrl->pseg2 - 1;

    if ( ctrl->pseg1 < CAN_MAX_RJW  )
    {
      ctrl->rjw = ctrl->pseg1;
    }
    else
    {
      ctrl->rjw = CAN_MAX_RJW;
    }

    ctrl->propseg -= 1;
    ctrl->rjw     -= 1;
    ctrl->pseg1   -= 1;
    ctrl->pseg2   -= 1;
    ctrl->presdiv -= 1;
  }
  else
  {
    return ERROR;
  }

  return SUCCESS;

}

/*-------------------------------------------------------------------------------------------------------------
  Обслуживание прерываний CAN шины
 
  ptr - указатель на область регистров контроллера CAN:  CAN0_BASE_PTR или CAN1_BASE_PTR  
-------------------------------------------------------------------------------------------------------------*/
int CAN_init(CAN_MemMapPtr ptr, INT32U bitrate)
{
  volatile CAN_MemMapPtr   CAN = ptr;
  SIM_MemMapPtr            SIM = SIM_BASE_PTR;
  INT32U                   clk = PERIPH_CLOCK;
  T_can_ctrl               ctrl;
  INT32U                   i;



  if ( ptr == CAN0_BASE_PTR )
  {
    SIM->SCGC6 |= BIT(4);  // Разрешаем тактирование CAN0
    CAN->CTRL1 |= BIT(13); // Тактирование от источника тактирования периферии

    // Инициализация всех прерываний
    Set_VIC_chanel(INT_CAN0_ORed_Message_buffer, 0, 0);
    Set_VIC_chanel(INT_CAN0_Bus_Off, 0, 0);
    Set_VIC_chanel(INT_CAN0_Error, 0, 0);
    Set_VIC_chanel(INT_CAN0_Tx_Warning, 0, 0);
    Set_VIC_chanel(INT_CAN0_Rx_Warning, 0, 0);
    Set_VIC_chanel(INT_CAN0_Wake_Up, 0, 0);


  }
  else if ( ptr == CAN1_BASE_PTR )
  {
    SIM->SCGC3 |= BIT(4);  // Разрешаем тактирование CAN1
    CAN->CTRL1 |= BIT(13); // Тактирование от источника тактирования периферии

    // Инициализация всех прерываний
    Set_VIC_chanel(INT_CAN1_ORed_Message_buffer, 0, 0);
    Set_VIC_chanel(INT_CAN1_Bus_Off, 0, 0);
    Set_VIC_chanel(INT_CAN1_Error, 0, 0);
    Set_VIC_chanel(INT_CAN1_Tx_Warning, 0, 0);
    Set_VIC_chanel(INT_CAN1_Rx_Warning, 0, 0);
    Set_VIC_chanel(INT_CAN1_Wake_Up, 0, 0);
  }
  else    return ERROR;

  // Выполняем программный сброс и переходим во Freeze mode
  if ( CAN->MCR & BIT(20) ) // Проверим выключен ли модуль, и сбросим этот режим если надо
  {
    CAN->MCR &= (~BIT(31));       // Включаем модуль
    while (CAN->MCR & BIT(20))    // Ждем пока флаг не сбросится
    {}
  }

  CAN->MCR = BIT(25);             // Выполняем SOFTRESET
  while (CAN->MCR & BIT(25))      // Ожидаем пока SOFTRESET закончится
  {}

  CAN->MCR |= 0
              + BIT(30) // FRZ    . Freeze Enable. 1 Enabled to enter Freeze Mode
              + BIT(28) // HALT   . Halt FlexCAN. 1 Enters Freeze Mode if the FRZ bit is asserted.
  ;
  while ((CAN->MCR & BIT(24)) == 0)   // Ожидаем установки FRZACK
  {}

  Solve_can_timings(clk, bitrate, &ctrl);

  CAN->CTRL1 = 0
               + LSHIFT(ctrl.presdiv, 24) // PRESDIV. Prescaler Division Factor
               + LSHIFT(ctrl.rjw,     22) // RJW    . Resync Jump Width
               + LSHIFT(ctrl.pseg1,   19) // PSEG1  . Phase Segment 1
               + LSHIFT(ctrl.pseg2,   16) // PSEG2  . Phase Segment 2
               + LSHIFT(1,            15) // BOFFMSK. Bus Off Mask. 1 Bus Off interrupt enabled
               + LSHIFT(1,            14) // ERRMSK . Error Mask.   1 Error interrupt enabled
               + LSHIFT(1,            13) // CLKSRC . CAN Engine Clock Source. 1 The CAN engine clock source is the peripheral clock
               + LSHIFT(0,            12) // LPB    . Loop Back Mode
               + LSHIFT(1,            11) // TWRNMSK. Tx Warning Interrupt Mask. 1 Tx Warning Interrupt enabled
               + LSHIFT(1,            10) // RWRNMSK. Rx Warning Interrupt Mask. 1 Rx Warning Interrupt enabled
               + LSHIFT(0,             7) // SMP    . CAN Bit Sampling. 0 Just one sample is used to determine the bit value
               + LSHIFT(0,             6) // BOFFREC. Bus Off Recovery. 1 Automatic recovering from Bus Off state disabled
               + LSHIFT(0,             5) // TSYN   . Timer Sync. 1 Timer Sync feature enabled
               + LSHIFT(0,             4) // LBUF   . Lowest Buffer Transmitted First. 1 Lowest number buffer is transmitted first.
               + LSHIFT(0,             3) // LOM    . Listen-Only Mode. 1 FlexCAN module operates in Listen-Only Mode.
               + LSHIFT(ctrl.propseg,  0) // PROPSEG. Propagation Segment.
  ;

  CAN->CTRL2 = 0
               + LSHIFT(1,    28) // WRMFRZ. Write-Access to Memory in Freeze mode. 1 Enable unrestricted write access to FlexCAN memory.
               + LSHIFT(0,    24) // RFFN  . Number of Rx FIFO Filters
               + LSHIFT(0x22, 19) // TASD  . Tx Arbitration Start Delay.
               + LSHIFT(0,    18) // MRP   . Mailboxes Reception Priority. 0 Matching starts from Rx FIFO and continues on Mailboxes.
               + LSHIFT(0,    17) // RRS   . Remote Request Storing. 0 Remote Response Frame is generated.
               + LSHIFT(0,    16) // EACEN . Entire Frame Arbitration Field Comparison Enable for Rx Mailboxes.0 Rx Mailbox filter’s IDE bit is always compared and RTR is never compared despite mask bits.
  ;

  for (i = 0; i < 16; i++)
  {
    CAN->RXIMR[i] = 0; // Сбросим все маски
    CAN->MB[i].CS = 0;    // Все буфферы сообщений не активны
    CAN->MB[i].ID = 0;    //
    CAN->MB[i].WORD0 = 0; //
    CAN->MB[i].WORD1 = 0; //
  }
  //CAN->RXIMR[CAN_RX_MB1] = 0xFF000000;

  // Выходим из режима Freeze
  CAN->MCR = 0
             + LSHIFT(0,  31) // MDIS   . Module Disable. 1 Disable the FlexCAN module.
             + LSHIFT(0,  30) // FRZ    . Freeze Enable. 1 Enabled to enter Freeze Mode
             + LSHIFT(0,  29) // RFEN   . Rx FIFO Enable. 1 Rx FIFO enabled
             + LSHIFT(0,  28) // HALT   . Halt FlexCAN. 1 Enters Freeze Mode if the FRZ bit is asserted.
             + LSHIFT(0,  26) // WAKMSK . Wake Up Interrupt Mask. 1 Wake Up Interrupt is enabled.
             + LSHIFT(0,  25) // SOFTRST. Soft Reset. 1 Resets the registers affected by soft reset.
             + LSHIFT(0,  23) // SUPV   . Supervisor Mode. 1 FlexCAN is in Supervisor Mode.
             + LSHIFT(0,  22) // SLFWAK . Self Wake Up. 1 FlexCAN Self Wake Up feature is enabled.
             + LSHIFT(1,  21) // WRNEN  . Warning Interrupt Enable. 1 TWRNINT and RWRNINT bits are set when the respective error counter transitions from less than 96 to greater than or equal to 96.
             + LSHIFT(1,  17) // SRXDIS . Self Reception Disable. 1 Self reception disabled
             + LSHIFT(1,  16) // IRMQ   . Individual Rx Masking and Queue Enable. 1 Individual Rx masking and queue feature are enabled.
             + LSHIFT(0,  13) // LPRIOEN. Local Priority Enable. 1 Local Priority enabled
             + LSHIFT(0,  12) // AEN    . Abort Enable. 0 Abort disabled.  !!!Не выставлять 1, поскольку процедуры инициализации MB расчитаны на режим 0
             + LSHIFT(0,   8) // IDAM   . ID Acceptance Mode. 00 Format A: One full ID (standard and extended) per ID Filter Table element.
             + LSHIFT(16,  0) // MAXMB  . Number of the Last Message Buffer.
  ;


  // Инициализация всех майлбоксов предназначенных для приема сообщений
  for (i = 0; i < RX_MBOX_CNT; i++)
  {
    CAN_set_rx_mbox(CAN, rx_mboxes[i].mb_num, rx_mboxes[i].canid, rx_mboxes[i].canid_mask, 1, 0);
    rx_events_mask |= (1 << rx_mboxes[i].mb_num);
  }


  return SUCCESS;
}



/*-------------------------------------------------------------------------------------------------------------
  Установка буффера на прием и инициация приема
  n    - номер буфера
  id   - идентификатор CAN пакета
  mask - фильтр-маска принимаемого идентификатора
  ext  - если расширенный формат то 1, иначе 0
  rtr  - если 1 то будет отправлен пакет вида Remote Request Frame 
-------------------------------------------------------------------------------------------------------------*/
void CAN_set_rx_mbox(volatile CAN_MemMapPtr CAN, INT8U n, INT32U id, INT32U mask, INT8U ext, INT8U rtr)
{
  // Переходим в Freeze режим поскольку маску фильтра можно изменить только в этом режиме
  CAN->MCR |= 0
              + BIT(30) // FRZ    . Freeze Enable. 1 Enabled to enter Freeze Mode
              + BIT(28) // HALT   . Halt FlexCAN. 1 Enters Freeze Mode if the FRZ bit is asserted.
  ;
  while ((CAN->MCR & BIT(24)) == 0)   // Ожидаем установки FRZACK
  {}

  // Сбросим флаг прерывания
  CAN->IFLAG1  |= (1 << n);

  // Инициализируем mailbox на передачу
  CAN->MB[n].CS = 0
                  + LSHIFT(0x0,     24) // CODE. 0b0000:INACTIVE- MB is not active.
                  + LSHIFT(1,       22) // SRR.  Substitute Remote Request. (должен быть 1 в расширенном формате)
                  + LSHIFT(ext,     21) // IDE.  ID Extended Bit.           1 = расширенный формат
                  + LSHIFT(rtr,     20) // RTR.  Remote Transmission Request. 1 - это mailbox с Remote Request Frame
                  + LSHIFT(0,       16) // DLC.  Длина данных
  ;
  CAN->MB[n].ID    = id;
  CAN->MB[n].WORD0 = 0;
  CAN->MB[n].WORD1 = 0;
  CAN->RXIMR[n] = mask; // Установим маску


  CAN->IMASK1  |= (1 << n); // Разрешаем прерывания после приема

  // Обозначаем готовность к приему
  CAN->MB[n].CS |= BIT(26); // 0b0100: EMPTY - MB is active and empty.

  CAN->MCR &= ~(BIT(30) + BIT(28)); // Выходим из режиме Freeze
  while (CAN->MCR & BIT(24))        // Ожидаем сброса FRZACK
  {}

}

/*-------------------------------------------------------------------------------------------------------------
  Установка буффера на отправку и инициация отправки
  n    - номер буфера
  id   - идентификатор CAN пакета
  data - буффер с данными длинной 8 байт
  len  - длина данных
  ext  - если расширенный формат то 1, иначе 0
  rtr  - если 1 то будет отправлен пакет вида Remote Request Frame 
-------------------------------------------------------------------------------------------------------------*/
int CAN_set_tx_mbox(volatile CAN_MemMapPtr CAN, INT8U n, INT32U id, INT8U *data, INT8U len, INT8U ext, INT8U rtr)
{
    INT32U cancs;
    
    cancs = CAN->MB[n].CS;

  if ( ((cancs >> 24) & 0x0F) == 0x0C ) // 0b1100: - означает что данные еще не высланы
  {
    // Если буфер еще занят то вернуть флаг ошибки
    return ERROR;
  }

  // Сбросим флаг прерывания
  CAN->IFLAG1  |= (1 << n);
  // Инициализируем mailbox на передачу
  CAN->MB[n].CS = 0
                  + LSHIFT(0x8,     24) // CODE. 0b1000:MB is not active
                  + LSHIFT(1,       22) // SRR.  Substitute Remote Request. (должен быть 1 в расширенном формате)
                  + LSHIFT(ext,     21) // IDE.  ID Extended Bit.           1 = расширенный формат
                  + LSHIFT(rtr,     20) // RTR.  Remote Transmission Request. 1 - это mailbox с Remote Request Frame
                  + LSHIFT(len,     16) // DLC.  Длина данных
  ;
  CAN->MB[n].ID = id;
  CAN->MB[n].WORD0 = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
  CAN->MB[n].WORD1 = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];

  // Разрешаем прерывания после передачи
  CAN->IMASK1  |= (1 << n);
  // Запускаем передачу
  CAN->MB[n].CS |= BIT(26); // 0b1100: DATA - MB is a Tx Data Frame (MB RTR must be 0)
  return SUCCESS;
}


/*-------------------------------------------------------------------------------------------------------------
  Выборка данных из mailbox-а с принятым пакетом
  n    - номер буфера
-------------------------------------------------------------------------------------------------------------*/
void CAN_read_rx_mbox(volatile CAN_MemMapPtr CAN, INT8U n, T_can_msg *rx)
{

  INT32U cs;
  INT32U w;
  volatile INT32U tmp;

  cs = CAN->MB[n].CS;
  rx->len  = (cs >> 16) & 0x0F;
  rx->format  = (cs >> 21) & 1;
  rx->type  = (cs >> 20) & 1;
  rx->code = (cs >> 24) & 0x0F;
  rx->canid  = CAN->MB[n].ID;
  w = CAN->MB[n].WORD0;
  rx->data[0] = (w >> 24) & 0xFF;
  rx->data[1] = (w >> 16) & 0xFF;
  rx->data[2] = (w >>  8) & 0xFF;
  rx->data[3] = (w >>  0) & 0xFF;
  w = CAN->MB[n].WORD1;
  rx->data[4] = (w >> 24) & 0xFF;
  rx->data[5] = (w >> 16) & 0xFF;
  rx->data[6] = (w >>  8) & 0xFF;
  rx->data[7] = (w >>  0) & 0xFF;
  tmp = CAN->TIMER; // Разблокировка Mialbox

}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
int Send_can_msg(T_can_msg *can_msg)
{
  return CAN_set_tx_mbox(CAN0_BASE_PTR, CAN_TX_MB1, can_msg->canid, can_msg->data, can_msg->len, can_msg->format, can_msg->type);
}



/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
int  Read_can_msg(T_can_msg *rcv_msg)
{

  T_can_msg  rx;

  INT32U    n;

  INT32U    flags;



  __disable_interrupt();
  flags = g_can_flags;
  __enable_interrupt();

  // Проверяем флаги  приема пакета
  if ( (flags & rx_events_mask) != 0 )
  {

    // Проверяем по всем приемникам
    for (n = 0; n < RX_MBOX_CNT; n++)
    {
      if ( flags & (1 << rx_mboxes[n].mb_num) )
      {
        CAN_read_rx_mbox(CAN0_BASE_PTR, rx_mboxes[n].mb_num, &rx);


        // Парсинг полученного пакета

        if ( rx.canid == ALL_PROLONG_BOOT_MODE )
        {
          // Продлим время загрузочного периода
          g_prolong_boot = 1;
          __disable_interrupt();
          g_can_flags &= ~(1 << rx_mboxes[n].mb_num);
          __enable_interrupt();
          continue;
        }
        if ( rx.canid == ALL_ENTER_TO_WRK_MOD )
        {
          Check_and_Jump_to_App();
          __disable_interrupt();
          g_can_flags &= ~(1 << rx_mboxes[n].mb_num);
          __enable_interrupt();
          continue;
        }

        memcpy(rcv_msg, &rx, sizeof(T_can_msg));
        __disable_interrupt();
        g_can_flags &= ~(1 << rx_mboxes[n].mb_num);
        __enable_interrupt();
        return SUCCESS;
      }
    }
  }
  return ERROR;
}
