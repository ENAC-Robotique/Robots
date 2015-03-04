/*
 * log.h
 *
 *  Created on: 1 mars 2015
 *      Author: seb
 */

#ifndef MAIN_AI_TOOLS_LOG_H_
#define MAIN_AI_TOOLS_LOG_H_

#include <iostream>
#include <fstream>

#define ERR_V(a)  E_ERR  << a << "[ERROR]   " << "[" << __FILE__ << ":" << __LINE__ << "] "
#define WAR_V(a)  E_WAR  << a << "[WARNING] " << "[" << __FILE__ << ":" << __LINE__ << "] "
#define INFO_V(a) E_INFO << a << "[INFO]    "
#define MES_V(a)  E_MES  << a << "[MESSAGE] "
#define OTHER_V(a) E_MES << a << "[OTHER]   "

#define ERR ERR_V(E_V1)
#define WAR WAR_V(E_V1)
#define INFO INFO_V(E_V2)
#define MES MES_V(E_V2)
#define OTHER OTHER_V(E_V2)

typedef enum {E_ERR, E_WAR, E_INFO, E_MES, E_OTHER} eLog_t;
typedef enum {E_V1, E_V2, E_V3} eVer_t;

using namespace std;

class Log {
    public:
        Log();
        Log(char* file);
        ~Log();

        void changeFile(const char* file);
        void setType(const eLog_t& log);
        void setVer(const eVer_t& ver);

        void putNewMes();
        void putNewPos(float x, float y, float theta);

        void putText(const char* text);
        void putNum(const float& num);

        bool term() const;

    private:
        ofstream _file;
        eLog_t _type;
        eVer_t _ver;
        bool _pos; //true if the previous message was a position

};

extern Log& operator<<(Log& log, const char*text);
extern Log& operator<<(Log& log, const int num);
extern Log& operator<<(Log& log, const float num);
extern Log& operator<<(Log& log, const double num);
extern Log& operator<<(Log& log, const uint32_t num);
extern Log& operator<<(Log& log, eLog_t type);
extern Log& operator<<(Log& log, eVer_t ver);

#endif /* MAIN_AI_TOOLS_LOG_H_ */


