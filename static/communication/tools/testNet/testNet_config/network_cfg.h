/*
 * network_cfg.h
 *
 *  Created on: 28 mai 2013
 *      Author: quentin
 */

#ifndef NETWORK_CFG_H_
#define NETWORK_CFG_H_


#include "../../../botNet/shared/message_header.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef BIT
#define BIT(a) (1<<(a))
#endif



#define DEVICE_ADDR_SIZE 8      //in bits, on a 16 bits adress. Must equal the larger size of the adresse in the different subnetworks

//masks
    #define SUBNET_MASK (0xff<<DEVICE_ADDR_SIZE)  //on a 16-bits adress
	#define DEVICEX_MASK ( BIT(DEVICE_ADDR_SIZE)-1 )
	#define DEVICEI_MASK ( BIT(DEVICE_ADDR_SIZE)-1 )
    #define ADDRI_MASK  (0xff)      //on a 16-bits adress, xbee devices
    #define ADDRX_MASK  (0xff)      //on a 16-bits adress, i2c devices

//subnet adresses
    #define SUBNETX         (1<<DEVICE_ADDR_SIZE)
    #define SUBNETI         (2<<DEVICE_ADDR_SIZE)
    #define SUBNETU1        (3<<DEVICE_ADDR_SIZE)
    #define SUBNETU2        (4<<DEVICE_ADDR_SIZE)


//xbee adresses
    #define ADDRX_PC1       ( BIT(0) | SUBNETX )
    #define ADDRX_BEACON_1  ( BIT(2) | SUBNETX )
    #define ADDRX_BEACON_2  ( BIT(3) | SUBNETX )

//I2C adresses
//!\ NO ODD ADDRESSES FOR I²C !!!!
    #define ADDRI_ARDUINO       (2 | SUBNETI_MAIN)
    #define ADDRI_BEACON_1      (4 | SUBNETI_MAIN)
    #define ADDRI_LPC           (6 | SUBNETI_MAIN)
//UART adresses
    #define ADDRU_PC2       (1 | SUBNETU1)
    #define ADDRU_ARDUINO   (2 | SUBNETU1)
    #define ADDRU_LPC       (1 | SUBNETU2)
    #define ADDRU_PC3       (2 | SUBNETU2)

//default debug address :
    #define ADDR_DEBUG_DFLT 0



/* Interface enum
 *
 */
typedef enum{
	IF_XBEE,
	IF_I2C,
	IF_UART,
	IF_LOCAL, 	//virtal interface, describing local node. A message send to "self" should be popped out and "given" to the node trought the sb_receive() api
	IF_DROP,	//virtual interface, equivalent to /dev/null in linux


	IF_COUNT
}E_IFACE;

/* routing table entry
 * the routing of a message "msg", with destination address "destAddr", coming from the interface "interface" is the following :
 * if (the destination is on one of our subnetworks){
 * 		if (the message has not already been red on the latter subnet){
 * 			send message on the appropriate subnet;
 * 			return;
 * 			}
 * 		}
 *
 * while(we are not at the end of the routing table){
 *      if (destination's subnet==routing table entry subnet){
 *      	send to ifTo;
 *      	return
 *      }
 *      go to next routing table entry
 * }
 * perform default action (send to default)
 *
 * /!\ the last entry MUST be {0x42&(~SUBNET_MASK),default interface,default address}
 */

typedef struct {
    E_IFACE ifTo;
    bn_Address nextHop;
}sRouteInfo;

typedef struct{
    bn_Address destSubnet;
    sRouteInfo nextHop;
}sRTableEntry;




extern sRTableEntry rTable[];

#ifdef __cplusplus
}
#endif


#endif /* NETWORK_CFG_H_ */
