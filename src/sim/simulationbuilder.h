/***************************************************************************
                          simulationbuilder.h  -  description
                             -------------------
    begin                : Wed Apr 6 2005
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

#ifndef SIMULATIONBUILDER_H
#define SIMULATIONBUILDER_H

#include <QDomDocument>

class BaseSimulation;
class Robot;
class World;
class Settings;

/**
  *@author Jose M. Cuadra Troncoso
  */
  
class SimulationBuilder {
  
  public:
  	SimulationBuilder();
  	~SimulationBuilder();

    BaseSimulation* construct(QDomDocument& doc, const QString& f_name);
    BaseSimulation* construct(QDomDocument& doc);
    bool needWorld(void) { return need_world ; };            

  protected:
    BaseSimulation* factorySimulation(int sim_type, QDomElement& e);
/*    BaseSimulation* factorySimulation(int sim_type); */   

  protected:
    BaseSimulation* sim;
    QString file_name;
    bool need_world;
    
};

#endif
