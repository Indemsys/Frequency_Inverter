#include "App.h"

LWEVENT_STRUCT can_event;



const T_can_rx_config rx_mboxes[RX_MBOX_CNT] =
{
  { CAN_RX_MB1, INVERT_REQ, 0xFFF00000 },
  { CAN_RX_MB2, INVERT_REQ, 0xFFF00000 },
  { CAN_RX_MB3, INVERT_REQ, 0xFFF00000 },
  { CAN_RX_MB4, INVERT_REQ, 0xFFF00000 },
  { CAN_RX_MB5, INVERT_REQ, 0xFFF00000 },
  { CAN_RX_MB6, INVERT_REQ, 0xFFF00000 },
  { CAN_RX_MB7, INVERT_REQ, 0xFFF00000 },
  { CAN_RX_MB8, INVERT_REQ, 0xFFF00000 },
};

INT32U       rx_log_head;
INT32U       rx_log_tail;
T_can_rx     rx_log[CAN_RX_LOG_SZ];
LWSEM_STRUCT can_log_sem;


/*-------------------------------------------------------------------------------------------------------------
  Обслуживание прерываний CAN шины при отправке и получении данных
-------------------------------------------------------------------------------------------------------------*/
void CAN_isr(pointer ptr)
{
  volatile CAN_MemMapPtr   CAN;
  INT32U                   reg;

  CAN = (CAN_MemMapPtr)ptr;

  reg = CAN->IFLAG1; // Получить флаги прерываний (всего в работе 16-ть младших флагов).
                     // Если не разрешен FIFO то каждый флаг отмечает прерывание от соответствующего буфера.
                     // Снимаються флаги записью 1
                     // Каждый флаг может отмечать, как прерывание по приему так и по передаче.
  if ( reg )
  {
    _lwevent_set(&can_event, reg); // Создать для каждого флага событие
    CAN->IFLAG1 = reg;             // Сбрасываем флаги, но только те о которых мы сообщили
  }
}
/*-------------------------------------------------------------------------------------------------------------
  Обслуживание прерываний CAN шины в результате ошибок м других событий
-------------------------------------------------------------------------------------------------------------*/
void CAN_err_isr(pointer ptr)
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
    return MQX_ERROR;
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
    return MQX_ERROR;
  }

  return MQX_OK;

}

/*-------------------------------------------------------------------------------------------------------------
  Обслуживание прерываний CAN шины
 
  ptr - указатель на область регистров контроллера CAN:  CAN0_BASE_PTR или CAN1_BASE_PTR  
-------------------------------------------------------------------------------------------------------------*/
int CAN_init(CAN_MemMapPtr ptr, INT32U bitrate)
{
  volatile CAN_MemMapPtr   CAN = ptr;
  SIM_MemMapPtr            SIM = SIM_BASE_PTR;
  INT32U                   clk = BSP_SYSTEM_CLOCK / 2;
  T_can_ctrl               ctrl;
  INT32U                   i;


  _lwevent_create(&can_event, 0);
  _lwsem_create(&can_log_sem, 1);

  if ( ptr == CAN0_BASE_PTR )
  {
    SIM->SCGC6 |= BIT(4);  // Разрешаем тактирование CAN0
    CAN->CTRL1 |= BIT(13); // Тактирование от источника тактирования периферии

    // Инициализация всех прерываний
    _int_install_isr(INT_CAN0_ORed_Message_buffer, CAN_isr, (void *)ptr);
    _bsp_int_init(INT_CAN0_ORed_Message_buffer, CAN_ISR_PRIO, 0, TRUE);
    _int_install_isr(INT_CAN0_Bus_Off,    CAN_err_isr,  (void *)ptr);
    _bsp_int_init(INT_CAN0_Bus_Off,       CAN_ISR_PRIO, 0, TRUE);
    _int_install_isr(INT_CAN0_Error,      CAN_err_isr,  (void *)ptr);
    _bsp_int_init(INT_CAN0_Error,         CAN_ISR_PRIO, 0, TRUE);
    _int_install_isr(INT_CAN0_Tx_Warning, CAN_err_isr,  (void *)ptr);
    _bsp_int_init(INT_CAN0_Tx_Warning,    CAN_ISR_PRIO, 0, TRUE);
    _int_install_isr(INT_CAN0_Rx_Warning, CAN_err_isr,  (void *)ptr);
    _bsp_int_init(INT_CAN0_Rx_Warning,    CAN_ISR_PRIO, 0, TRUE);
    _int_install_isr(INT_CAN0_Wake_Up,    CAN_err_isr,  (void *)ptr);
    _bsp_int_init(INT_CAN0_Wake_Up,       CAN_ISR_PRIO, 0, TRUE);
  }
  else if ( ptr == CAN1_BASE_PTR )
  {
    SIM->SCGC3 |= BIT(4);  // Разрешаем тактирование CAN1
    CAN->CTRL1 |= BIT(13); // Тактирование от источника тактирования периферии

    // Инициализация всех прерываний
    _int_install_isr(INT_CAN1_ORed_Message_buffer, CAN_isr, (void *)ptr);
    _bsp_int_init(INT_CAN1_ORed_Message_buffer, CAN_ISR_PRIO, 0, TRUE);
    _int_install_isr(INT_CAN1_Bus_Off,    CAN_err_isr,  (void *)ptr);
    _bsp_int_init(INT_CAN1_Bus_Off,       CAN_ISR_PRIO, 0, TRUE);
    _int_install_isr(INT_CAN1_Error,      CAN_err_isr,  (void *)ptr);
    _bsp_int_init(INT_CAN1_Error,         CAN_ISR_PRIO, 0, TRUE);
    _int_install_isr(INT_CAN1_Tx_Warning, CAN_err_isr,  (void *)ptr);
    _bsp_int_init(INT_CAN1_Tx_Warning,    CAN_ISR_PRIO, 0, TRUE);
    _int_install_isr(INT_CAN1_Rx_Warning, CAN_err_isr,  (void *)ptr);
    _bsp_int_init(INT_CAN1_Rx_Warning,    CAN_ISR_PRIO, 0, TRUE);
    _int_install_isr(INT_CAN1_Wake_Up,    CAN_err_isr,  (void *)ptr);
    _bsp_int_init(INT_CAN1_Wake_Up,       CAN_ISR_PRIO, 0, TRUE);
  }
  else    return MQX_ERROR;

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

  return MQX_OK;
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
void CAN_set_tx_mbox(volatile CAN_MemMapPtr CAN, INT8U n, INT32U id, INT8U *data, INT8U len, INT8U ext, INT8U rtr)
{
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
}


/*-------------------------------------------------------------------------------------------------------------
  Выборка данных из mailbox-а с принятым пакетом
  n    - номер буфера
-------------------------------------------------------------------------------------------------------------*/
void CAN_read_rx_mbox(volatile CAN_MemMapPtr CAN, INT8U n, T_can_rx *rx)
{

  INT32U cs;
  INT32U w;
  volatile INT32U tmp;

  cs = CAN->MB[n].CS;
  rx->len  = (cs >> 16) & 0x0F;
  rx->ext  = (cs >> 21) & 1;
  rx->rtr  = (cs >> 20) & 1;
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
void CAN_Tx_Task(uint_32 parameter)
{
  volatile CAN_MemMapPtr   CAN = (CAN_MemMapPtr)parameter;

  INT8U databl[8] =
  {
    0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  // Периодическая отправка тестового сообщения
  while (1)
  {
/*
    CAN_set_tx_mbox(CAN, CAN_TX_MB1, PDISPLx_REQ, databl, 3, 1, 0);
    databl[1]++;
    if ( databl[1] > 15 )
    {
      databl[1] = 0;
    }
*/
    _time_delay(1000);
  }

}


/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
_mqx_uint CAN_wait_log_rec(_mqx_uint ticks)
{
  if ( _lwevent_wait_ticks(&can_event, 0x80000000, FALSE, ticks) == MQX_OK )
  {
    _lwevent_clear(&can_event, 0x80000000);
    return MQX_OK;
  }
  return MQX_ERROR;
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
_mqx_uint CAN_pop_log_rec(T_can_rx  *rx)
{
  _lwsem_wait(&can_log_sem);
  if ( rx_log_head != rx_log_tail )
  {
    *rx = rx_log[rx_log_tail];
    rx_log_tail++;
    if ( rx_log_tail >= CAN_RX_LOG_SZ )
    {
      rx_log_tail = 0; 
    }
    _lwsem_post(&can_log_sem);
    return MQX_OK;
  }
  _lwsem_post(&can_log_sem);
  return MQX_ERROR;
}
/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
static _mqx_uint CAN_push_log_rec(T_can_rx  *rx)
{
  _lwsem_wait(&can_log_sem);
  rx_log[rx_log_head] = *rx;
  rx_log_head++;
  if ( rx_log_head >= CAN_RX_LOG_SZ )
  {
    rx_log_head = 0;
  }
  if ( rx_log_head == rx_log_tail )
  {
    rx_log_tail++;  
    if ( rx_log_tail >= CAN_RX_LOG_SZ )
    {
      rx_log_tail = 0; 
    }
  }
  _lwsem_post(&can_log_sem);

  _lwevent_set(&can_event, 0x80000000); // Уведомить о записи в лог
  return 0;
}

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
void CAN_Rx_Task(uint_32 parameter)
{
  volatile CAN_MemMapPtr   CAN = (CAN_MemMapPtr)parameter;
  _mqx_uint events;
  T_can_rx  rx;
  T_MC_CBL     *mc_pcbl;


  INT32U    n;
  INT32U    events_mask = 0;

  mc_pcbl = MC_get_pcbl();

  rx_log_head=0;
  rx_log_tail=0;
  // Инициализация всех майлбоксов предназначенных для приема сообщений
  for (n = 0; n < RX_MBOX_CNT; n++)
  {
    CAN_set_rx_mbox(CAN, rx_mboxes[n].mb_num, rx_mboxes[n].canid, rx_mboxes[n].canid_mask, 1, 0);
    events_mask |= (1 << rx_mboxes[n].mb_num);
  }

  while (1)
  {

    if ( _lwevent_wait_ticks(&can_event, events_mask, FALSE, 0) == MQX_OK )
    {
      events = _lwevent_get_signalled();
      _lwevent_clear(&can_event, events);

      for (n = 0; n < RX_MBOX_CNT; n++)
      {
        if ( events & (1 << rx_mboxes[n].mb_num) )
        {
          CAN_read_rx_mbox(CAN, rx_mboxes[n].mb_num, &rx);
 
          CAN_push_log_rec(&rx);

          // Парсинг полученного пакета 
          
          if ( rx.canid == INVERT_REQ )
          {
            switch (rx.data[0])
            {
            case START_MOVING:
              
              if ( rx.data[1] == MOVING_UP )
              {
                 mc_pcbl->up_move_freq    = rx.data[2];
                 mc_pcbl->up_acceler_time = rx.data[3];
                 MC_set_events(MOTOR_START_UP);
              }
              else if ( rx.data[1] == MOVING_DOWN )
              {
                mc_pcbl->down_move_freq    = rx.data[2];    
                mc_pcbl->down_acceler_time = rx.data[3];    
                MC_set_events(MOTOR_START_DOWN);
              }

              //MC_start_motor_moving(rx.data[1], rx.data[2], rx.data[3]);
              break;
            case STOP_MOVING:
              if ( mc_pcbl->direction == MOVING_DOWN )
              {
                mc_pcbl->down_deceler_time = rx.data[1];
              }
              else
              {
                mc_pcbl->up_deceler_time = rx.data[1];
              }

              MC_set_events(MOTOR_STOP);

              //MC_stop_motor_moving(rx.data[1]);
              break;

            case EMERGENCY_STOP_MOVING:
              MC_emergency_stop_motor();
              break;
            }
          }
        }
      }
    }
  }
}
