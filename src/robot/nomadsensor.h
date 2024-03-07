//
// C++ Interface: nomadsensor
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADSENSOR_H
#define NOMADSENSOR_H

#include "abstractsensor.h"

namespace Nomad{
    class Connector;
}
/**
	@author Javier Garcia Misis
*/
class NomadSensor : public AbstractSensor
{
public:
    NomadSensor(int id, double rad, double start_ang, double end_ang, int group_idx, QObject* parent, const char* name);
    
    NomadSensor(int id, int group_idx, double rad, double rot, double orient, QObject* parent, const char* name);
    
    NomadSensor(int id, int group_idx, double x_, double y_, double rot, double orient, QObject* parent, const char* name);
    
    NomadSensor(int id, int group_idx, QObject* parent, const char* name);

    ~NomadSensor();

    virtual int sensorSuperType(void) {return NOMAD_SENSOR;}
    virtual void setRobotPosition(RobotPosition *rob_pos);

    void advance(int stage);
    void moveBy(double dx, double dy);
    void moveBy(double dx, double dy, double ang);
    void rotate(void );
    void showField(bool show);
    void setPosition(double x_, double y_, double rot);
    void setVisible(bool on);

    int getIdLocal(){ return ident%1000;}

    virtual double sample(void);
    virtual int getRawReading(bool obtain = true);
    
    void setConnector(Nomad::Connector * con);
    Nomad::Connector * getConnector();
    

private:
    Nomad::Connector * connector_;

};

#endif
