#ifndef __ADC_CONTROL
  #define __ADC_CONTROL



#define SIG_II_W_CHANNEL     16 // ADC0 16
#define SIG_V_BUS_CHANNEL    17 // ADC0 17

#define SIG_II_V_CHANNEL     16 // ADC1 16
#define SIG_TEMPER_CHANNEL   26 // ADC1 26 Температура кристала

#define SIG_II_U_CHANNEL     16 // ADC2 16
#define SIG_V_U_CHANNEL      12 // ADC2 12

#define SIG_15V_MEAS_CHANNEL  7 // ADC3 7a (после тюнинга на плате ver. 1.0)
#define SIG_5V_MEAS_CHANNEL   5 // ADC3 5a (после тюнинга на плате ver. 1.0)


#define TST_SMPLS_ARR_SZ  (800*2) //  Общее время выборки всех сэмплов 50 мс


#define ADC_AVER_4   1
#define ADC_AVER_8   2
#define ADC_AVER_16  3
#define ADC_AVER_32  4





typedef struct
{

  // Результаты калибровки модулей ADC
  int adc0_cal_res;
  int adc1_cal_res;
  int adc2_cal_res;
  int adc3_cal_res;

} T_ADC_state;


typedef struct
{
  int    maxv;
  int    minv;
  int    averv;
  int long long    rmsv;
} T_meas_stat;


typedef struct
{
  float  favr;
  float  fmin;
  float  fmax;
  float  frms;
} T_meas_results;

#define  FILTR_AVER_SZ  16

typedef struct
{
  unsigned short smpl_ii_w   ; // Текущий отсчет сигнала
  unsigned short smpl_v_bus  ; // Текущий отсчет сигнала
  unsigned short smpl_ii_v   ; // Текущий отсчет сигнала
  unsigned short smpl_temper ; // Текущий отсчет сигнала
  unsigned short smpl_ii_u   ; // Текущий отсчет сигнала
  unsigned short smpl_v_u    ; // Текущий отсчет сигнала
  unsigned short smpl_15v    ; // Текущий отсчет сигнала
  unsigned short smpl_5v     ; // Текущий отсчет сигнала

  int ii_w_offs; // Накопленное смещение
  int ii_v_offs; // Накопленное смещение
  int ii_u_offs; // Накопленное смещение

  int ii_w; // Отфильтрованный от постоянной составляющей сигнал  ii_w
  int ii_v; // Отфильтрованный от постоянной составляющей сигнал  ii_v
  int ii_u; // Отфильтрованный от постоянной составляющей сигнал  ii_u

  unsigned short v_bus_arr[FILTR_AVER_SZ];
  int            v_bus_acc;
  unsigned short v15v_arr[FILTR_AVER_SZ];
  int            v15v_acc;
  unsigned short v5v_arr[FILTR_AVER_SZ];
  int            v5v_acc;
  unsigned int   aai;

  int            v_bus; // Отфильтрованное значение сигнала v_bus;

} T_ADC_res;

typedef struct
{
  const char   *name;
  float        (*int_converter)(int);
  float        (*flt_converter)(float);

} T_vals_scaling;

void ADCs_prepare(void);
void ADC_switch_on_all(void);
void ADC_init(ADC_MemMapPtr ADC);
void ADC_set_channel(ADC_MemMapPtr ADC,int ch, int diff, int reg);
void ADC_activate_hrdw_trig(ADC_MemMapPtr ADC);
void ADC_activate_soft_trig(ADC_MemMapPtr ADC);
void ADC_print_regs(ADC_MemMapPtr ADC);
int  ADC_calibrating(ADC_MemMapPtr ADC);
signed short   ADC_get_sample(ADC_MemMapPtr ADC, INT32U channel, INT32U diff, INT32U aver);

void PDB_configure_ADC_triggers(void);
void PDB_activate_ADC_triggers(void);
void PDB_deactivate_ADC_triggers(void);
void PDB_set_delays(int delay1, int delay2);

//void ADC_reset_smpl_arr_pos(void);
int  ADC_is_smpl_arr_ready(void);
void ADC_get_arr_smp(unsigned int indx, signed short *arr);
void ADC_get_smpl_arr(signed short **ptr);
T_ADC_state *ADC_get_state(void);

void Get_copy_meas_results(T_meas_results * mres);
#endif
