#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include "App.h"

#define I2C_DEVICE_POLLED    "i2c0:"
#define I2C_DEVICE_INTERRUPT "ii2c0:"
#define I2C_MAX31725_ADDRESS  (0x90>>1)
#define I2C_SPEED            400000 

static  MQX_FILE_PTR   i2c0_fd;


/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
_mqx_int TempCtrl_init_drivers(void)
{
  _mqx_int param;

  i2c0_fd = fopen(I2C_DEVICE_POLLED, NULL);
  if ( i2c0_fd == NULL )
  {
    return MQX_ERROR;
  }
  param = I2C_SPEED;
  if ( ioctl(i2c0_fd, IO_IOCTL_I2C_SET_BAUD, &param)!= I2C_OK )
  {
    fclose(i2c0_fd);
    return MQX_ERROR;
  }
  if (ioctl(i2c0_fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL)!= I2C_OK )
  {
    fclose(i2c0_fd);
    return MQX_ERROR;
  }
  return MQX_OK;
}


/*-----------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------*/
_mqx_int TempCtrl_read_IGBT_temperature(float *temp_ptr)
{
  _mqx_int  param;
  _mqx_int  result;
  uchar     buf[4];
  _mqx_int  n;
  float     f;

  param = I2C_MAX31725_ADDRESS;
  if ( ioctl(i2c0_fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param)!= I2C_OK )
  {
    return MQX_ERROR;
  }
  fwrite(buf, 1, 0, i2c0_fd);
  if ( ioctl(i2c0_fd, IO_IOCTL_FLUSH_OUTPUT, &param) == I2C_OK )
  {
    if ( param )
    {
      ioctl(i2c0_fd, IO_IOCTL_I2C_STOP, NULL);
      return MQX_ERROR;
    }
  }
  else
  {
    return MQX_ERROR;
  }
  buf[0] = 0;
  result = fwrite(buf, 1, 1, i2c0_fd);
  if ( 1 != result )
  {
    return MQX_ERROR;
  }
  if (ioctl(i2c0_fd, IO_IOCTL_I2C_REPEATED_START, NULL)!= I2C_OK  )
  {
    return MQX_ERROR;
  }

  n = 2; // Читаем 2-а байта
  param = n;
  if (ioctl(i2c0_fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param)!= I2C_OK)
  {
    return MQX_ERROR;
  }

  result = fread(buf, 1, n, i2c0_fd);
  if ( result != n )
  {
    return MQX_ERROR;
  }

  ioctl(i2c0_fd, IO_IOCTL_I2C_STOP, NULL);


  f =  (float)buf[0] + (float)buf[1]/256.0;
  *temp_ptr = f;
  return MQX_OK;
}
