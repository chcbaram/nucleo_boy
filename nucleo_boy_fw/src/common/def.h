/*
 *  def.h
 *
 *  Created on: 2016. 5. 14.
 *      Author: Baram
 */

#ifndef DEF_H
#define DEF_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#ifndef BOOL
#define BOOL uint8_t
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define PROGMEM


#define OK    0


#include "def_err.h"
#include "binary.h"



#define _DEF_UART1          0
#define _DEF_UART2          1
#define _DEF_UART3          2
#define _DEF_UART4          3
#define _DEF_UART5          4
#define _DEF_UART6          5

#define _DEF_TIMER1         0
#define _DEF_TIMER2         1
#define _DEF_TIMER3         2
#define _DEF_TIMER4         3

#define _DEF_SPI1           0
#define _DEF_SPI2           1
#define _DEF_SPI3           2
#define _DEF_SPI4           3



#define _DEF_INPUT          0
#define _DEF_OUPUT          1

#define _DEF_HIGH           1
#define _DEF_LOW            0



#define PI                  3.1415926535897932384626433832795
#define HALF_PI             1.5707963267948966192313216916398
#define TWO_PI              6.283185307179586476925286766559
#define DEG_TO_RAD          0.017453292519943295769236907684886
#define RAD_TO_DEG          57.295779513082320876798154814105
#define EULER               2.718281828459045235360287471352




#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif


#define constrain(amt,low,high)         ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define radians(deg)                    ((deg)*DEG_TO_RAD)
#define degrees(rad)                    ((rad)*RAD_TO_DEG)
#define sq(x)                           ((x)*(x))

#define lowByte(w)                      ((uint8_t) ((w) & 0xff))
#define highByte(w)                     ((uint8_t) ((w) >> 8))

#define bitRead(value, bit)             (((value) >> (bit)) & 0x01)
#define bitSet(value, bit)              ((value) |= (1UL << (bit)))
#define bitClear(value, bit)            ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue)  (bitvalue ? bitSet(value, bit) : bitClear(value, bit))




typedef struct
{
  uint32_t  ptr_in;
  uint32_t  ptr_out;
  uint32_t  length;
  uint8_t  *p_buf;
} ring_buf_t;




typedef uint8_t err_code_t;

typedef bool      boolean;
typedef uint8_t   byte;
typedef uint16_t  word;



typedef void (*voidFuncPtr)(void);













#endif
