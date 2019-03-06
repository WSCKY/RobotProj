#ifndef __COMTYPES_H
#define __COMTYPES_H

#include <stdint.h>
#include <sys/_stdint.h>

/* __packed keyword used to decrease the data type alignment to 1-byte */
#if defined (__CC_ARM)         /* ARM Compiler */
  #define __PACK_BEGIN  __packed
  #define __PACK_END
#elif defined (__ICCARM__)     /* IAR Compiler */
  #define __PACK_BEGIN  __packed
  #define __PACK_END
#elif defined   ( __GNUC__ )   /* GNU Compiler */
  #define __PACK_BEGIN
  #define __PACK_END    __attribute__ ((__packed__))
#elif defined   (__TASKING__)  /* TASKING Compiler */
  #define __PACK_BEGIN  __unaligned
  #define __PACK_END
#endif /* __CC_ARM */

/* exact-width signed integer types */
#ifndef int8_t
typedef   signed          char int8_t;
#endif
#ifndef int16_t
typedef   signed short     int int16_t;
#endif
//#ifndef int32_t
//typedef   signed           int int32_t;
//#endif

/* exact-width unsigned integer types */
#ifndef uint8_t
typedef unsigned          char uint8_t;
#endif
#ifndef uint16_t
typedef unsigned short     int uint16_t;
#endif
//#ifndef uint32_t
//typedef unsigned           int uint32_t;
//#endif

typedef void (*PortRecvByteCallback)(uint8_t Data);

#endif /* __COMTYPES */
