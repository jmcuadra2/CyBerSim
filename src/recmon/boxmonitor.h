/***************************************************************************
                          boxmonitor.h  -  description
                             -------------------
    begin                : Tue Mar 2 2004
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

#ifndef BOXMONITOR_H
#define BOXMONITOR_H

#include <QWidget>
#include <QDomDocument>
#include <QCloseEvent>
#include <QList>

#include "../neuraldis/mdisubwininterface.h"

class ISODevice;
class QColor;
class GraphicMonitor;

/*!
  *@author Jose M. Cuadra Troncoso
  */

class BoxMonitor : public QWidget , public MdiSubWinInterface
{
    Q_OBJECT

public:
    BoxMonitor(const QDomElement& e,
               QWidget *parent = 0, const char *name = 0);
    BoxMonitor(int id, QString cap, bool color_pap = true,
               QWidget *parent = 0, const char *name = 0);
    ~BoxMonitor();

public slots:
    void reset(void);

public:
    bool init(void);
    void addMon(GraphicMonitor *gmon);
    void delMon(GraphicMonitor *gmon);
    QList<GraphicMonitor *>& getGraphs(void) { return gmons ; }
    void write(QDomDocument& doc, QDomElement& e);
    virtual void edit(void) {}
    virtual void setNumId(int n_id);
    virtual void setBackColor(bool b_color);
    int getNumId(void);
    bool getBackColor(void);

signals:
    void closeNumId(int);

protected:
    void closeEvent(QCloseEvent *);

public:
    int num_id;
    QList<GraphicMonitor *> gmons;
    bool color_paper;
    QColor fore_color, back_color;

private:
    QString titulo;

};

#endif
