
/****************************************************************
gestion des interruptions provenant des capteurs

pour arduino UNO
****************************************************************/

#include "Arduino.h"
#include "lib_int_laser.h"
#include "tools.h"
#include "../src/params.h"

#include "../../../communication/botNet/shared/bn_debug.h"

//#define DEBUG_LASER

enum {
    ACQUISITION,
    TRACKING,
    TRACK_DELAY,
    DEBUG_STAGE
};

//globales
#ifdef DEBUG_LASER_SPECIAL_STAGE
bufStruct buf0={{0},0,0,0,3,0,0,0,0,0};
bufStruct buf1={{0},0,0,0,3,0,0,0,0,1};
#else
bufStruct buf0={{0},0,0,0,0,0,0,0,0,0};
bufStruct buf1={{0},0,0,0,0,0,0,0,0,1};

#endif


#define LASER_THICK_MIN     24    // in µs refined with measurement
#define LASER_THICK_MAX     600 // in µs refined with measurement
#define LASER_MAX_MISSED    3
#define LASER_DEBOUNCETIME  20  //measured

#define LAT_SHIFT 2 //in µs TODO : refine



/*
laserIntInit :
  arguments : 0 ou 1 (numéro de l'interruption
  valeur de retour : rien
"attache" les interruptions associées aux couples de capteurs
*/
void laserIntInit(int irqnb) {
  pinMode(irqnb+2,INPUT);  // trick for the arduino UNO only
  attachInterrupt(irqnb, !irqnb?laserIntHand0:laserIntHand1 ,CHANGE);
}
    
//do I really have to do a description ? Anyway, it probably won't be used.
void laserIntDeinit(){
  detachInterrupt(0);
  detachInterrupt(1);
}

void laserIntHand0(){ //interrupt handler, puts the time in the rolling buffer
  //new! debouce, will hide any interruption happening less than DEBOUNCETIME µsec after the last registered interruption
    unsigned long time=micros();//mymicros();
    if ( time > (buf0.buf[(buf0.index-1)&7]+LASER_DEBOUNCETIME) ){
        buf0.buf[buf0.index]=time;
        buf0.index++;
        buf0.index&=7;
    }
    EIFR = BIT(0);
}

void laserIntHand1(){
  //new! debouce, will hide any interruption happening less than DEBOUNCETIME µsec after the last registered interruption
    unsigned long time=micros();//mymicros();
    if ( time > (buf1.buf[(buf1.index-1)&7]+LASER_DEBOUNCETIME) ){
        buf1.buf[buf1.index]=time;
        buf1.index++;
        buf1.index&=7;
    }
    EIFR = BIT(1);
}

//returns the delta-T in µs and the time at which it was measured
ldStruct laserDetect(bufStruct *bs){
    unsigned long prevCall, t = micros();
    unsigned long bufTemp[8], d1, d2;
    int i=8, ilast=0, nb;

    //noInterrupts();

    prevCall=bs->prevCall;
    do {
        i--;
        bufTemp[i]=bs->buf[i];
    } while(i);
    ilast=bs->index-1;//index of the last value written in the rolling buffer

    //interrupts();

    //looking for the index of first value updated since the last "interesting" call of laserDetect
    nb=0;
    i=ilast;
    while( nb<8 && long(bufTemp[i&7] - prevCall)>0 ) {
    i--;
    nb++;
    }
    if ( nb>=4 ){
        // we just got enough data, set the new update time
        d1 = bufTemp[ilast&7]-bufTemp[(ilast-2)&7];
        d2 = bufTemp[(ilast-1)&7]-bufTemp[(ilast-3)&7];
        unsigned long t1,t2;
        t1=bufTemp[ilast&7]-bufTemp[(ilast-1)&7];
        t2=bufTemp[(ilast-2)&7] - bufTemp[(ilast-3)&7];

        //if the detected patter has not the good shape
        //xxx in this case we can only handle a whole pattern (we may be able to do it on 3)
        if ( t1 < LASER_THICK_MIN || t1 > LASER_THICK_MAX || t2<LASER_THICK_MIN || t2>LASER_THICK_MAX || labs((((long)d1)-((long)d2))*100)>(d1+d2)*10 ){
            ldStruct ret={0,0,0};
            return ret;
        }
        else {
            bs->prevCall=t;

            ldStruct ret={(d1+d2)>>1, (bufTemp[(ilast-3)&7]+bufTemp[(ilast)&7])>>1, min( t1, t2 )};
            return ret ;
        }

    }
    else { //we don't have enough data,  just drop the current data <=> prevCall=t a few lines above
    }
    ldStruct ret={0,0,0};
    return ret;

}

/* Function co call periodically to poll if any new laser value
 * Argument :
 *  bs : pointer to the buffer structure to test
 *  pRet : pointer to the return structure. This latter is not modified if there is no new value
 * Return value :
 *  1 if something new has been detected and written in pRet,
 *  0 otherwise
 *
 */
int periodicLaser(bufStruct *bs,plStruct *pRet){
    unsigned long int time=micros();
    ldStruct measure={0};

    if ( time-bs->prevTime >= bs->timeInc){
        switch (bs->stage){
            case ACQUISITION : { //acquisition
                //laserdetect
                measure=laserDetect(bs);
                //if correct, goto tracking_clearing (stage 2), set the latency to LAT_INIT and the nextTime accordingly
                if (measure.deltaT!=0){
                    bs->stage=TRACK_DELAY;

                    bs->lat=laser_period>>LAT_SHIFT;
                    bs->prevTime=measure.date;
                    bs->timeInc=laser_period- (bs->lat>>1);
                    bs->lastDetect=measure.date;
                    bs->missed=0;

                    pRet->period=0;
                    pRet->deltaT=measure.deltaT;
                    pRet->date=measure.date;
                    pRet->thickness=measure.thickness;
                    pRet->sureness=laser_period;
                    pRet->precision=4; //in µs TODO
#ifdef DEBUG_LASER
                    bn_printfDbg("%lu acq->tra %lu %lu %lu\n",micros(),pRet->date,bs->lat,laser_period);
#endif
                    return 1;

                }
                //else, "delay" periodicLaser and return to acquisition
                else {

                    //"clear "the buffer and lastLaserDetection
                    bs->prevCall=time;
                    bs->lastDetect=0;

                    //set the nextime and prevtime and tolerated latency
                    bs->prevTime=time;
                    bs->timeInc=((3*laser_period)>>3); // NOT a period submultiple
                    bs->stage=ACQUISITION;

                    return 0;
                }
                break;
            }
            case TRACKING :{ //tracking
                //laserdetect
                measure=laserDetect(bs);
                if (measure.deltaT!=0){
                    pRet->deltaT=measure.deltaT;
                    pRet->date=measure.date;
                    pRet->thickness=measure.thickness;
                    pRet->sureness=(long int)(measure.date-bs->prevTime-(bs->lat>>1)); //sureness = difference between the expected time and the measured time
                    pRet->precision=4; //in µs TODO
                    if (bs->lastDetect) pRet->period=measure.date-bs->lastDetect;
                    else pRet->period=0;

                    bs->lastDetect=measure.date;
                    bs->lat=laser_period>>LAT_SHIFT;    //MAX( bs->lat-LAT_DEINC,LAT_MIN); todo refine
                    bs->prevTime=measure.date;
                    bs->timeInc=laser_period-(bs->lat>>1);
                    bs->stage=TRACK_DELAY;
                    bs->missed=0;
#ifdef DEBUG_LASER
                    bn_printfDbg("%lu tra->tra %lu %lu %lu\n",micros(),pRet->date,bs->lat,pRet->period);
#endif
                    return 1;
                }
                else {

                    bs->missed++;
                    // if not too much have been missed, keep going into tracking
                    if (bs->missed<LASER_MAX_MISSED){
                        bs->stage=TRACK_DELAY;
                        //fixme
                        bs->lat=laser_period>>LAT_SHIFT;    //MAX( bs->lat-LAT_DEINC,LAT_MIN); todo refine
                        bs->prevTime=micros();
                        bs->timeInc=laser_period-(bs->lat);
                        bs->stage=TRACK_DELAY;
#ifdef DEBUG_LASER
                        bn_printfDbg("%lu tra->tra missed %d\n",micros(),bs->missed);
#endif
                        return 0;
                    }
                    // otherwise, go back to acquisition
                    else {
                        //"clear "the buffer
                        bs->prevCall=time;
                        bs->lastDetect=0;

                        bs->lat=laser_period>>LAT_SHIFT;  //bs->lat+LAT_INC;
                        bs->prevTime=time;


                        bs->timeInc=((3*laser_period)>>3); // NOT a period submultiple
                        bs->stage=ACQUISITION;
#ifdef DEBUG_LASER
                        bn_printfDbg("%lu tra->acq\n",micros());
#endif
                        return 0;

                    }

                }
                break;
            }
            case TRACK_DELAY :{ // clear the buffer right before the beginning of the measurement time
                //"clear "the buffer
                bs->prevCall=time;

                //set the nextime and prevtime
                bs->prevTime=time;
                bs->timeInc=bs->lat;

                //return to stage traccking
                bs->stage=TRACKING;
                return 0;
                break;
            }
#ifdef DEBUG_LASER_SPECIAL_STAGE
            case DEBUG_STAGE : { //debug case
                static int nbdetected=0;
                //laserdetect
                measure=laserDetect(bs);
                //if correct, goto tracking_clearing (stage 2), set the latency to LAT_INIT and the nextTime accordingly
                if (measure.deltaT!=0){

                    nbdetected++;
                    pRet->period=0;
                    pRet->deltaT=measure.deltaT;
                    pRet->date=measure.date;
                    pRet->thickness=measure.thickness;
                    pRet->sureness=laser_period;
                    pRet->precision=4; //in µs TODO
                    bn_printfDbg("%lu acq->tra %lu %lu %lu %d\n",micros(),pRet->date,bs->lat,laser_period,nbdetected);

                }

                    //"clear "the buffer and lastLaserDetection
                    bs->prevCall=time;
                    bs->lastDetect=0;

                    //set the nextime and prevtime and tolerated latency
                    bs->prevTime=time;
                    bs->timeInc=((3*laser_period)>>3); // NOT a period submultiple
                    bs->stage=DEBUG_STAGE;

                    return 0;
                break;
            }
#endif
            default : break;
        }
    }
return 0;
}

/* delta2dist : converts delta-T and period in distance in mm
 * TODO : re-write it to take into account measured period
 */
uint32_t delta2dist(unsigned long delta, unsigned long period){
    float temp=((float)8.006/(((float)delta/(float)period) - (float)0.001127));//<-eureqa-ifed equation //25/( (delta/period-0.5*3.141593/180)/2);//approx of 25/sin( (delta/laser_period-0.5*3.141593/180)/2) (formula found by geometry)
    uint32_t temp2=temp;
    return temp2;
}
