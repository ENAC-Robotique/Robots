/*
 * node_cfg.h
 */

#ifndef NODE_CFG_H_
#define NODE_CFG_H_

#include "network_cfg.h"

//network config
#define MYADDRX 0
#define MYADDRI ADDRI_MAIN_IO
#define MYADDRU ADDRU1_MAIN_IO
#define MYADDRD 0
#define MYADDR (MYADDRX?:MYADDRI?:MYADDRU?:MYADDRD)

#define MYROLE 0
// MYROLE must be equal to role_get_role(MYADDR)

#define BN_INC_MSG_BUF_SIZE 4
#define BN_WAIT_XBEE_SND_FAIL   25000
#define BN_MAX_RETRIES          2
#define BN_ACK_TIMEOUT          1000    //in ms

#define UART_WAITFRAME_TIMEOUT 10
#define UART_READBYTE_TIMEOUT 10000

#define XBEE_RST_PIN 5

#endif /* NODE_CFG_H_ */
