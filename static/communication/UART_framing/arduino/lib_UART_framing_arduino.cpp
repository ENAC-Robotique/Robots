/*
 * lib_UART_framing_arduino.cpp
 *
 *  Created on: 1 nov. 2013
 *      Author: quentin
 */

#include "lib_UART_framing_arduino.h"
#include "Arduino.h"


/* initSerial : Initialises the UART serial interface
 * Argument :
 *  speed : speed
 * Return value :
 *  >0 on success
 *  <0 on error
 */
int serialInit(uint32_t speed){
    Serial.begin(speed);
    Serial.setTimeout(0); // immediate and non-blocking read, custom tempo done in serialRead.
    return 1;
}

/* deinitSerial : Closes the UART serial interface
 * Argument :
 *  none
 * Return value :
 *  >0 on success
 *  <0 on error
 */
int serialDeinit(){
    Serial.end();
    return 1;
}

/* serialRead : reads one byte from serial interface
 * Argument :
 *  byte : pointer to where the byte should be stored
 *  timeout : in µs, time after which the function returns 0 if no byte can be read.
 * Return value :
 *  1 if one byte have been read
 *  0 if timeout
 *  -1 on error
 *
 */
int serialRead(uint8_t *byte,uint32_t timeout){
    unsigned long int sw=micros();

    if (timeout){
        while ( (micros()-sw) <= timeout ) {
            if (Serial.available()) {
                *byte = Serial.read();
                return 1;
            }
        }
    }

    else {
        if (Serial.available()) {
            *byte = Serial.read();
            return 1;
        }
    }

    return 0;
}

/* serialWrite : writes a byte on the serial interface
 * Argument :
 *  byte : byte to write
 * Return value :
 *  1 on success
 *  -1 on error
 */
int serialWrite(uint8_t byte){
    if (Serial.write(byte)) return 1;
    return -1;
}
