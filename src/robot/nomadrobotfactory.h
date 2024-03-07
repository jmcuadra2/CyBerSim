//
// C++ Interface: nomadrobotfactory
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADROBOTFACTORY_H
#define NOMADROBOTFACTORY_H

#include "robotfactory.h"

/**
	@author Javier Garcia Misis
*/
class NomadRobotFactory : public RobotFactory
{
public:
    NomadRobotFactory();

    ~NomadRobotFactory();

    virtual bool setRobotInWorld();
    virtual Robot* createRobot(const QDomElement& e);
    virtual Robot* createRobot();
    virtual void readRobotKinetics(const QDomElement& e);
    virtual void readRobotKinetics();
    virtual void setRobotKinetics();
    virtual void setSensorMode(const QDomElement& e, bool load_first);


    static QString TYPE_NAME;

private:
    double max_rot_;

};

#endif
