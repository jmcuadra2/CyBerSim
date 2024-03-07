/***************************************************************************
                          basesimulationfactory.h  -  description
                             -------------------
    begin                : Thu Apr 21 2005
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

#ifndef BASESIMULATIONFACTORY_H
#define BASESIMULATIONFACTORY_H

#include <QDomDocument>

#include "../neuraldis/dictionarymanager.h"

class BaseSimulation;
class BaseSimControl;
class XMLOperator;

/*!
  *@author Jose M. Cuadra Troncoso
  */    

class BaseSimulationFactory {
  
  protected:
 	  BaseSimulationFactory();

  public:    
 	  virtual ~BaseSimulationFactory();
    
//    void construct(BaseSimulation* b_sim, QDomElement& e);
    virtual BaseSimulation* factorySim(int sim_type, const QDomElement& e, const QString& sim_name) = 0;     
    virtual bool construct(void);
    bool needWorld(void) { return need_world ; }
    
  protected:
    virtual void setTimeControl(const QDomElement& e);
    virtual void setClocks(void);
    virtual BaseSimControl* constructControl(QDomElement& e);
    virtual BaseSimControl* constructControl(void);    
    virtual void setScreenUpdate(const QDomElement& e);
    virtual void setSimBase(BaseSimulation* b_sim) { sim = b_sim ; };

  protected:
    XMLOperator* xml_operator;
    QDomElement info_sim;
    bool need_world;    

  private:
    BaseSimulation* sim;    
           
};

#endif
