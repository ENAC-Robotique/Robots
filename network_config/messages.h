/*
 * messages.h
 *
 *  Created on: 4 mai 2013
 *      Author: quentin
 *
 *
 *      Remark concerning this file :
 *      The user MUST ONLY modify the content between the matching commented lines *** user XXXX start *** and *** user XXXX stop ***
 *      He/she MUST NOT modify anything outside these markers
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "network_cfg.h"
#include "message_header.h"

// sb_Address : cf SUBNET_MASK and ADDRxx_MASK in network_cfg.h

#define SB_MAX_PDU 64 //max size of a message, including header AND payload.

//message types
typedef enum{
    E_DEBUG,                // general debug
    E_DEBUG_SIGNALLING,     // debug receiver signalling
    E_DATA,                 // arbitrary data payload
    E_ACK_RESPONSE,                  // ack response

/************************ user types start ************************/
    E_SWITCH_CHANNEL,       // switch channel message
    E_SYNC_EXPECTED_TIME,   // sync expected time (send from the turret to the receiver)
    E_SYNC_OK,              // synced
    E_PERIOD,               // period measurement
    E_MEASURE,              // laser delta-time measurement
    E_TRAJ,                 // a trajectory step
    E_POS,                  // position (w/ uncertainty) of an element
    E_SERIAL_DUMP,          // serial dump (for debug)
/************************ user types stop ************************/

    E_TYPE_COUNT            // This one MUST be the last element of the enum
}E_TYPE;

/************************ !!! user action required start ************************/
/* WARNING : SB_TYPE_VERSION describes the version of the above enum.
 It is managed "by hand" by the user. It MUST be incremented (modulo 16) only when the order of the *already existing* elements
 of this enum is modified.
 Examples :
     New type AFTER the last one (but before E_TYPE_COUNT) : do not increment SB_TYPE_VERSION
     New type between 2 previously exinsting types : increment SB_TYPE_VERSION
     Cleaning (removing) unused type : increment SB_TYPE_VERSION
     Changing the order of the types : increment SB_TYPE_VERSION
 The recommended use is the following :
     Add every new type at the end of the list (but before E_TYPE_COUNT), do not delete old ones.
     Every month (or so, when it is needed) : remove unused types, sort logically the other types and increse SB_TYPE_VERSION.
 */
#define SB_TYPE_VERSION 0       //last modifed : 2013/10/31
/************************ user action required stop ************************/

//function returning a string corresponding to one element of the above enum. Must be managed by hand.
char *eType2str(E_TYPE elem);


/************************ user payload definition start ************************/
//user-defined payload types.
//for simple payloads ( single variable or array), this step can be skipped
//Warning : the user has to make sure that these payloads are not too big (cf SB_MAX_PDU)

typedef struct __attribute__((__packed__)){
    uint32_t value;        //laser measured delta-time
    uint32_t date;           //laser sensing time
    uint16_t precision;      //precision of the measure
    uint16_t sureness;       //sureness of the mesure
} sMesPayload;


typedef struct {
// segment
  float p1_x;
  float p1_y;
  float p2_x;
  float p2_y;
  float seg_len;
// circle
  float c_x;
  float c_y;
  float c_r;
  float arc_len;
// trajectory data
  uint16_t tid; // trajectory identifier
  uint16_t sid; // step identifier
} sTrajElRaw_t;

typedef struct {
// position in game area frame
    float x; // (cm)
    float y; // (cm)
    float theta; // (rad)
// uncertainty (oriented rectangle)
    float u_a_theta; // (rad)
    float u_a; // (cm)
    float u_b; // (cm)
// identifier of the robot/element
    uint8_t id; // 0:prim, 1:sec, 2:adv_prim, 3:adv_sec
} sPosPayload;

/************************ user payload definition stop ************************/




/*
 * union defining the different possible payloads
 */
typedef union{
    uint8_t raw[SB_MAX_PDU-sizeof(sGenericHeader)];		//only used to access data/data modification in low layer
    uint8_t data[SB_MAX_PDU-sizeof(sGenericHeader)];	//arbitrary data, actual size given by the "size" field of the header
    uint8_t debug[SB_MAX_PDU-sizeof(sGenericHeader)];   //debug string, actual size given by the "size" field of the header
    sAckPayload ack;

/************************ user payload start ************************/
//the user-defined payloads from above must be added here. The simple ones can be directly added here
//Warning : the user has to make sure that these payloads are not too big (cf SB_MAX_PDU)
    uint32_t syncTime;
    uint8_t channel;
    uint32_t period;
    sMesPayload measure;
    sTrajElRaw_t traj;
    sPosPayload pos;
/************************ user payload stop ************************/

}uPayload;


//final message structure
typedef struct{
    sGenericHeader header;
    uPayload payload;
}sMsg;

#ifdef __cplusplus
}
#endif

#endif /* MESSAGES_H_ */
