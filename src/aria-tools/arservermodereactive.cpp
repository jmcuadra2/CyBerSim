/***************************************************************************
 *   Copyright (C) 2004 by jose manuel                                     *
 *   jose@pcjose                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "arservermodereactive.h"

ArServerModeReactive::ArServerModeReactive(ArServerBase *server, ArRobot *robot, ArVCC4* vcc4, ArACTS_1_2* acts)
    : ArServerMode(robot, server, "reactive_wander"), myReactiveGroup(robot),
      myNetReactiveWanderCB(this, &ArServerModeReactive::netReactiveWander)
{
    myMode = "Reactive Wander";
    if (myServer != NULL)
        myServer->addData("reactive_wander", "makes the robot wander reactivily", &myNetReactiveWanderCB,
                          "none", "none", "Movement");

    this->vcc4 = vcc4;
    this->acts = acts;
    delayCount = -1;
    commandsDelay = 3;
    panIncrement = 3;
    actual_incre = panIncrement;
    blobArea = -1;

    myLastTime.setToNow();

    getVideo = 0;
}

ArServerModeReactive::~ArServerModeReactive()
{
    if(getVideo)
        delete getVideo;
}

void ArServerModeReactive::activate(void)
{
    if (!baseActivate())
        return;

    setActivityTimeToNow();
    myRobot->clearDirectMotion();
    myReactiveGroup.activateExclusive();
    myStatus = "Reactive Wandering";
    if(vcc4)
        vcc4->pan(0);
    if(acts) {
        QString program = "getVideoExample";
        QStringList arguments;
        arguments << "-host" << "localhost" << "-p" << "7070";
        arguments << "-rate" << "1" << "-counter" << "-secname" << "consejo";

        getVideo = new QProcess(0);
        getVideo->setWorkingDirectory("/home/ActivMedia/consejo");
        getVideo->start(program, arguments);
    }
}

void ArServerModeReactive::deactivate(void)
{
    myReactiveGroup.deactivate();
    baseDeactivate();
    if(vcc4)
        vcc4->pan(0);
    if(acts) {
        getVideo->kill();
        delete getVideo;
        getVideo = 0;
    }
}

void ArServerModeReactive::reactiveWander(void)
{
    activate();
}

void ArServerModeReactive::netReactiveWander(ArServerClient *client, 
                                             ArNetPacket *packet)
{
    if(!isActive()) {
        myRobot->lock();
        ArLog::log(ArLog::Verbose, "Reactive Wandering");
        reactiveWander();
        myRobot->unlock();
        setActivityTimeToNow();
    }
    else {
        myRobot->lock();
        ArLog::log(ArLog::Verbose, "Stop reactive Wandering");
        deactivate();
        myRobot->unlock();
    }
}

void ArServerModeReactive::userTask(void)
{
    if(vcc4) {
        ++delayCount;
        if(delayCount < 150)
            return;
        if(!(delayCount%commandsDelay)) {

            if(acts) {
                ArACTSBlob blob;
                int numChannels = 8;
                ArACTSBlob largestBlob;
                blobArea = 10;
                //         ArACTSBlob largestBlob1;
                //         ArACTSBlob largestBlob2;
                //         int blobArea1 = 10;
                //         int blobArea2 = 10;
                int blobsChannels[] = {0, 0, 0, 0, 0, 0, 0, 0};// ESTE CÓDIGO ESTÁ REPETIDO EN ArServerExtInfoRobot
                /*        int numberOfBlobs = 0;
        for(int i = 0; i < numChannels; i++) {
          blobsChannels[i] = acts->getNumBlobs(i+1);
          numberOfBlobs += blobsChannels[i];
        }*/
                // If there are blobs to be seen, set the time to now
                /*        int ch1 = -1;
        int ch2 = -1;
        for(int j = 0; j < numChannels/2; j++) {
          blobsChannels[j] = acts->getNumBlobs(j+1);
          for(int i = 0; i < blobsChannels[j]; i++) {
            acts->getBlob(j + 1, i + 1, &blob);
            if(blob.getArea() > blobArea1) {
              blobArea1 = blob.getArea();
              largestBlob1 = blob;
              ch1 = j;
            }
          }
        }

        for(int j = numChannels/2; j < numChannels; j++) {
          blobsChannels[j] = acts->getNumBlobs(j+1);
          for(int i = 0; i < blobsChannels[j]; i++) {
            acts->getBlob(j + 1, i + 1, &blob);
            if(blob.getArea() > blobArea2) {
              blobArea2 = blob.getArea();
              largestBlob2 = blob;
              ch2 = j;
            }
          }
        } */

                int ch = -1;
                for(int j = 0; j < numChannels; j++) {
                    blobsChannels[j] = acts->getNumBlobs(j+1);
                    for(int i = 0; i < blobsChannels[j]; i++) {
                        acts->getBlob(j + 1, i + 1, &blob);
                        if(blob.getArea() > blobArea) {
                            blobArea = blob.getArea();
                            largestBlob = blob;
                            ch = j;
                        }
                    }
                }

                //         if(blobArea1 > 500 && blobArea1 > 500) {
                //           blobArea = blobArea1 + blobArea1;
                //           ArLog::log(ArLog::Terse, "channel1 = %d channel2 = %d blobArea = %d pan = %f", ch1, ch2, blobArea, vcc4->getPan());
                // //           if(!blobFound) {
                // //             blobFound = true;
                // //           }
                //
                //           double blobXCG1 = largestBlob1.getXCG();
                //           double blobYCG1 = largestBlob1.getYCG();
                //           double blobXCG2 = largestBlob2.getXCG();
                //           double blobYCG2 = largestBlob2.getYCG();
                //
                //           double blobXCG = (blobXCG1*blobArea1 + blobXCG2*blobArea2)/blobArea;
                //           double blobYCG = (blobYCG1*blobArea1 + blobYCG2*blobArea2)/blobArea;

                if(blobArea > 800) {
                    ArLog::log(ArLog::Terse, "channel = %d blobArea = %d pan = %f", ch, blobArea, vcc4->getPan());
                    //           if(!blobFound) {
                    //             blobFound = true;
                    //           }

                    double blobXCG = largestBlob.getXCG();
                    double blobYCG = largestBlob.getYCG();
                    double xRel = (blobXCG - WIDTH/2.0) / WIDTH;
                    double yRel = (blobYCG - HEIGHT/2.0) / HEIGHT;

                    //           // Tilt the camera toward the blob
                    //           if(yRel > 0.20)
                    //             vcc4->tiltRel(-1);
                    //           else if(yRel < -0.20)
                    //             vcc4->tiltRel(1);


                    // Pan the camera toward the blob
                    if(xRel > 0.05 || xRel < -0.05) {
                        double panRel = -xRel*5;
                        vcc4->panRel(panRel);
                    }

                }
                else
                    lookAround();
                //       myLastTime.setToNow();
            }
        }
    }
    setActivityTimeToNow();

}

void ArServerModeReactive::lookAround(void)
{
    if(vcc4) {
        double l_pan = vcc4->getPan();
        ArLog::log(ArLog::Terse, "getPan: %f", l_pan);
        //   if(l_pan <= vcc4->getMaxNegPan())
        if(l_pan <= -95)
            actual_incre = panIncrement;
        //   else if(l_pan >= vcc4->getMaxPosPan())
        else if(l_pan >= 95)
            actual_incre = -panIncrement;
        vcc4->panRel(actual_incre);
        ArLog::log(ArLog::Terse, "Time: %d", myLastTime.mSecSince ());
    }
}
