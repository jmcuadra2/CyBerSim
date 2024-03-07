/***************************************************************************
                          sensorsbuilder.h  -  description
                             -------------------
    begin                : Thu Jul 21 2005
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

#ifndef SENSORSBUILDER_H
#define SENSORSBUILDER_H

#include <QDomDocument>

class AbstractWorld;
class RobotPosition;
class SensorsSet;

/*! \ingroup Sensores
    \brief Construye SensorsSet invocando a las oportunas factorias.
  */


class SensorsBuilder {
  
  public:
    SensorsBuilder();
    ~SensorsBuilder();

    void construct(SensorsSet* sensorsSet, QDomElement& e);
    void setWorld(AbstractWorld* world_);
    void setRobotPosition(RobotPosition* robot_pos);
    bool fileChanged(void) { return file_changed ; };

  protected:
    void factorySensorGroup(QDomElement& e);

  protected:
    AbstractWorld* world;
    RobotPosition* robot_position;
    SensorsSet* sensorsSet;
    int sensor_supertype;
    bool file_changed; 
};

#endif
