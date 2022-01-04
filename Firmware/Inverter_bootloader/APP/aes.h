#ifndef __AES_H
  #define __AES_H


  #define KEY_COUNT 3

extern void aesInit(void);
extern void aesDecrypt(unsigned char *buffer, unsigned char *chainBlock);

#endif

