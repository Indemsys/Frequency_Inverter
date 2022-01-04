#ifndef  __APP_TYPES
  #define  __APP_TYPES


#define     BIT(n) (1u << n)
#define     LSHIFT(v,n) (((unsigned int)(v) << n))



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



#endif
