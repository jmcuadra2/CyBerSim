//
// C++ Interface: robot2dmotorplan
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ROBOT2DMOTORPLAN_H
#define ROBOT2DMOTORPLAN_H

#include "outputmotorplan.h"

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class Robot2DMotorPlan : public OutputMotorPlan
{
public:
    Robot2DMotorPlan(AbstractMotor* motor);

    ~Robot2DMotorPlan();

};

#endif
