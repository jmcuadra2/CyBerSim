/***************************************************************************
                          devicerecorder.h  -  description
                             -------------------
    begin                : Tue Nov 2 2004
    copyright            : (C) 2004 by Jose M. Cuadra Troncoso
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

#ifndef DEVICERECORDER_H
#define DEVICERECORDER_H

#include "filerecorder.h"

/**
  *@author Jose M. Cuadra Troncoso
  */

class DeviceRecorder : public FileRecorder
{
    Q_OBJECT

  public:
    DeviceRecorder(const QDomElement& e,
                      QObject *parent = 0, const char *name = 0);
    DeviceRecorder(int id, QObject *parent = 0, const char *name = 0);    
    DeviceRecorder(int id, QString const& fname,
                const QDomElement& e, int f_mode = AUTO,
                QObject *parent = 0, const char *name = 0);                 
    DeviceRecorder(int id, QString const& fname,
                QVector<int> channels, int f_mode = AUTO,
                QObject *parent = 0, const char *name = 0);
    DeviceRecorder(int id, QString const& fname, int f_mode = AUTO,
                QObject *parent = 0, const char *name = 0);    
    ~DeviceRecorder() {}

  protected:
   virtual void initRecordText(QTextStream& ts, QStringList& data);

};

#endif
