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
#ifndef SLAMPAINTER_H
#define SLAMPAINTER_H

#include <utility>
#include <QPainter>
#include <QPolygonF>
#include <QGraphicsScene>
#include <QGraphicsView>

using namespace std;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class SlamPainter : public QGraphicsView
{
    Q_OBJECT
  public:
    SlamPainter();

    virtual ~SlamPainter();

    void paintSlam(QVector< pair<QPointF,QPointF> > endPoints, QVector<int> segSizes, int sizePoint = 5);
    
    void setParams(double minx, double miny, double width, double height, int maxSensorVal, double robot_x = 0, double robot_y = 0, double robot_th = 0);
    void setRotation(double rotation);

  protected:
//     void paintEvent(QPaintEvent * event);
//     void paintOnDevice(QPaintDevice* device); 
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent (QWheelEvent * e); 
    void setZoom(int delta);    

  protected:
    QVector< pair<int, pair<QPointF,QPointF> > > endPoints;    
    int numSegments;
    QPainter p;
    double offsetX, offsetY, scale;
    
    double minx;
    double miny;
    double maxx;
    double maxy;
    int maxSensorVal;
    bool erase;
    int sizePoint;
    double zoom;
//    QPoint clickPoint;
    QPointF clickPoint;
    QVector<int> segSizes;
    int countColor;
    int totalColors;
    QVector<QColor> colors;
    double rotation;
    
    QGraphicsScene scene;
    
    QPointF offset;

};

#endif
