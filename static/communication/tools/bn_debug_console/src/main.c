/*
 * main.c
 *
 *  Created on: 10 oct. 2013
 *      Author: quentin
 */
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>

#include "../botNet/shared/botNet_core.h"
#include "../botNet/shared/bn_debug.h"
#include "../botNet/shared/bn_utils.h"
#include "node_cfg.h"


static int menu = 0;

void intHandler(int dummy) {
    menu = 1;
}

int main(){
    volatile int quit=0,quitMenu=0,err;
    sMsg msgIn;
    char cmd;
    bn_Address destAd;

    bn_init();

    printf("listening, CTRL+C  for menu\n");

    signal(SIGINT, intHandler);

    //main loop
    while (!quit){
        int nbTraces,f; //for traceroute display

        usleep(500);

        bn_routine();

        //receives messages, displays string if message is a debug message
        if (bn_receive(&msgIn)){
            printf("message received from %hx, type : %u %s\n",msgIn.header.srcAddr,msgIn.header.type,eType2str(msgIn.header.type));
            switch (msgIn.header.type){
            case E_DEBUG : printf("%s\n",msgIn.payload.debug); break;
            default : break;
            }
        }

        //menu
        if (menu){
            menu=0;
            quitMenu=0;
            while (!quitMenu){
                cmd=0;
                printf("\ndebug reader menu\n");
                printf("s : send debugger address\n");
                printf("p : ping\n");
                printf("t : traceroute\n");
                printf("i : info about this node\n");
                printf("r : return\n");
                printf("q : quit\n");

                while(isspace(cmd=getchar()));

                switch (toupper(cmd)){
                case 'S' :  //sends debug address to distant node
                    printf("enter destination address\n");
                    if (scanf("%hx",&destAd) != 1){
                        printf("error getting destination address\n");
                    }
                    if ( (err=bn_debugSendAddr(destAd)) > 0){
                        printf("signalling send\n");
                        quitMenu=1;
                    }
                    else {
                        printf("error while sending : %d\n", err);

                    }
                    break;
                case 'P' :
                    destAd=0;
                    printf("enter destination adress\n");
                    scanf("%hx",&destAd);
                    printf("ping %hx : %d ms\n",destAd,bn_ping(destAd));
                    break;
                case 'T' :
                    {
                        destAd=0;

                        printf("enter destination adress\n");
                        scanf("%hx",&destAd);
                        int depth;
                        printf("enter depth\n");
                        scanf("%i",&depth);
                        sTraceInfo trInfo[depth];
                        nbTraces=bn_traceroute(destAd,trInfo,depth,1000);
                        for (f=0;f<nbTraces;f++){
                            printf("%hx in %d ms\n",trInfo[f].addr,trInfo[f].ping);
                        }
                    }
                    break;
                case 'I' :  //displays info about current node
                    {
                        printf("my addr (total) : %4hx\n",MYADDRX);
                        printf("my addr (local) : %4hx\n",MYADDRX&DEVICEX_MASK);
                        printf("my subnet  : %4hx\n\n",MYADDRX&SUBNET_MASK);
                    }
                    break;
                case 'R' : quitMenu=1; printf("back to listening, CTRL+C for menu\n\n"); break;
                case 'Q' : quitMenu=1; quit=1; break;
                default : break;
                }

            }

        }

    }

    printf("bye\n");

    return 0;
}
