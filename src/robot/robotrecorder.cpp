/***************************************************************************
                          robotrecorder.cpp  -  description
                             -------------------
    begin                : Tue Mar 8 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
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

#include "robotrecorder.h"
#include <QStringList>

RobotRecorder::RobotRecorder(int id, const QString &fname,
                const QDomElement& e, int f_mode,
                QObject *parent, const char *name)
        : FileRecorder(id, fname, e, f_mode, parent, name)
{

  type = ROBOT_RECORDER;
  recording = true;  //  provisional
  
}

RobotRecorder::RobotRecorder(const QString &fname)
        : FileRecorder(1, fname, APPEND)
{

  type = ROBOT_RECORDER;
  for(int i = 0; i < 7; i++)
    addChannel(i + 1);
  reset();
  recording = true;  //  provisional
    
}

RobotRecorder::~RobotRecorder()
{
}

void RobotRecorder::initRecordText(QTextStream& ts, QStringList& data)
{

    figFileHeader(ts, data);

    ts << "#Robot\n";
    ts <<  "# " + tr("T(s)") << "\t" << tr("X(cm)") << "\t"
     << tr("Y(cm)") << "\t" << tr("Rot(dg)") << "\t"
     << tr("VLin(cm/s)") << "\t" << tr("VRad(dg/s):")
     << "\n" << "#\t0, ";  // 0 para la columa T(s)

}
