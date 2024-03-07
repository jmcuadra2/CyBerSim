//
// C++ Interface: nomadsimrobotfactory
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADSIMROBOTFACTORY_H
#define NOMADSIMROBOTFACTORY_H

#include "robotfactory.h"

/**
	@author Javier Garcia Misis 
*/
class NomadSimRobot;

class NomadSimRobotFactory : public RobotFactory
{
public:
    NomadSimRobotFactory();

    ~NomadSimRobotFactory();

    virtual Robot* createRobot(const QDomElement& e);

    void readRobotKinetics(const QDomElement& e);
    void setRobotKinetics(void);
    void setSensorMode(const QDomElement& e, bool load_first = false);
    bool setRobotInWorld(void);   


    static QString TYPE_NAME;

protected:
    NomadSimRobot* ns_robot;
    int type_iner;
    int noise;
    double max_rot_;
    bool first_time[8];

};

#endif
