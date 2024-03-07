/**************************************************************************
                          robotbuilder.h  -  description
                             -------------------
    begin                : Tue Mar 29 2005
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

#ifndef ROBOTBUILDER_H
#define ROBOTBUILDER_H

#include <QDomDocument>

class AbstractWorld;
class Robot;
class RobotFactory;
class RobotDrawer;
class XMLOperator;

/*! \ingroup robot_base 
    \brief En esta revision solo contruye robots con dos ruedas
    no holonomicos, no invoca factorias.
  */
  
class RobotBuilder {
  
  public:
    RobotBuilder(AbstractWorld* world);
    ~RobotBuilder();

    Robot* construct(QDomElement& e);

   protected:
    RobotDrawer* factoryDrawer(const QString& r_name);
    void setRobotConstructor(const QDomElement& e);
    void setIntrinsicVars(const QDomElement& e);
    void setSensorsGroups(QDomElement& e);                       
    
  protected:
    AbstractWorld* world;
    Robot* robot;
    XMLOperator* xml_operator;
    RobotFactory* factory;   
          
};

#endif
