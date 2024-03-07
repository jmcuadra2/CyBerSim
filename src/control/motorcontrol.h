//
// C++ Interface: motorcontrol
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include "basesimcontrol.h"
#include "../robot/abstractmotor.h"

class DriveControl;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class MotorControl : public BaseSimControl
{

public:
    MotorControl();

    virtual ~MotorControl();
    void sendOutputs(void);
    int getType(void);
    int getSuperType(void);
    void backPropagate(void);
    void doMonitor(void);
    void connectIO(void);
    QList<QewExtensibleDialog*> tabsForEdit(QewExtensibleDialog* parent);
    void initElements(void);
    void setParams(const QDomElement& e);
    virtual void setMotor(AbstractMotor* motor);
    void setDriver(DriveControl* driver) { this->driver = driver; }
    DriveControl* getDriver() const { return driver ; }
    virtual void connectDrive(bool on);


protected:
    void readInputs(void);
    void propagate(void);
    
protected:
    AbstractMotor* motor;
    DriveControl* driver;

};

#endif
