//
// C++ Implementation: sensorsrecorder
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QStringList>

#include "sensorsrecorder.h"

SensorsRecorder::SensorsRecorder(int id, const QString& fname, int f_mode,
                                 QObject* parent,const char* name) :
                FileRecorder(id, fname, f_mode, parent, name)
{
    type = SENSORS_RECORDER;
}


SensorsRecorder::SensorsRecorder(int id, const QString& fname, QVector< int > channels,
                                 int f_mode, QObject* parent, const char* name) :
                FileRecorder(id, fname, channels, f_mode, parent, name)
{
    type = SENSORS_RECORDER;
}


SensorsRecorder::SensorsRecorder(int id, const QString& fname, const QDomElement& e,
                                 int f_mode, QObject* parent, const char* name) :
                FileRecorder(id, fname, e, f_mode, parent, name)
{
    type = SENSORS_RECORDER;
}


SensorsRecorder::SensorsRecorder(int id, QObject* parent, const char* name) :
                FileRecorder(id, parent, name)
{
    type = SENSORS_RECORDER;
}


SensorsRecorder::SensorsRecorder(const QDomElement& e, QObject* parent, const char* name) :
                FileRecorder(e, parent, name)
{
    type = SENSORS_RECORDER;
}


SensorsRecorder::~SensorsRecorder()
{
}

void SensorsRecorder::initRecordText(QTextStream& ts, QStringList& data)
{
    figFileHeader(ts, data);
    QStringList data_sen = data[8].split("\n");
    ts << "# " + tr("Sensors group:") << " " << data_sen[0] << Qt::endl;
    ts << "# " + tr("Sensor maximun:") << " " << data_sen[1] << Qt::endl;
    ts << "# " + tr("Sensors: identification orientation") << Qt::endl ;
    int numSen = data_sen.size() - 2;
    for(int i = 0; i < numSen; i++)
        ts << "# " + data_sen[i + 2] << Qt::endl;
}

void SensorsRecorder::initChannelsText(QTextStream& ts)
{
    // asi para que no haga nada, se hace desde RobotMonitorsTechnician::constructLeds() en initRecordText()
    ts << Qt::endl;
}
