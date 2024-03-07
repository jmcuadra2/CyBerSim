/***************************************************************************
                          exports2fig.h  -  description
                             -------------------
    begin                : Thu Apr 14 2005
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

#ifndef EXPORTS2FIG_H
#define EXPORTS2FIG_H

#include <QObject>
#include <QTextStream>
#include <QFile>


/**
  *@author Jose M. Cuadra Troncoso
  */

class Exports2Fig : public QObject{
  
  Q_OBJECT

  public:
    enum ExportModes {ToFig, ToAscii, ToSvg};
    
  protected:
    Exports2Fig(QObject* parent = 0);
    void messageNoPath(void);
    
  public:
    ~Exports2Fig();
    static Exports2Fig* instance(void);
    virtual void exportPath(QTextStream &ts_dat, QTextStream &ts_fig, ExportModes exportMode = ToFig);

  protected:
    virtual void exportToFig(QTextStream &ts_dat, QTextStream &ts_fig, QString line = "");
    virtual void exportToAscii(QTextStream &ts_dat, QTextStream &ts_new, QString line = "");
    virtual void exportToSvg(QTextStream &ts_dat, QTextStream &ts_new, QString line = "");

  protected:
    static Exports2Fig* ex_this;
    int worldWidth, worldHeight, toFigUnits;
    bool inverseY;
    double world_scale, robot_scale, worldXMin, worldYMin;
    qint64 prevFilePos;
    bool robot_or_ac;
   
};

#endif
