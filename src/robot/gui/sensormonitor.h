//
// C++ Interface: sensormonitor
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SENSORMONITOR_H
#define SENSORMONITOR_H

#include <QWidget>

#include "ui_sensormonitor.h"

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class SensorMonitor : public QWidget, public Ui::SensorMonitor
{
		Q_OBJECT
	public:
		SensorMonitor(QWidget *parent = 0);

		~SensorMonitor();

  public slots:
    void setLCD(int val);
    
  protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif
