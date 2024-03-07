/***************************************************************************
                          robotrecorder.h  -  description
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

#ifndef ROBOTRECORDER_H
#define ROBOTRECORDER_H


#include <QTextStream>

#include "../recmon/filerecorder.h"


/*! \ingroup robot_monitor
    \brief Graba a fichero de las velocidades lineal y radial del robot,
    las coordenadas x e y de la posicion del robot y su rotacion con respecto
    al sistema de referencia base.
  */


class RobotRecorder : public FileRecorder  {

  Q_OBJECT
  public:
//    RobotRecorder(int id, QObject *parent = 0, const char *name = 0);
    RobotRecorder(int id, const QString &fname,
                const QDomElement& e, int f_mode = AUTO,
                QObject *parent = 0, const char *name = 0);
    RobotRecorder(const QString &fname);
    ~RobotRecorder();

  protected:
    void initRecordText(QTextStream& ts, QStringList& data);

};

#endif
