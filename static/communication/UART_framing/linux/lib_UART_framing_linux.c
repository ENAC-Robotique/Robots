/*
 * lib_UART_framing_linux.c
 *
 *  Created on: 2 nov. 2013
 *      Author: quentin
 */


#ifdef ARCH_X86_LINUX

#include "lib_UART_framing_linux.h"
#include "global_errors.h"

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/select.h>


int serial_port;

/* initSerial : Initializes the UART serial interface (opens the serial interface at 115200 bauds nonblocking, in a way that pleases the XBee)
 * Argument :
 *  device : string describing the device (/dev/ttyYYY)
 *  mode : E_115200_8N2, E_115200_8N1
 * Return value :
 *  0 on success
 *  <0 (or terminate program) on error
 */
int serialInit(const char *device, uint32_t mode){
    struct termios options;
    int ret;

    printf("opening of:%s: at 115200 bd\n",device);
    serial_port = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);//lecture et ecriture | pas controlling terminal | ne pas attendre DCD

    //cas d'erreur d'ouverture
    if(serial_port < 0)
    {
        perror("open() @ serialInit()");
        exit(-1);       //XXX terminate program abruptly here or gently tell the calling function (main) that a problem occurred ?
    }

    //reading config data from serial port
    ret = tcgetattr(serial_port, &options);
    if(ret < 0){
        perror("tcgetattr() @ serialInit()");
        return -ERR_SYSERRNO;
    }

    options.c_iflag = IGNBRK; // ignore break
    options.c_oflag = 0;
    options.c_cflag = CLOCAL /* ignore modem ctrl */ | CREAD /* enable receiver */ | CS8 /* 8 bits per character */;
    ret = cfsetospeed(&options, B115200 /* 115200bauds */);
    if(ret < 0){
        perror("cfsetospeed() @ serialInit()");
        return -ERR_SYSERRNO;
    }
    if (mode==E_115200_8N2) {
        printf("115200 8N2 set\n");
        options.c_cflag |= CSTOPB; // 2 stop bits (hack for XBee)
    }
    else if (mode==E_115200_8N1) {
        printf("115200 8N1 set\n");
//        options.c_cflag &= ~CSTOPB; //1 stop bit
    }
    options.c_lflag = 0;

    ret = tcsetattr(serial_port, TCSANOW, &options); //enregistrement des valeurs de configuration
    if(ret < 0){
        perror("tcsetattr() @ serialInit()");
        return -ERR_SYSERRNO;
    }
    printf("Port serie ouvert\n");

    ret = fcntl(serial_port,F_SETFL,O_NONBLOCK);//mode non bloquant pour la fonction read() si aucun caractere dispo, programme attend
    if(ret < 0){
        perror("fcntl() @ serialInit()");
        return -ERR_SYSERRNO;
    }

    return 0;
}

/* deinitSerial : Closes the UART serial interface
 * Argument :
 *  none
 * Return value :
 *  0 on success
 *  <0 on error
 */
int serialDeinit(){
    close(serial_port);
    return 0;
}

/* serialRead : reads one byte from serial interface
 * Argument :
 *  byte : pointer to where the byte should be stored
 *  timeout : in µs, time after which the function returns 0 if no byte can be read.
 * Return value :
 *  1 if one byte have been read
 *  0 if timeout (nothing to read, not an error)
 *  <0 on error
 *
 */
int serialRead(uint8_t *byte,uint32_t timeout){
    int i;

    // some variables and struct to use select
    fd_set rfds;
    struct timeval tv;
    int retval;

    // Watch Xbee to see when it has input
    FD_ZERO(&rfds);
    FD_SET(serial_port, &rfds);

    // Wait up to timeout microseconds.
    tv.tv_sec = timeout/1000000;
    tv.tv_usec = timeout%1000000;

    // select() wait
    retval = select(serial_port+1, &rfds, NULL, NULL, &tv);
    if (retval == -1){
        perror("select() @ serialRead()");
        return -ERR_SYSERRNO;
    }
    else if (retval>0){
       // Data is available now, we can read
        i=read(serial_port,byte,1);
        if (i<0) {
            if (errno==EAGAIN || errno==EWOULDBLOCK) return 0;

            perror("read() @ serialRead()");
            return -ERR_SYSERRNO;
        }
#ifdef DEBUG_PRINT_HEX
        else if (i) {
            printf("r%x ",*byte);
            fflush(stdout);
        }
#endif
        return i;
   }
   else return 0;
}

/* serialWrite : writes a byte on the serial interface
 * Argument :
 *  byte : byte to write
 * Return value :
 *  1 on success
 *  <=0 on error
 */
int serialWrite(uint8_t byte){
#ifdef DEBUG_PRINT_HEX
    printf("w%x ",byte);
#endif
    int ret;
    ret=write(serial_port, &byte, 1);
    if (ret<0) {
        perror("write() @ serialWrite");
        return -ERR_SYSERRNO;
    }
    fflush(NULL); //flushes all stream
    return ret;
}

//int serialReadBytes(uint8_t *bytes, uint32_t size, uint32_t timeout){
//    int ret, try=3;
//    fd_set rs;
//    struct timeval tv;
//
//    FD_ZERO(&rs);
//    FD_SET(serial_port, &rs);
//    tv.tv_sec = timeout/1000000;
//    tv.tv_usec = timeout%1000000;
//
//    ret = select(serial_port+1, &rs, NULL, NULL, &tv);
//    if(ret < 0){
//        perror("select() @ serialReadBytes");
//        return -ERR_SYSERRNO;
//    }
//    else if(!ret){
//        return 0;
//    }
//
//    do{
//        ret = read(serial_port, bytes, size);
//    }while(ret < 0 && (errno == EAGAIN || errno == EWOULDBLOCK) && try-- > 0);
//    if (ret < 0) {
//        perror("write() @ serialReadBytes");
//        return -ERR_SYSERRNO;
//    }
//    return ret;
//}

int serialWriteBytes(uint8_t *bytes, uint8_t size){
    int ret, try=3;
    fd_set ws;
    struct timeval tv;

    FD_ZERO(&ws);
    FD_SET(serial_port, &ws);
    tv.tv_sec = 0;
    tv.tv_usec = 500;

    ret = select(serial_port+1, NULL, &ws, NULL, &tv);
    if(ret < 0){
        perror("select() @ serialWriteBytes");
        return -ERR_SYSERRNO;
    }
    else if(!ret){
        return 0;
    }

    do{
        ret = write(serial_port, bytes, size);
    }while(ret < 0 && (errno == EAGAIN || errno == EWOULDBLOCK) && try-- > 0);
    if(ret < 0) {
        perror("write() @ serialWriteBytes");
        return -ERR_SYSERRNO;
    }
    return ret;
}

#endif // ARCH_X86_LINUX
