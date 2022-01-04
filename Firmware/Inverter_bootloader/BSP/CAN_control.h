#ifndef __CAN_CONTROL
  #define __CAN_CONTROL

  #define CAN_DEVICE  0

  #define CAN_SPEED   100000

  #define CAN_MIN_TQ  8
  #define CAN_MAX_TQ  25

  #define CAN_MAX_PROPSEG 8
  #define CAN_MAX_RJW 4
  #define CAN_MIN_PSEG2 2

  
  // Майлбоксы на чтение 
  #define CAN_RX_MB1     0
  #define CAN_RX_MB2     1
  #define CAN_RX_MB3     2
  #define CAN_RX_MB4     3
  #define CAN_RX_MB5     4
  #define CAN_RX_MB6     5
  #define CAN_RX_MB7     6
  #define CAN_RX_MB8     7
  // Всего майлбоксов на чтение
  #define RX_MBOX_CNT    8

  // Майлбоксы на передачу 
  #define CAN_TX_MB1     8

  // Количество сообщений в логе приемника
  #define CAN_RX_LOG_SZ  128



typedef struct
{
  INT32U pseg1;
  INT32U pseg2;
  INT32U propseg;
  INT32U rjw;
  INT32U presdiv;
}
T_can_ctrl;


typedef struct 
{
  INT32U canid;   // Полученый идентификатор CAN пакета           
  INT8U  data[8]; // Буффер с данными длинной 8 байт                          
  INT8U  len;     // Длина данных                                
  INT8U  format;  // Формат идентификатора           0 - STANDARD,   1 - EXTENDED IDENTIFIER    
  INT8U  type;    // Признак  Remote Request Frame   0 - DATA FRAME, 1 - REMOTE FRAME      
  INT8U  code;    // Message Buffer Code
} 
T_can_msg;

typedef struct
{
  INT8U  mb_num;
  INT32U canid; 
  INT32U canid_mask; 

} T_can_rx_config;



int  CAN_init(CAN_MemMapPtr ptr, INT32U bitrate);
void CAN_Tx_Task(INT32U parameter);
void CAN_Rx_Task(INT32U parameter);
int  Read_can_msg(T_can_msg *rcv_msg);
int  Send_can_msg(T_can_msg *can_msg);
int  CAN_set_tx_mbox(volatile CAN_MemMapPtr CAN, INT8U n, INT32U id, INT8U *data, INT8U len, INT8U ext, INT8U rtr);
void CAN_set_rx_mbox(volatile CAN_MemMapPtr CAN, INT8U n, INT32U id, INT32U mask, INT8U ext, INT8U rtr);
#endif
