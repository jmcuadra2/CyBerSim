//
// C++ Implementation: motorcontrol
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "motorcontrol.h"
#include "basesimcontrolfactory.h"

MotorControl::MotorControl()
{
    controlName = "MotorControl";
    driver = 0;
}

MotorControl::~MotorControl()
{
}

void MotorControl::initElements(void)
{
}

void MotorControl::setMotor(AbstractMotor* motor) 
{ 
    this->motor = motor;}

void MotorControl::connectDrive(bool /*on*/)
{
}

void MotorControl::readInputs(void)
{
}

void MotorControl::propagate(void)
{  
}

void MotorControl::backPropagate(void)
{    
}

void MotorControl::sendOutputs(void)
{
}

int MotorControl::getType(void)
{   
    return DIFFERENTIAL_MOTOR_CONTROL ;
}

int MotorControl::getSuperType(void)
{   
    return MOTOR_CONTROL ;
}

void MotorControl::doMonitor(void)
{   
}

void MotorControl::connectIO(void)
{
}

QList< QewExtensibleDialog* > MotorControl::tabsForEdit(QewExtensibleDialog* parent)
{
    QList<QewExtensibleDialog*> add_dialogs;
    return add_dialogs;
}

void MotorControl::setParams(const QDomElement& )
{
} 
