/*
 * lib_I2C_arduino.h
 *
 *  Created on: 6 juin 2013
 *      Author: quentin
 */

#ifdef ARCH_328P_ARDUINO

#ifndef LIB_I2C_ARDUINO_H_
#define LIB_I2C_ARDUINO_H_

#ifdef __cplusplus
extern "C" { //to enable use in both C projects an C++ projects
#endif

void I2C_init(unsigned long speed);
int I2C_receive(sMsg *pRet);
int I2C_send(const sMsg *msg,bn_Address firstDest);

#ifdef __cplusplus
}
#endif

#endif /* LIB_I2C_ARDUINO_H_ */

#endif // ARCH_328P_ARDUINO
