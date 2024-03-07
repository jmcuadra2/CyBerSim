/***************************************************************************
                          boxmonitor.cpp  -  description
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

#include "boxmonitor.h"
#include "graphicmonitor.h"
#include "../neuraldis/windowoperations.h"

#include <QColor>
#include <QMessageBox>
#include <QLayout>
#include <QMdiSubWindow>

#include <QGridLayout>
#include <QCloseEvent>
#include <QList>
#include <QVector>

#define N_ROWS    4

BoxMonitor::BoxMonitor(const QDomElement& e,
                       QWidget *parent, const char *name)
    : QWidget(parent)
{

    bool ok;
    num_id = e.attribute("num_id").toInt(&ok);
    titulo = e.attribute("caption");
    color_paper = bool(e.attribute("back_color").toInt(&ok));
    setWindowTitle(titulo);
    if(!color_paper)
    {
        fore_color = QColor("#ffe000");
        back_color = QColor(Qt::black);
    }
    else
    {
        fore_color = QColor(Qt::black);
        back_color = QColor(Qt::white);
    }
    gmons.clear();

}  

BoxMonitor::BoxMonitor(int id,  QString cap, bool color_pap, QWidget *parent, const char *name)
    : QWidget(parent)
{

    num_id = id;
    color_paper = color_pap;
    titulo = cap;
    setWindowTitle(cap);
    if(!color_paper) {
        fore_color = QColor("#ffe000");
        back_color = QColor(Qt::black);
    }
    else {
        fore_color = QColor(Qt::black);
        back_color = QColor(Qt::white);
    }
    gmons.clear();

}

BoxMonitor::~BoxMonitor()
{
    if(parent())
        setParent(0);
}

bool BoxMonitor::init(void)
{

    GraphicMonitor *mon;
    bool ret = true;
    int nrows = N_ROWS;
    int ncols, i, j;
    int n_graphs = 0;
    int width = 0;
    int total_height = 0;
    int total_width = 0;
    int height = 0;
    int graph_width = 0;
    int graph_height = 0;
    int g_cnt = 0;

    QListIterator<GraphicMonitor *> it(gmons);
    n_graphs = gmons.count();
    if(n_graphs < 1 || n_graphs > 12)
    {
//        QMessageBox::information(nullptr, tr("Monitor"),
//                                 tr("Invalid number of monitors"), tr("&Ok"));
        QMessageBox::information(nullptr, tr("Monitor"),
                                 tr("Invalid number of monitors"), QMessageBox::Ok);
        ret = false;
    }
    else
    {
        if(n_graphs < N_ROWS)
        {
            nrows = n_graphs;
            ncols = 1;
        }
        else
            ncols = (n_graphs - 1)/nrows + 1;

        i = 0;
        j = 0;
        QGridLayout* p_layout = new QGridLayout(this);
        while(it.hasNext()) {
            mon = it.next();
            ++g_cnt;
            graph_width = mon->width();
            graph_height = mon->height();
            if(width < graph_width)
                width = graph_width;
            height += graph_height;
            p_layout->addWidget(mon, j, i);
            ++j;
            if(!(j %= nrows)) {
                total_width += width;
                width = 0;
                if(total_height < height)
                    total_height = height;
                height = 0;
                ++i;
            }
        }
        resize(total_width, total_height);
        setMinimumSize(total_width, total_height);
        show();
        resizeMdi(QSize(total_width, total_height));
    }

    return ret;

}

void BoxMonitor::closeEvent(QCloseEvent* e)
{

    if(parent()) {
        emit closeNumId(num_id);
        e->ignore();
    }

}

void BoxMonitor::reset(void)
{
    QListIterator<GraphicMonitor *> it(gmons);
    GraphicMonitor *gmon;
    while(it.hasNext()){
        gmon = it.next();
        gmon->reset();
    }
}

void BoxMonitor::addMon(GraphicMonitor *gmon)
{  
    gmons.append(gmon);
}

void BoxMonitor::delMon(GraphicMonitor *gmon)
{

    if(!gmons.isEmpty()) {
        int index_gmon = gmons.indexOf(gmon);
        if(index_gmon != -1)
            gmons.removeAt(index_gmon);
    }

}

void BoxMonitor::write(QDomDocument& doc, QDomElement& e)
{

    WindowOperations* w_oper = WindowOperations::instance();
    QListIterator<GraphicMonitor *> it(gmons);
    GraphicMonitor *gmon;
    QDomElement tag_cons = doc.createElement( "constructor" );
    e.appendChild(tag_cons);
    tag_cons.setAttribute("num_id", QString::number(num_id));
    tag_cons.setAttribute("caption", titulo);
    tag_cons.setAttribute("back_color", color_paper);

    QDomElement tag_geom = doc.createElement( "geometry" );
    e.appendChild(tag_geom);
    QWidget* w = this;
    w_oper->writePosSize(w, tag_geom);
    
    QDomElement tag_mons = doc.createElement( "Monitors" );
    e.appendChild(tag_mons);

    while(it.hasNext()) {
        gmon = it.next();
        QDomElement tag_gmon = doc.createElement( "Monitor" );
        tag_mons.appendChild(tag_gmon);
        gmon->write(doc, tag_gmon);
    }

}

void BoxMonitor::setNumId(int n_id)
{    
    num_id = n_id;
}

void BoxMonitor::setBackColor(bool b_color)
{    
    color_paper = b_color;
}

int BoxMonitor::getNumId(void)
{   
    return num_id;
}

bool BoxMonitor::getBackColor(void)
{   
    return color_paper;
}
