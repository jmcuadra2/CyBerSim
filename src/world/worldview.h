/***************************************************************************
                          worldview.h  -  description
                             -------------------
    begin                : Tue Nov 11 2003
    copyright            : (C) 2003 by Jose M. Cuadra Troncoso
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

#ifndef WORLDVIEW_H
#define WORLDVIEW_H

#include <QWidget>
#include <QStringList>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>

#include "../neuraldis/mdisubwininterface.h"

class WorldView : public QGraphicsView , public MdiSubWinInterface
{

    Q_OBJECT

    friend class AbstractWorld;
    friend class World;
    friend class NDAriaWorld;

public:
    const QString& getXFigColor(uint id) { return xfig_colors[id] ; }
    void restoreInitialZoom(void);
    int findFigColor(const QString& color);

    void resizeView(int width, int height);
    void setWorldOrigin(QPointF worldOrigin) { this->worldOrigin = worldOrigin;  }

protected:
    WorldView(QGraphicsScene *w_canvas = 0, QWidget *parent = 0);
    ~WorldView();
    
    void resizeToCanvas(void);
    void xfigColors(void);
    void fitInWindow(void);
    
    void wheelEvent (QWheelEvent * e);
    void setZoom(int delta);
    void setZoom(double scale);
    double getZoom(void) { return zoom ; }
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);

    void clear(void);

    void closeEvent(QCloseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    QMouseEvent transformCoordinates(QMouseEvent *e);
    void updateDragRect(const QPoint& point1, const QPoint& point2);
    void centerInRobot(bool centeredInRobot) { this->centeredInRobot = centeredInRobot ; }
    void rotate(double ang);
    double getRotation() const { return rotation; }
    void resizeCanvas(double width, double height);
    
signals:
    void closeWorld(void);
    void mouseEventToRobot(QMouseEvent *e);
    void keyToRobot(QKeyEvent* key_event, bool key_on);
    void emitDragRect(QGraphicsRectItem* const );

protected:
    QStringList xfig_colors;
    double zoom, rotation, initZoom;
    QPoint dragStartPosition;
    QGraphicsRectItem dragRect;
    QGraphicsScene *mw_canvas;
    bool isFilmed, centeredInRobot;
    int prevXCenter, prevYCenter;
    QPointF worldOrigin;

};

#endif
