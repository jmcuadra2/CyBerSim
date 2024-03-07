//
// C++ Interface: ariasimulation
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ARIASIMULATION_H
#define ARIASIMULATION_H

#include "robotsimulation.h"

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AriaSimulation : public RobotSimulation
{
public:
    AriaSimulation(QObject* parent = 0, const char* name = 0);

    ~AriaSimulation();

    void stop(void);
    int simulationType(void) const;

  public slots:
    void pause(void);
    
  protected slots:
    void slotStep(void);

};

#endif
