#ifndef  __APP_TYPES
  #define  __APP_TYPES

  #define     BIT(n) (1u << n)
  #define     LSHIFT(v,n) (((unsigned int)(v) << n))

  #ifndef TRUE
    #define FALSE 0
    #define TRUE 1
  #endif

  #define RES_OK   (1)
  #define RES_NONE (0)

  #define  ERROR     (-1)
  #define  SUCCESS     0
  #define  BUSY      (-2)



  #define BYTESWAP16(x)	(INT16U)((((x) & 0xFF00) >> 0x8) | (((x) & 0xFF) << 0x8))
  #define BYTESWAP32(val)	(INT32U)((BYTESWAP16((INT32U)(val) & (INT32U)0xFFFF) << 0x10) |  \
					(BYTESWAP16((INT32U)((val) >> 0x10))))

  #define ieee_htons(x)		BYTESWAP16(x)
  #define ieee_htonl(x)		BYTESWAP32(x)
  #define ieee_ntohs(x)		BYTESWAP16(x)
  #define ieee_ntohl(x)		BYTESWAP32(x)

  #define UNUSED(x)   (void)x;


typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef unsigned long  INT64U;                   /* Unsigned 64 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef volatile unsigned long   VINT32U;
typedef volatile unsigned short  VINT16U;
typedef volatile unsigned char   VINT8U;

typedef INT8U  *INT8U_ptr;          /* ptr to 8-bit*/
typedef INT16U *INT16U_ptr;         /* ptr to 16-bit */
typedef INT32U *INT32U_ptr;         /* ptr to 32-bit*/

/* definition of 8 bit word */
typedef union _BYTE
{
  INT8U _byte;
  struct
  {
    unsigned b0:1;
    unsigned b1:1;
    unsigned b2:1;
    unsigned b3:1;
    unsigned b4:1;
    unsigned b5:1;
    unsigned b6:1;
    unsigned b7:1;
  }Bit;
} BYTE;

/* definition of 16 bit word */
typedef union _WORD
{
  INT16U _word;
  struct
  {
    INT8U byte1;
    INT8U byte0;
  }_byte;
  struct
  {
    BYTE HighB;
    BYTE LowB;
  }_Byte;
} WORD;

/* definition of 32 bit word */
typedef union _DWORD
{
  INT32U _dword;
  struct
  {
    INT8U byte3;
    INT8U byte2;
    INT8U byte1;
    INT8U byte0;
  }_byte;
  struct
  {
    INT16U word1;
    INT16U word0;
  }_word;
  struct
  {
    BYTE Byte3;
    BYTE Byte2;
    BYTE Byte1;
    BYTE Byte0;
  }_Byte;
  struct
  {
    WORD Word1;
    WORD Word0;
  }_Word;
} DWORD;

extern void Delay_m8(int cnt); // Задержка на (cnt+1)*8 тактов . Передача нуля недопускается

  #define  DELAY_1us    Delay_m8(15-1)
  #define  DELAY_2us    Delay_m8(2*15-1)
  #define  DELAY_4us    Delay_m8(4*15-1)
  #define  DELAY_8us    Delay_m8(8*15-1)
  #define  DELAY_30us   Delay_m8(30*15-1)
  #define  DELAY_ms(x)  Delay_m8(1000*15*x-1)


#endif
