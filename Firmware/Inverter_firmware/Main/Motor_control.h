#ifndef __MOTOR_CONTROL
  #define __MOTOR_CONTROL

#define  PWM_OE_RISE       BIT(0) // Подъем на сигнале разрешении сигналов PWM к драйверу двигателя
#define  PWM_OE_FALL       BIT(1) // Спад   на сигнале разрешении сигналов PWM к драйверу двигателя
#define  VFO_RISE          BIT(2) // Подъем на сигнале аварии драйвера
#define  VFO_FALL          BIT(3) // Спад   на сигнале аварии драйвера
#define  MOTOR_START_DOWN  BIT(4) // Старт  двигателя влево
#define  MOTOR_START_UP    BIT(5) // Старт  двигателя вправо
#define  MOTOR_STOP        BIT(6) // Остановка двигателя
#define  SMPL_ARR1_FULL    BIT(7) // Заполнен массив 1 отсчетов
#define  SMPL_ARR2_FULL    BIT(8) // Заполнен массив 2 отсчетов
#define  MEAS_RES_READY    BIT(9) // Готовность результатов статистических измерений сигналов


#define  MAX_MOT_FREQ       50
#define  PWM_FREQ           16000
#define  PWM_MODULO        (60000000UL/(2*PWM_FREQ))

// Поскольку обновление компараторов происходит при достижении счетчиком максимального значения,
// то для верхнего уровня самый короткий импульс будет в ситуации когда сразу после обновления выход обнулится
#define  PWM_MARGIN_HI_LEV  180// Импульс не короче 3 мкс

// Здесь в тактах даем в два раза меньшую величину поскольку в расчет берется импульс при убывании и его продолжение при нарастании
#define  PWM_MARGIN_LO_LEV  90 // Импульс не короче 3 мкс.

#define  PWM_SCALE_TRASITION_TIME 0.5 // Время в сек за которое коэффициент масштабирования PWM меняет значение

#define  MOVING_DOWN        1
#define  MOVING_UP          0

#define  MOT_IDLE           0   // Мотор остановлен или останавливается
#define  MOT_START_ACTION   1   // Идентификатор процесса старта движения
#define  MOT_STOP_ACTION    2   // Идентификатор процесса остановки движения
#define  MOT_UNIFORM_MOTION 3   // Равномерное движение

#define  MIN_FREQ           4   // Частота при снижении до которой происходит полная остановка двигателя
#define  START_FREQ         5   // Частота с которой стартует вращение двигателя



typedef struct PWM_CBL
{
  unsigned int       action;         // Фаза движения. 1- старт, 2- процесс торможения, 0 - равномерное движение
  unsigned int       direction;      // Направление вращения. Принимает значения MOVING_DOWN и MOVING_UP
  unsigned int       skew_cnt;       // Счетчик этапа изменения частоты вращения
  unsigned int       mot_freq;       // Частота вращения двигателя
  unsigned long long ll_mot_freq;    // Значение текущей частоты в 64-х битном формате 64,32
  signed long long   ll_step;        // Значение шага частоты в 64-х битном формате
  int                motor_drv_fail; // Флаг ошибки драйвера мотора.
  int                fail_cnt;       // Счетчик ошибок.

  float              up_accel_pwm_scale;  // Коэффициент масштабирования PWM при ускорении  и движении вверх . Более 0.5 означает насыщенную синусоиду
  float              up_decel_pwm_scale;  // Коэффициент масштабирования PWM при замедлении и движении вверх . Более 0.5 означает насыщенную синусоиду
  float              up_pwm_scale;        // Коэффициент масштабирования PWM при установившейся скорости и движении вверх . Более 0.5 означает насыщенную синусоиду
  int                up_move_freq;
  int                up_acceler_time;
  int                up_deceler_time;
  float              down_accel_pwm_scale;// Коэффициент масштабирования PWM при ускорении  и движении вниз . Более 0.5 означает насыщенную синусоиду
  float              down_decel_pwm_scale;// Коэффициент масштабирования PWM при замедлении и движении вниз . Более 0.5 означает насыщенную синусоиду
  float              down_pwm_scale;      // Коэффициент масштабирования PWM при установившейся скорости и движении вниз . Более 0.5 означает насыщенную синусоиду
  int                down_move_freq;
  int                down_acceler_time;
  int                down_deceler_time;
  float              pwm_scale;
  float              pwm_scale_delta;
  unsigned int       pwm_scale_cnt;
}
T_MC_CBL, _PTR_ T_MC_CBL_ptr;

typedef struct
{
  int pwm_a;
  int pwm_b;
  int pwm_c;
}
T_3ph_pwm;

void      Control_task(uint_32 initial_data);
void      Motor_ISR_task(uint_32 initial_data);
void      PWM_start(int dir, unsigned int freq);
void      PWM_stop(void);
int       PWM_state(void);
void      PWM_from_Cnt(T_3ph_pwm *pmp_3ph_ptr);
_mqx_uint MC_get_events(_mqx_uint *events, _mqx_uint ticks, _mqx_uint events_mask);
void      MC_create_event(void);
_mqx_uint MC_set_events(_mqx_uint evnt);

void MC_get_CBL(T_MC_CBL *cbl_ptr);
void MC_set_CBL(T_MC_CBL *cbl_ptr);

void      MC_start_motor_moving(INT8U dir,  INT32U target_freq, INT32U time);
void      MC_stop_motor_moving(INT32U time);
void      MC_emergency_stop_motor(void);
void      MC_init_PWM(void);
T_MC_CBL *MC_get_pcbl(void);

void      MC_init_speed_change(unsigned int action,  unsigned int target_freq, unsigned int target_time);

#endif
