//
// C++ Interface: sensorsrecorder
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SENSORSRECORDER_H
#define SENSORSRECORDER_H



#include "../recmon/filerecorder.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class SensorsRecorder : public FileRecorder
{
public:
    SensorsRecorder(int id, const QString& fname, int f_mode, QObject* parent = 0,
                    const char* name = 0);

    SensorsRecorder(int id, const QString& fname, QVector< int > channels, int f_mode,
                    QObject* parent = 0, const char* name = 0);

    SensorsRecorder(int id, const QString& fname, const QDomElement& e, int f_mode,
                    QObject* parent = 0, const char* name = 0);

    SensorsRecorder(int id, QObject* parent = 0, const char* name = 0);

    SensorsRecorder(const QDomElement& e, QObject* parent = 0, const char* name = 0);

    ~SensorsRecorder();

protected:
    void initRecordText(QTextStream& ts, QStringList& data);
    void initChannelsText(QTextStream& ts);

};

#endif
