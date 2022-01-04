#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include "App.h"

static int test_cnt;

/*-------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------*/
static void Test_space_vector_modulator(void)
{
  int                            i;
  int                            angle_int;
  float                          angle_flt;
  MCLIB_2_COOR_SYST_ALPHA_BETA_T tr32InVoltage;
  MCLIB_3_COOR_SYST_T            tr32PwmABC;
  UWord32                        u32SvmSector;

  printf("Space vector modulation test.\r\n");
  for (i = 0; i < 360; i++)
  {
    int rem, c;


    Led_control(LED1, 1);
    angle_int = i % 180;
    c     = i / 180;
    if ( c & 1 )
    {
      // Нечетное
      angle_int = -180 + angle_int;
    }
    angle_flt = angle_int / 180.0;

    tr32InVoltage.f32Alpha = GFLIB_Cos(FRAC32(angle_flt));
    tr32InVoltage.f32Beta  = GFLIB_Sin(FRAC32(angle_flt));
    u32SvmSector = GMCLIB_SvmStd(&tr32PwmABC, &tr32InVoltage);
    Led_control(LED1, 0);

    {
      float alpha, beta, f32A, f32B, f32C;
      alpha = tr32InVoltage.f32Alpha / (float)0x80000000;
      beta  = tr32InVoltage.f32Beta / (float)0x80000000;

      f32A  = tr32PwmABC.f32A / (float)0x80000000;
      f32B  = tr32PwmABC.f32B / (float)0x80000000;
      f32C  = tr32PwmABC.f32C / (float)0x80000000;
      printf("Angle=%03d alpha=%6.3f beta=%6.3f s=%d A=%6.3f B=%6.3f C=%6.3f\r\n", i, alpha, beta, u32SvmSector, f32A, f32B, f32C);
    }
  }
}


