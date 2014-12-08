/*
 * network_cfg.c
 *
 *  Created on: 28 mai 2013
 *      Author: quentin
 */

#include "network_cfg.h"
#include "node_cfg.h"

/*Remark : requires a "node.cfg.h" in the project, containing :
 * MYADDRX : address of the xbee interface of the node
 * MYADDRI : address of the i2c interface of the node
 * MYADDRU : address of the uart interface of the node
 * MYADDRD : address of the UDP interface of the node
 */

// addresses check
#if MYADDRX && ((MYADDRX & SUBNET_MASK) != SUBNETX)
#error "MYADDRX not on xBee subnet"
#endif
#if MYADDRI && ((MYADDRI & SUBNET_MASK) != SUBNETI_MAIN)
#error "MYADDRI not on I²C subnet"
#endif
#if MYADDRU && ((MYADDRU & SUBNET_MASK) != SUBNETU1_DEBUG) && ((MYADDRU & SUBNET_MASK) != SUBNETU2_MAIN)
#error "MYADDRU not on UART subnet"
#endif
#if MYADDRD && ((MYADDRD & SUBNET_MASK) != SUBNETD1_DEBUG) && ((MYADDRD & SUBNET_MASK) != SUBNETD2_MAIN)
#error "MYADDRD not on UDP subnet"
#endif

#if MYADDRI & 1
#error "I²C address is odd, do you know what you do?"
#endif

#if (MYADDRX == ADDRX_MAIN_TURRET || MYADDRI == ADDRI_MAIN_TURRET)
// project "balise_principal"
sRTableEntry rTable[]={
    {SUBNETU1_DEBUG, {IF_I2C,  ADDRI_MAIN_IO}},
    {SUBNETU2_MAIN,  {IF_I2C,  ADDRI_MAIN_PROP}},
    {SUBNETD1_DEBUG, {IF_I2C,  ADDRI_MAIN_IO}},
    {SUBNETD2_MAIN,  {IF_I2C,  ADDRI_MAIN_PROP}},
    {0x42&(~SUBNET_MASK),{IF_DROP,0}}
};
#elif (MYADDRI == ADDRI_MAIN_IO || MYADDRU == ADDRU1_MAIN_IO)
// project "arduino_io"
sRTableEntry rTable[]={
    {SUBNETX,        {IF_I2C,  ADDRI_MAIN_TURRET}},
    {SUBNETU2_MAIN,  {IF_I2C,  ADDRI_MAIN_PROP}},
    {SUBNETD1_DEBUG, {IF_UART, ADDRU1_DBGBRIDGE}},
    {SUBNETD2_MAIN,  {IF_I2C,  ADDRI_MAIN_PROP}},
    {0x42&(~SUBNET_MASK),{IF_DROP,0}}
};
#elif (MYADDRI == ADDRI_MAIN_PROP || MYADDRU == ADDRU2_MAIN_PROP)
// project "main_prop_axle/lpc"
sRTableEntry rTable[]={
    {SUBNETX,        {IF_I2C,  ADDRI_MAIN_TURRET}},
    {SUBNETU1_DEBUG, {IF_I2C,  ADDRI_MAIN_IO}},
    {SUBNETD1_DEBUG, {IF_I2C,  ADDRI_MAIN_IO}},
    {SUBNETD2_MAIN,  {IF_UART, ADDRU2_MAIN_AI}},
    {0x42&(~SUBNET_MASK),{IF_DROP,0}}
};
#elif (MYADDRX == ADDRX_MOBILE_1 || MYADDRX == ADDRX_MOBILE_2 || MYADDRX == ADDRX_DEBUG || MYADDRX == ADDRX_FIX)
// project "balise_mobile" OR debug node
sRTableEntry rTable[]={
    {SUBNETI_MAIN,   {IF_XBEE, ADDRX_MAIN_TURRET}},
    {SUBNETU1_DEBUG, {IF_XBEE, ADDRX_MAIN_TURRET}},
    {SUBNETU2_MAIN,  {IF_XBEE, ADDRX_MAIN_TURRET}},
    {SUBNETD1_DEBUG, {IF_XBEE, ADDRX_MAIN_TURRET}},
    {SUBNETD2_MAIN,  {IF_XBEE, ADDRX_MAIN_TURRET}},
    {0x42&(~SUBNET_MASK),{IF_DROP,0}}
};
#elif (MYADDRD == ADDRD1_DEBUG1 || MYADDRD == ADDRD1_DEBUG2  || MYADDRD == ADDRD1_DEBUG3 || MYADDRD == ADDRD1_MAIN_PROP_SIMU || MYADDRD == ADDRD1_MAIN_IA_SIMU || MYADDRD == ADDRD1_MONITORING)
// project "bn_debug_console" OR project "main_prop_axle/linux" OR project "main_ai" OR project "monitoring_hmi/*"
sRTableEntry rTable[]={
    {SUBNETX,        {IF_UDP,  ADDRD1_DBGBRIDGE}},
    {SUBNETI_MAIN,   {IF_UDP,  ADDRD1_DBGBRIDGE}},
    {SUBNETU1_DEBUG, {IF_UDP,  ADDRD1_DBGBRIDGE}},
    {SUBNETU2_MAIN,  {IF_UDP,  ADDRD1_DBGBRIDGE}},
    {SUBNETD2_MAIN,  {IF_UDP,  ADDRD1_DBGBRIDGE}},
    {0x42&(~SUBNET_MASK),{IF_DROP,0}}
};
#elif (MYADDRU == ADDRU2_MAIN_AI || MYADDRD == ADDRD2_MAIN_AI)
// project "main_ai"
sRTableEntry rTable[]={
    {SUBNETX,        {IF_UART, ADDRU2_MAIN_PROP}},
    {SUBNETI_MAIN,   {IF_UART, ADDRU2_MAIN_PROP}},
    {SUBNETU1_DEBUG, {IF_UART, ADDRU2_MAIN_PROP}},
    {SUBNETD1_DEBUG, {IF_UART, ADDRU2_MAIN_PROP}},
    {0x42&(~SUBNET_MASK),{IF_DROP,0}}
};
#elif (MYADDRU == ADDRU1_DBGBRIDGE || MYADDRD == ADDRD1_DBGBRIDGE)
// project "debug_bridge"
sRTableEntry rTable[]={
    {SUBNETX,        {IF_UART, ADDRU1_MAIN_IO}},
    {SUBNETI_MAIN,   {IF_UART, ADDRU1_MAIN_IO}},
    {SUBNETU2_MAIN,  {IF_UART, ADDRU1_MAIN_IO}},
    {SUBNETD2_MAIN,  {IF_UART, ADDRU1_MAIN_IO}},
    {0x42&(~SUBNET_MASK),{IF_DROP,0}}
};
#endif
