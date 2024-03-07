//
// C++ Implementation: sensormonitorscontainer
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sensormonitorscontainer.h"
#include "../abstractsensor.h"

SensorMonitorsContainer::SensorMonitorsContainer(QWidget *parent)
    : QewSimpleDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    numMonitors = 0;
    monsPerColumn = 4;
    columnsSpinBox->setValue(monsPerColumn);
    connect(columnsSpinBox, SIGNAL(valueChanged(int )), this, SLOT(changeColumns(int )));
    setUpDialog();
    //   RecordForm* recordForm = new RecordForm(this);
    //   if(recordForm->setUpDialog())
    //     addExtensibleChild(recordForm , "");
}


SensorMonitorsContainer::~SensorMonitorsContainer()
{ 
    //   qDebug("~SensorMonitorsContainer()");
    emit destroyed(this);
}

void SensorMonitorsContainer::addMonitor(AbstractSensor* sensor)
{
    SensorMonitor* sensorMonitor = new SensorMonitor(this);
    monitors.append(sensorMonitor);
    containerLayoutMonitors->addWidget(sensorMonitor, numMonitors/monsPerColumn, numMonitors%monsPerColumn);
    sensorMonitor->label->setText(QString::number(sensor->getOrientation()));
    sensorMonitor->label->setToolTip(sensor->toString());
    //   adjustSize();

    connect(sensorMonitor, SIGNAL(destroyed(QObject* )), this, SLOT(monitorClosed(QObject* )));
    connect(sensor, SIGNAL(getSensorValue(int)),
            sensorMonitor, SLOT(setLCD(int)));
    connect(sensorMonitor, SIGNAL(destroyed(QObject* )), sensor, SLOT(monitorClosed(QObject* )));
    sensor->setEmitSignal(true);

    ++numMonitors;
}

void SensorMonitorsContainer::monitorClosed(QObject* monitor)
{
    if(!monitor->isWidgetType()) return;

    containerLayoutMonitors->removeWidget((QWidget*) monitor);
    monitors.removeOne((QWidget*) monitor);
    adjustSize();
    --numMonitors;
    if(!numMonitors)
        close();
}

void SensorMonitorsContainer::changeColumns(int numCols)
{
    if(!numCols) return;

    if(monsPerColumn != numCols) {
        monsPerColumn = numCols;
        hide();
        for(int i = 0; i < monitors.size(); i++)
            containerLayoutMonitors->removeWidget(monitors[i]);
        for(int i = 0; i < monitors.size(); i++)
            containerLayoutMonitors->addWidget(monitors[i], i/monsPerColumn, i%monsPerColumn);
        show();
        adjustSize();
    }
}
