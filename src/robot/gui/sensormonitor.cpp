//
// C++ Implementation: sensormonitor
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sensormonitor.h"

SensorMonitor::SensorMonitor(QWidget *parent)
		: QWidget(parent)
{
  setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
}


SensorMonitor::~SensorMonitor()
{
//   qDebug("~SensorMonitor()");
}

void SensorMonitor::setLCD(int val)
{
  lcdNumber->display(val);  
}

void SensorMonitor::mouseDoubleClickEvent(QMouseEvent * /*event*/)
{
  close();
}

