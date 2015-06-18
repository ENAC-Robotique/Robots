/*
 * DisplayImage.cpp
 *
 *  Created on: 23 févr. 2014
 *      Author: yoyo
 */

#include <opencv2/core/core.hpp>
#include <processes/ProcAbsPosNTree.h>
#include <processes/ProcAbsPos.h>
#include <processes/Process.h>
#include <tools/AbsPos2D.h>
#include <tools/Acq.h>
#include <tools/Cam.h>
#include <tools/Uncertainty2D.h>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

#include "shared/botNet_core.h"

#include "tools.hpp"
#include "params.hpp"
#include "process.hpp"
#include "performance.hpp"
#include "save.hpp"
#include "sourceVid.hpp"

//##### Main #####
int main(int argc, char* argv[]) {
    Perf& perf = Perf::getPerf();
    Mat frameRaw;

    // init cameras
    map<Cam*, VideoCapture*> camList;
    camList.insert(make_pair(
            new Cam(516.3, Size(640, 480), Transform3D<float>(0, 12.7, 26.7, 226. * M_PI / 180., 0, 0)),
            //            new VideoCapture("MyVideo.avi")));
//            new VideoCapture(0)));
//            new VideoCapture("Images/captures/guvcview_image-25.jpg"))); // coin avec 2 pieds
//            new VideoCapture("Images/captures/1-955-743.jpg"))); // zone de départ jaune
            new VideoCapture("Images/captures/z1.png"))); // "obomovie"

// init processes
    vector<Process*> processList;
    processList.push_back(new ProcAbsPosNTree(camList.begin()->first, "simu/testpoints.csv"));

    // init botnet
    bn_init();

    if (!camList.begin()->second->read(frameRaw)) { //if not success, break loop
        cout << "Cannot read the frame from source video file" << endl;
    }

    bool quit = false;
    do {
//		ret = bn_receive(&inMsg);
//
//		switch(inMsg.header.type){
//            case E_POS_CAM:
        perf.beginFrame();

        for (Process* p : processList) {
            vector<Acq*> acqList;

            for (Cam* c : p->getCamList()) {
                map<Cam*, VideoCapture*>::iterator it = camList.find(c);

                // Read a new frame from the video source
//                if (!it->second->read(frameRaw)) {  //if not success, break loop
//                    cout << "Cannot read the frame from source video file" << endl;
//                    continue;
//                }

                if (frameRaw.size() != c->getSize()) {
                    continue;
                }

//                imshow("rgb", frameRaw);

                acqList.push_back(new Acq(frameRaw, BGR, c));
            }

            perf.endOfStep("acquisitions");

//            p->process(acqList, AbsPos2D<float>(45, 65, 130. * M_PI / 180.), Uncertainty2D<float>(180, 180, 0, 10.f * M_PI / 180.f)); /// optim: 39.58, 59.58, 134.99
//            p->process(acqList, AbsPos2D<float>(100, 62, 60 * M_PI / 180.), Uncertainty2D<float>(180, 180, 0, 10.f * M_PI / 180.f)); /// optim: 93.58, 73.58, 64
            p->process(acqList, AbsPos2D<float>(145, 30, 5 * M_PI / 180.), Uncertainty2D<float>(180, 180, 0, 10.f * M_PI / 180.f)); /// optim: 159.58, 21.58, 0

            for (Acq* a : acqList) {
                delete a;
            }

            perf.endOfStep("process");
        }

//        switch(waitKey(1000./10)){
//        case 0x110001B:
//        case 27:
//            quit = true;
//            break;
//        default:
//            break;
//        }

        perf.endFrame();
        quit = 1;
//
//                break;
//            case E_DATA:
//            case E_PING:
//                break;
//            default:
//                bn_printfDbg("got unhandled msg: type%hhu sz%hhu", inMsg.header.type, inMsg.header.size);
//                break;
//		}  // End switch

    } while (!quit);  // End while

    printf("End loop\n");

    return 0;
}
