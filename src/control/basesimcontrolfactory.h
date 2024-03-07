/***************************************************************************
                          basesimcontrolfactory.h  -  description
                             -------------------
    begin                : Wed Apr 27 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BASESIMCONTROLFACTORY_H
#define BASESIMCONTROLFACTORY_H

#include <QDomDocument>

#include "motorcontrol.h"
#include "drivecontrol.h"
#include "../neuraldis/dictionarymanager.h"

/*!
  *@author Jose M. Cuadra Troncoso
  */

class BaseSimControlFactory {
  
  protected:
  	BaseSimControlFactory();
    
  public: 
   	virtual ~BaseSimControlFactory() = 0;
  
    virtual BaseSimControl* construct(int , const QDomElement& , const QString&) = 0; 
    virtual BaseSimControl* construct(void) = 0; 
                                       
//     static BaseSimControl* createSensorGroupControl(SensorGroup* sensorGroup);
    static BaseSimControl* createMotorControl(AbstractMotor* motor);
    static BaseSimControl* createDriveControl(AbstractMotor* motor);    
//     static QList<BaseSimControl*> createSensorGroupControls(SensorsSet* sensorsset);
     // int type, const QString& control_filename
  protected:
    bool hasView;
    
};

#endif
