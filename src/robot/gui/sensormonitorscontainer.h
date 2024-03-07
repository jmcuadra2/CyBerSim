//
// C++ Interface: sensormonitorscontainer
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SENSORMONITORSCONTAINER_H
#define SENSORMONITORSCONTAINER_H

#include "sensormonitor.h"
#include "../../neuraldis/mdisubwininterface.h"
#include "ui_sensormonitorscontainer.h"

class AbstractSensor;
class SensorMonitor;

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class SensorMonitorsContainer : public QewSimpleDialog , public MdiSubWinInterface , protected Ui::SensorMonitorsContainer
{
	Q_OBJECT
	public:
		SensorMonitorsContainer(QWidget *parent = 0);

		~SensorMonitorsContainer();
    
    void addMonitor(AbstractSensor* sensor);
  
  public slots:
    void monitorClosed(QObject* monitor);

  protected slots:
    void changeColumns(int numCols);
    
  protected:
    int numMonitors;
    int monsPerColumn;
    QList<QWidget* > monitors;

};

#endif
