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
#ifndef NDARIASONAR_H
#define NDARIASONAR_H

#include "ariarangedevice.h"
#include "../aria-tools/sonarinfowrapper.h"

/**
    @author jose manuel <jose@pcjose>
*/
class NDAriaSonar : public AriaRangeDevice
{

    Q_OBJECT

public:
    NDAriaSonar(int id, int group_idx, SonarInfoWrapper* sonar_inf,
                QObject* parent = 0, const char* name = 0);
    NDAriaSonar(int id, int group_idx, double x_, double y_, double rot,
                double orient, SonarInfoWrapper* sonar_inf, QObject* parent = 0,
                const char* name = 0);
    NDAriaSonar(int id, int group_idx, double rad, double rot, double orient,
                SonarInfoWrapper* sonar_inf, QObject* parent = 0, const char* name = 0);
    NDAriaSonar(int id, double rad, double start_ang, double end_ang, int group_idx,
                SonarInfoWrapper* sonar_inf, QObject* parent = 0, const char* name = 0);
    ~NDAriaSonar();

    int sensorSuperType(void);
    int sensorType(void);

    double sample(void);
    
protected:
    SonarInfoWrapper* sonar_info;

protected:
    void initParams(void);
    void initMeasure(void);

};

#endif
