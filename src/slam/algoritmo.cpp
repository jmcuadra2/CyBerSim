/***************************************************************************
 *   Copyright (C) 2010 by Jose Manuel Cuadra Troncoso   *
 *   jmcuadra@dia.uned.es   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "algoritmo.h"
#include <QDateTime>

const double Algoritmo::RAD2GRAD = 180.0/M_PI;

Algoritmo::Algoritmo()
{
    coord1 = 0, coord2 = 0;
    numSensors = 0, sensorsField = 1, startAngle = 0;
    verbose = false;
    QDateTime date_t= QDateTime::currentDateTime();
    QDate d = date_t.date();
    QTime t = date_t.time();
    QString filename = QString::number(d.day()) + "-" + QString::number(d.month()) + "-" + QString::number(t.hour()) + "-" + QString::number(t.minute()) + "-" +  QString::number(t.second());
    filename = "dataDebug" + filename + ".dat";
    fileDebug.setFileName(filename);
    fileDebug.open(QIODevice::ReadWrite);
    outDebug.setDevice(&fileDebug);    
}


Algoritmo::~Algoritmo()
{
  fileDebug.close();
}

void Algoritmo::setMeasures(double * coord1, double * coord2)
{
  this->coord1 = coord1;
  this->coord2 = coord2;
}

void Algoritmo::setBeamFeatures(int numSensors, int sensorsField, int startAngle)
{
  this->numSensors = numSensors;
  this->sensorsField = sensorsField;
  this->startAngle = startAngle;
}



