/*
 * lib_xbee_x86.c
 *
 *  Created on: 28 mai 2013
 *      Author: quentin
 */

#include "lib_xbee_x86.h"
#include "messages.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>

int Xbee_serial_port;


/* Initialize the serial port deisgnated by devStr
 * Arguments :
 *  devStr : string containing the location of the device/file
 *
 */
void Xbee_initSerial(char * devStr){
    struct termios options;

    printf("opening of:%s:\n",devStr);
    Xbee_serial_port = open(devStr, O_RDWR | O_NOCTTY | O_NDELAY);//lecture et ecriture | pas controlling terminal | ne pas attendre DCD

    //cas d'erreur d'ouverture
    if(Xbee_serial_port < 0)
    {
        perror("Erreur d'ouverture du port serie");
        exit(-1);
    }

    printf("Port serie numero %d bien ouvert.\n", Xbee_serial_port);

    //chargement des données
    tcgetattr(Xbee_serial_port, &options);
    //B115200 bauds
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);//programme propriétaire du port
    //structure en 8N1 !!
    options.c_cflag &= ~PARENB; //pas de parité
    options.c_cflag &= ~CSTOPB; // 1 bit de stop
    options.c_cflag &= ~CSIZE; //option a 0
    options.c_cflag |= CS8; //8 bits
    tcsetattr(Xbee_serial_port, TCSANOW, &options); //enregistrement des valeurs de configuration
    printf("Port serie ouvert\n");

    fcntl(Xbee_serial_port,F_SETFL,10);//mode bloquant pour la fonction read() si aucun caractere dispo, programme attend

}


/* closes the serial port
 * Arguments : none
 *
 */
void Xbee_deInitSerial(){
    close(Xbee_serial_port);
}


#define CBUFF_SIZE 8        //MUST be a power of 2
#define MAX_READ_BYTES 100  //nb of bytes to read before we leave this function (to avoid blocking)

int Xbee_receive(sMsg *pRet){
    static uint8_t i;
    static uint8_t smallBuf[CBUFF_SIZE]={0};
    int count=0;
    int j;
    //read(...)=0 <=> no data available, <0 <=> error | count to limit the time spend in the loop in case of spam, checksum to get out of the loop if it is correct AND the sender address id OK (if sender=0 it means it has been reset to 0 after reading the message)
    while( read(Xbee_serial_port,&(smallBuf[i]),1)>0 \
            && count<=MAX_READ_BYTES \
            &&  ( cbChecksumHead(smallBuf,CBUFF_SIZE,(i-1)&(CBUFF_SIZE-1)) || !( smallBuf[(i-5)&(CBUFF_SIZE-1)]<<8&smallBuf[(i-4)&(CBUFF_SIZE-1)] ) ) ) {
        i=(i+1)&(CBUFF_SIZE-1);                                          // &7 <~> %8, but better behaviour with negative in our case (and MUCH faster)
        count++;
    }


    if (count<=MAX_READ_BYTES && cbChecksumHead(smallBuf,CBUFF_SIZE,(i-1)&(CBUFF_SIZE-1)) && smallBuf[(i-3)&3] ){

        count=sizeof(sGenericHeader);

        //TODO : do we copy the message before checking payload checksum, or after (in that case, we must copy it one more time in memory if we don't want to corrupt any previous message stored at the location pointed by return pointer) ?

        //we copy the header in the return structure
        for (j=0;j<sizeof(sGenericHeader);j++){
            ((uint8_t *)(&(pRet->header)))[j]=smallBuf[(i-sizeof(sGenericHeader)+j)&(CBUFF_SIZE-1)];
        }

        //we read the rest of the data in this message (given by the "size" field of the header) and write the in the return structure
        count+=read(Xbee_serial_port,(char *)(&(pRet->payload)),pRet->header.size);//Serial.readBytes((char *)(&(pRet->payload)),pRet->header.size);

        memset(smallBuf,0,sizeof(smallBuf));
        return count;
    }

    return 0;
}

int Xbee_send(sMsg msg){

return 0;
}