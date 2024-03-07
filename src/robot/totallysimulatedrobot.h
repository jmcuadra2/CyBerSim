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
#ifndef TOTALLYSIMULATEDROBOT_H
#define TOTALLYSIMULATEDROBOT_H

#include "robot.h"
#include <QMouseEvent>
/**
@author jose manuel
*/

/*! \ingroup robot_base 
    \brief Controla todos los aspectos de un  robot totalmente simulado ejecutando la simulacion,
    directamente o a traves de otras clases.
  */

class TotallySimulatedRobot : public Robot
{

    Q_OBJECT

    friend class RobotBuilder;
    friend class RobotDrawer;
    friend class RobotKineticsTab;

public:
    TotallySimulatedRobot(const QDomElement& e, QObject* parent = 0, QString name = "");
    
    TotallySimulatedRobot(int n_id, QObject* parent = 0, QString name = "");

    ~TotallySimulatedRobot();

    void setAdvancePeriod(int ap);
    int getAdvancePeriod(void) { return advance_period ; }
    void advance(int stage);
    bool okPosition(void);
    void setSensorMode(int sm, bool load_first = false);
    int getSensorMode(void) { return sensorMode ; }

    void initSim(bool reset);
    void setTypeIner(int t_iner) { type_iner = t_iner ;}
    int getTypeIner(void) { return type_iner ;}
    
    bool stop(void);
    void outputMotors(void);
    const QString type(void) { return "2FixedDrivingWheels" ; }
    void initRecord(void);

    void setNoise(int nois);
    int getNoise(void) { return noise ; }
    void setStatus(bool on);

    QList<QewExtensibleDialog*> createKineticsDialog(QewExtensibleDialog* parent);
    void* getClient(void) {return  (void*)0 ; }
    double getY(void);
    double getX(void);
    double getRotation(void);
    double getLinearVel(void);
    double getRadialVel(void);

    void setMainMotor(void);

public slots:
    void recordRobot(double x, double y, double rot,
                     double vx, double vy, double vrot, bool forw);
    void getMouseEvent(QMouseEvent* e);

protected:
    void setCanvas(QGraphicsScene* w_canvas);
    void save_special(QDomElement& elem);
    void write_special(QDomDocument& doc, QDomElement& e);
    bool isCrash(void);

protected:
    int type_iner;
    int sensorMode;
    int noise;
};

#endif

