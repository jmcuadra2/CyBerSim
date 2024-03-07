//
// C++ Interface: accontrolrecorder
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ACCONTROLRECORDER_H
#define ACCONTROLRECORDER_H

#include <QtGui>

#include "../recmon/filerecorder.h"

/**
    @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class ACControlRecorder : public FileRecorder
{
public:
    ACControlRecorder(int id, const QString& fname, int f_mode, QObject* parent = 0,
                      const char* name = 0);
    
    ACControlRecorder(int id, const QString& fname, QVector<int> channels,
                      int f_mode, QObject* parent = 0, const char* name = 0);
    
    ACControlRecorder(int id, const QString& fname, const QDomElement& e, int f_mode,
                      QObject* parent = 0, const char* name = 0);
    
    ACControlRecorder(int id, QObject* parent = 0, const char* name = 0);
    
    ACControlRecorder(const QDomElement& e, QObject* parent = 0, const char* name = 0);

    ACControlRecorder(const QString &fname);

    ~ACControlRecorder();

protected:
    void initRecordText(QTextStream& ts, QStringList& data);

};

#endif
