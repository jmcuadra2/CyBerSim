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
#ifndef ARSERVERMODEREACTIVE_H
#define ARSERVERMODEREACTIVE_H

#include <ArServerMode.h>
#include <ArVCC4.h>
#include <ArACTS.h>
#include <QProcess>
#include "../aria-tools/aractiongroupreactive.h"
/**
    @author jose manuel <jose@pcjose>
*/
class ArServerModeReactive : public ArServerMode
{
public:
    ArServerModeReactive(ArServerBase *server, ArRobot *robot, ArVCC4* vcc4 = 0, ArACTS_1_2* acts = 0);
    ~ArServerModeReactive();
    
    void activate(void);
    void deactivate(void);
    void reactiveWander(void);
    void netReactiveWander(ArServerClient *client, ArNetPacket *packet);
    void userTask(void);
    void checkDefault(void) { activate(); }
    ArActionGroup *getActionGroup(void) {return &myReactiveGroup;}

    int getBlobArea(void) { return blobArea ; }
    enum VideoResolution { WIDTH = 320, HEIGHT = 240 };

protected:
    void lookAround(void);
    
protected:
    ArActionGroupReactive myReactiveGroup;
    ArFunctor2C<ArServerModeReactive, ArServerClient *, ArNetPacket *> myNetReactiveWanderCB;

    ArVCC4* vcc4;
    int delayCount, commandsDelay;
    double panIncrement, actual_incre;
    ArTime myLastTime;
    ArACTS_1_2* acts;
    bool blobFound;
    int blobArea;
    QProcess *getVideo;
};

#endif
