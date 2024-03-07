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
#ifndef NDARIAROBOT_H
#define NDARIAROBOT_H

#include <QMouseEvent>

#include "../aria-tools/robotinfowrapper.h"
#include "robot.h"
#include "differentialmotor.h"

class ArClientBase;
class ArClientSimpleConnector;
class ArNetPacket;


/**
    @author jose manuel <jmcuadra@dia.uned.es>
*/
class NDAriaRobot : public Robot
{
    
    Q_OBJECT
    
    friend class NDAriaRobotFactory;
    friend class NDAriaRobotKineticsTab;

public:
    NDAriaRobot(int n_id, QObject* parent, QString name);
    
    NDAriaRobot(const QDomElement& e, QObject* parent, QString name);
    
    virtual ~NDAriaRobot();
    
    bool okPosition(void);
    const QString type(void);
    QList<QewExtensibleDialog*> createKineticsDialog(QewExtensibleDialog* parent);
    void advance(int stage);

    void initMotors(void);
    void initRecord(void);
    void outputMotors(void);
    void rotation(double rot_);
    void sensorsVisible(bool on);
    void setIntrinsicVars(const QDomElement& e_size);
    void setVel2(double r_vel, double l_vel);
    bool stop(void);
    void viewSensors(bool show);

    void setOldVel(double r_vel, double l_vel);
    void initSim(bool reset);
    void setStatus(bool on);
    void handleRobotUpdate(ArNetPacket* packet);
    virtual void handleRobotExtUpdate(ArNetPacket* packet);
    void handleSensorsList(ArNetPacket* packet);
    void* getClient(void) {return  (void*)client ; }
    ArClientBase* getAriaClient(void) {return client ; }
    double getY(void);
    double getX(void);
    double getRotation(void);
    void gotoPose(double x, double y, double th = 0.0);
    bool hasSensorType(int type);
    list< int > * readActualSensorList();
    
    void setMainMotor(void);
    
    double getLeftVel(void) { return robot_info.getLeftVelRobot()/velUnitsConversion ; }
    double getRightVel(void) { return robot_info.getRightVelRobot()/velUnitsConversion ; }
    void setRotInit(double value);
    
public slots:
    void getMouseEvent(QMouseEvent* e);
    void recordRobot(double x, double y, double rot, double vx,
                     double vy, double vrot, bool forw);
    void setXY(double x_, double y_);
    void setXYInit(double x, double y);
    void updateOdometries(bool paintRaw = true, bool paintSlam = true);
    
protected:
    bool isCrash(void);
    void save_special(QDomElement& );

    void setCanvas(QGraphicsScene* w_canvas);
    void write_special(QDomDocument& doc, QDomElement&  e);
    void setAriaRobot(ArClientBase* r_client, ArClientSimpleConnector* r_clientConnector, bool realRobot);
    
protected:    
    ArClientBase* client;
    RobotInfoWrapper robot_info;
    ArClientSimpleConnector* clientConnector;
    int battery;
    bool stopping;
    
    double diffConvFactor;

};

#endif
