/*
 * lib_UART_framing_linux.h
 *
 *  Created on: 2 nov. 2013
 *      Author: quentin
 */

#ifdef ARCH_X86_LINUX

#ifndef LIB_UART_FRAMING_LINUX_H_
#define LIB_UART_FRAMING_LINUX_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum eblah{
    E_115200_8N2,   //useful for xbee @ 111111 bd
    E_115200_8N1
};

int serialInit(const char *device, uint32_t mode);
int serialDeinit();
int serialRead(uint8_t *byte,uint32_t timeout);
int serialWrite(uint8_t byte);

//#define DEBUG_PRINT_HEX

#ifdef __cplusplus
}
#endif

#endif /* LIB_UART_FRAMING_LINUX_H_ */


#endif // ARC_X86_LINUX
