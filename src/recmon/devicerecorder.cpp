/***************************************************************************
                          devicerecorder.cpp  -  description
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

#include "devicerecorder.h"
#include "../neuraldis/xmloperator.h"

#include <QStringList>

#define INPUT     -1  // Para las señales
#define OUT       -2
#define OUTDIFF   -3

DeviceRecorder::DeviceRecorder(const QDomElement& e, QObject *parent, const char *name)
            : FileRecorder(e, parent, name)
{
  type = NEURON_RECORDER;
  recording = true;  //  provisional
}                          

DeviceRecorder::DeviceRecorder(int id, QObject *parent, const char *name)
      : FileRecorder(id, parent, name)
{

  type = NEURON_RECORDER;
  filename = "";
  n_channels = 3;
  channels_ids.fill(0, n_channels);
  channels_ids[0] = INPUT;
  channels_ids[1] = OUT;
  channels_ids[2] = OUTDIFF;
  init_crono = 0;
  crono = init_crono;    
  crono_reset = false;
  really_append = true;
  setFileMode(AUTO);     
  reset(true);
  recording = true;  //  provisional

}

DeviceRecorder::DeviceRecorder(int id, QString const& fname, int f_mode,
                               QObject *parent, const char *name)
                : FileRecorder(id, fname, f_mode, parent, name)
{
  type = NEURON_RECORDER;
  recording = true;  //  provisional
}                                                                                  

DeviceRecorder::DeviceRecorder(int id, QString const& fname,
                          const QDomElement& e, int f_mode,
                          QObject *parent, const char *name)
    : FileRecorder(id, fname, e, f_mode, parent, name)
{
  type = NEURON_RECORDER;
  recording = true;  //  provisional
}  

DeviceRecorder::DeviceRecorder(int id, QString const& fname,
                         QVector<int> channels, int f_mode,
                         QObject *parent, const char *name)
    : FileRecorder(id, fname, channels, f_mode, parent, name)
{
  type = NEURON_RECORDER;
  recording = true;  //  provisional
}

void DeviceRecorder::initRecordText(QTextStream& ts, QStringList& data)
{

  ts << "# " + tr("Net name:") << " " << data[0] << "\n";
  ts << "# " + tr("Control file name:") << " "  << data[1] << "\n";
  ts << "# " + tr("ISO device") << " "  << num_id << ": "  ;
  ts << tr("Input, output and its first difference") << "\n";
  ts << "# " + tr("Resonators:") << " " ;

}  
