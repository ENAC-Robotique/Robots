/*
 * ProcAbsPos.h
 *
 *  Created on: 17 avr. 2015
 *      Author: ludo6431
 */

#ifndef PROCESSES_PROCABSPOS_H_
#define PROCESSES_PROCABSPOS_H_

#include <processes/Process.h>
#include <tools/Acq.h>
#include <tools/TestPoint.h>
#include <string>
#include <vector>

class ProjAcq;

class ProcAbsPos: public Process {
protected:
    std::vector<TestPoint> staticTP;

    std::vector<TestPoint> getPosDependentTP(const Pos& robPos);
    float getEnergy(ProjAcq& pAcq, const Pos& robPos);

public:
    ProcAbsPos(Cam* c, const std::string& staticTestPointFile);
    virtual ~ProcAbsPos();

    virtual void process(const std::vector<Acq*>& acqList, const Pos& pos, const PosU& posU) override;
};

#endif /* PROCESSES_PROCABSPOS_H_ */