/*
 * Xbee_API.h
 *
 *  Created on: 14 juin 2013
 *      Author: quentin
 *
 *      This library was created to use XBee devices with API mode 2 (with escaped characters).
 *      Though some libraries already exists for Arduino, they uses a lot of C++ functionalities and
 *      Arduino-specific funtions.
 *      In this one, I tried to be as architecture-independant as possible, and I used only C language.
 *      In consequence, this library can be used on any device, which can be programmed in C (microcontroller,
 *      PC...) and owns a serial (UART) link.
 *
 *      Because of this portability, there are a few things to do if you want to use it :
 *          1)  Your project must have a "node_cfg.h" file, visible from Xbee_API.h, which contains :
 *              #define ARCH_XXX                           // According to the #ifdef at the beginning of XBee_API.c
 *              #define XBEE_WAITFRAME_TIMEOUT  1000000    // in microsecond
 *              #define XBEE_READBYTE_TIMEOUT   5000       // in microsecond
 *          2)  You will have to modify "Xbee_API.c" to match your files. The simplest is to add :
 *                  "
 *                  #ifdef ARCH_XXX                             // the one of param.h
 *                  #include "drivers/Xbee_API_XXX_drivers.h"   //the one you juste wrote
 *                  "
 *                  Then replace the first #ifdef ARCH_YYY of the previous list by #elif defined(ARCH_YYY)
 */


/*
 * /!\ important remarks :
 *  unless otherwise noted, variable endianess is unspecified. Special cases :
 *      - "_be" suffix means "big-endian" for any value which should be understood by the Xbee (e.g.
 *        parameters of AT cmd parameters) or which have been generated by the Xbee.
 *      - "_be" does not apply on higher-level data (e.g. for TX or RX payload)
 *      -  "_h" means Host endianess
 *
 *  Structures describing API-specific structures MUST be packed and comply with byte order as specified
 *  in DIGI's documentation. In particular, the order of members of a structure is important (hence the pack)
 */
#ifndef XBEE_API_H_
#define XBEE_API_H_

#ifdef __cplusplus
extern "C" {
#endif

// config files
#include "node_cfg.h"

// other required libraries

// XBEE specific libraries
#ifdef ARCH_328P_ARDUINO
#include "../arduino/Xbee_API_arduino_drivers.h"
#elif defined(ARCH_X86_LINUX)
#include <stdio.h>
#elif defined(ARCH_LM4FXX)
#include "../lm4fxx/Xbee_API_lm4fxx_drivers.h"
#else
#error will not compile, check architecture define and driver library
#endif

// standard libraries
#include <stdint.h>






/////////////// Macros
#ifndef BIT
#define BIT(a) (1<<(a))
#endif

/////////////// Xbee API specific types

typedef uint16_t XbeeAddr16_t;  // 16-bit addresses
typedef uint64_t XbeeAddr64_t;  // 64-bit addresses

typedef struct __attribute__((__packed__)){
    uint8_t frameID;            // for acknowledgment purposes
    uint8_t status;
}spTXStatus;

typedef struct __attribute__((__packed__)){
    uint8_t frameID;
    uint16_t lDstAddr_be;          // LINK (layer 2) destination address
    uint8_t options;
    uint8_t pPayload[100];                               //optimization?  const uint8_t *pPayload;
}spTX16Data;

typedef struct __attribute__((__packed__)){
    uint16_t lSrcAddr_be;          // LINK (layer 2) source address
    uint8_t rssi;
    uint8_t options;
    uint8_t payload[100];
}spRX16Data;

typedef struct __attribute__((__packed__)){
    uint8_t     frameID;            // for acknowledgment purposes
    uint8_t     cmd[2];             // ascii big endian command. ex : for "DL", we MUST have cmb_be = ('D'<<8) | 'L'
    uint32_t    parameter_be;    // hexadecimal parameter value (if any) of the AT cmd. Value must be written big-endian.
}spATCmd;

typedef struct __attribute__((__packed__)){
    uint8_t     frameID;            // for acknowledgment purposes
    uint8_t     cmd[2];             // ascii Command. ex : for "DL", we MUST have cmb_be = ( 'D'<<8 ) | 'L'
    uint8_t     status;
    uint8_t     value_be[16];        // hex value returned (if any) by the AT cmd. Value will be written big-endian.
}spATResponse;

typedef union __attribute__((__packed__)){
    spTX16Data TX16Data;
    spTXStatus TXStatus;
    spRX16Data RX16Data;
    uint8_t modemStatus;
    spATResponse ATResponse;
    spATCmd ATCmd;
    uint8_t raw[104];
}upAPISpecificData;

typedef struct __attribute__((__packed__)){
    uint8_t APID;           // API identifier
    upAPISpecificData data;
}spAPISpecificStruct;


/////////////// Xbee specific defines and flags

    //// TX option flags
    #define XBEE_TX_O_NOACK     0x01
    #define XBEE_TX_O_BCAST     0x04    // use alone (without other flags)
    //// TX status
    #define XBEE_TX_S_SUCCESS   0
    #define XBEE_TX_S_NOACK     1       // no ack received after all retries (ALWAYS the case if broadcast)
    #define XBEE_TX_S_CCAFAIL   2       // Clear Channel assessment fail
    #define XBEE_TX_S_PURGED    3       // Coordinator times out of an indirect transmission

    //// RX option flags
    #define XBEE_RX_O_ADDRBCAST 0x02
    #define XBEE_RX_O_PANBCAST  0x04

    //// Modem status
    #define XBEE_MODEM_S_HARDRST        0   // Hardware reset
    #define XBEE_MODEM_S_WATCHDOGRST    1   // Wtachdog timer reset
    #define XBEE_MODEM_S_ASSOC          2   // Associated
    #define XBEE_MODEM_S_DISASSOC       3   // Disassociated
    #define XBEE_MODEM_S_SYNCLOST       4   // Synchronization Lost (Beacon -enabled only)
    #define XBEE_MODEM_S_COORDREAL      5   // Coordinator realignment
    #define XBEE_MODEM_S_COORDSTART     6   // Coordinator started

    //// AT cmd options
    #define XBEE_ATCMD_GET      0
    #define XBEE_ATCMD_SET      1
    //// AT response status
    #define XBEE_ATR_S_OK       0
    #define XBEE_ATR_S_ERROR    1
    #define XBEE_ATR_S_INVCOM   2    // Invalid command
    #define XBEE_ATR_S_INVPAR   3    // Invalid parameter


    //// API Identifiers
    #define XBEE_APID_MODEM_S           0x8A    // Modem status
    #define XBEE_APID_ATCMD             0x08    // AT command immediately applied
    #define XBEE_APID_ATCMD_QUEUE       0x09    // AT command applied after "AC" command only   XXX not implemented
    #define XBEE_APID_ATRESPONSE        0x88    // response to an AT cmd
    #define XBEE_APID_REMOTE_ATCMD      0x17    // remote AT command                            XXX not implemented
    #define XBEE_APID_REMOTE_ETRESPONSE 0x97    // response to remote AT command                XXX not implemented
    #define XBEE_APID_TX64              0x00    // 64 bits address TX request                   XXX not implemented
    #define XBEE_APID_TX16              0x01    // 16 bits address TX request
    #define XBEE_APID_TXS               0x89    // TX status 16
    #define XBEE_APID_RX64              0x80    // 64 bits address RX                           XXX not implemented
    #define XBEE_APID_RX16              0x81    // 16 bits address RX
    #define XBEE_APID_RXIO64            0x82    // 64 bits address IO RX                        XXX not implemented
    #define XBEE_APID_RXIO16            0x83    // 16 bits address IO RX                        XXX not implemented



int Xbee_init();
int Xbee_Tx16(XbeeAddr16_t to_h,uint8_t options, uint8_t frameID, const void* data, uint16_t datasize_h);
int Xbee_ATCmd(char cmd[2],uint8_t frameID, uint8_t option, uint32_t parameter);
int Xbee_writeFrame(const spAPISpecificStruct *str_be, uint16_t size);
int Xbee_readFrame(spAPISpecificStruct *str);
int Xbee_waitATAck(int frID, uint32_t timeOut);


/*
 * Swaps from Big-endian to host endianess or from host to big
 * Only for big and little endian
 */
static inline uint16_t hbe2_swap(uint16_t bytes){
    #ifdef ARCH_LITTLE_ENDIAN
        return (bytes>>8 | bytes <<8);
    #else
        return bytes;
    #endif
}

/*
 * Swaps from Big-endian to host endianess or from host to big
 * Only for big and little endian
 */
static inline uint32_t hbe4_swap(uint32_t bytes){
    #ifdef ARCH_LITTLE_ENDIAN
        return ( (bytes >> 24) | ((bytes & 0x00ff0000) >> 8) | ((bytes & 0x0000ff00) <<8) | (bytes << 24) );
    #else
        return bytes;
    #endif
}

#ifdef __cplusplus
}
#endif

#endif /* XBEE_API_H_ */
