//
// C++ Interface: nomadsimrobot
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADSIMROBOT_H
#define NOMADSIMROBOT_H

#include "robot.h"
#include <QMouseEvent>

/**
    @author Javier Garcia Misis
*/
class NomadSimRobot : public Robot
{

public:
    
    NomadSimRobot(int n_id, QObject* parent = 0, const char* name = 0);

    virtual ~NomadSimRobot();

    virtual bool okPosition();
    virtual bool stop();
    virtual const QString type();
    virtual double getRotation();
    virtual double getX();
    virtual double getY();
    virtual void initSim(bool reset);
    virtual void outputMotors();
    virtual void rotation(double rot_);
    virtual void setMainMotor();
    virtual void advance(int stage);
    virtual void* getClient(void);
    void setSensorMode(int sm, bool load_first);
    int getSensorMode(void) { return sensorMode ; }
    void setNoise(int nois);
    int getNoise(void) { return noise ; }
    void setAdvancePeriod(int ap);

    //     static QString TYPE_NAME;
    void getKeyEvent(QKeyEvent* event, bool key_on);

    void setTypeIner(int t_iner) { type_iner = t_iner ;}
    int getTypeIner(void) { return type_iner ;}

    void setMaxRot(double m_vel){ max_rot_ = m_vel;}
    double getMaxRot(void) { return max_rot_ ; }

protected:
    void setCanvas(QGraphicsScene* w_canvas);

private:
    int type_iner;
    int sensorMode;
    int noise;
    double max_rot_;
    static QString TYPE_NAME;

};

#endif
