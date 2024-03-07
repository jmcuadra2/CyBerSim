/***************************************************************************
                          robotmonitor.h  -  description
                             -------------------
    begin                : Thu Mar 3 2005
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

#ifndef ROBOTLEDMONITOR_H
#define ROBOTLEDMONITOR_H

#include <QWidget>
#include <QDomDocument>

#include "gui/irsmonitor.h"
#include "gui/lightmonitor.h"
#include "../neuraldis/mdisubwininterface.h"


class RobotMonitorsTechnician;
class QCloseEvent;

/*! \ingroup robot_monitor
    \brief Caja de monitores digitales de los sensores de proximidad,
    luz y colision, asi como, velocimetro de ambas ruedas y contador de
    colisiones.
  */

class RobotLedMonitor : public QWidget , public MdiSubWinInterface
{

    Q_OBJECT

    friend class RobotMonitorsTechnician;
public:
    RobotLedMonitor(QWidget* w_parent = 0, const char* name = 0);
    ~RobotLedMonitor();
    void resetBump(void);
    void write(QDomDocument& doc, QDomElement& e);

protected:
    template<class LM>
    void writeLED(LM* monitor, QDomDocument& doc, QDomElement& e);
    void closeEvent(QCloseEvent* e);

signals:
    void closeLed(int);

private:
    IRSMonitor* irsmonitor;
    lightMonitor* lightmonitor;

};

#endif
