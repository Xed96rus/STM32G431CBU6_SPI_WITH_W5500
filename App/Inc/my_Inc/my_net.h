#ifndef __NET_H
#define __NET_H
//--------------------------------------------------
#include "stm32g4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "my_w5500.h"
//--------------------------------------------------
#define IP_ADDR {192,168,0,197}
#define IP_GATE {192,168,0,197}
#define IP_MASK {255,255,255,0}
#define LOCAL_PORT 80
//--------------------------------------------------
#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
//--------------------------------------------------
#endif /* __NET_H */