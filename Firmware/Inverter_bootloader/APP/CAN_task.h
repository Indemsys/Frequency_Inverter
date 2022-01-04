#ifndef __CAN_TASK_H
#define __CAN_TASK_H


#define COMCH_BLOCK_MAX_SZ 512



typedef enum
{
  STANDARD_FORMAT = 0, 
  EXTENDED_FORMAT
} CAN_FORMAT;


typedef enum
{
  DATA_FRAME = 0, 
  REMOTE_FRAME
} CAN_FRAME;

void CAN_loader(void);

void Get_block_CRC(INT8U *block, INT16U len, INT8U *dest);

#endif