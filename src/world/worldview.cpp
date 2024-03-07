/***************************************************************************
                          worldview.cpp  -  description
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

#include <QTransform>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QCloseEvent>
#include <QApplication>
#include <QtGui>
#include <QHBoxLayout>
#include <QMdiSubWindow>

#include "worldview.h"
#include "rtti.h"
#include "solidobject.h"

#include "../neuraldis/ndmath.h"

WorldView::WorldView(QGraphicsScene *w_canvas , QWidget *parent )
    : QGraphicsView( w_canvas, parent), dragRect(QRect(0, 0, 1, 1))
    
{
    setAttribute(Qt::WA_DeleteOnClose);

    new QHBoxLayout(this);
    setBackgroundBrush(Qt::white);

    mw_canvas = w_canvas;
    setFocusPolicy (Qt::ClickFocus);
    setWindowTitle(tr("World"));
    zoom = 1.0;

    dragRect.setVisible(false);
    dragRect.setZValue(5000);
    QPen pen(Qt::DashLine);
    pen.setColor(QColor(Qt::magenta));
    pen.setWidth(2);
    dragRect.setPen(pen);
    dragRect.setBrush(Qt::NoBrush);
    w_canvas->addItem(&dragRect);

    isFilmed = false;
    centeredInRobot = false;
    rotation = 0;
    initZoom = 1;

}

WorldView::~WorldView()
{
    mw_canvas->removeItem(&dragRect);
    xfig_colors.clear();

}

void WorldView::closeEvent(QCloseEvent *e)
{
    emit closeWorld();
    e->ignore();

}

QMouseEvent WorldView::transformCoordinates(QMouseEvent *e)
{
    return QMouseEvent(e->type(), mapToScene(e->pos()),
                       e->globalPosition(), e->button(), e->buttons(), e->modifiers());
}

void WorldView::updateDragRect(const QPoint& point1, const QPoint& point2)
{
    QRect rect(mapToScene(point1).toPoint(), mapToScene(point2).toPoint());
    rect = rect.normalized();
    dragRect.setRect(rect);

    scene()->update();
}

void WorldView::mouseDoubleClickEvent(QMouseEvent *event)
{

    QMouseEvent e_trans = transformCoordinates(event);

    int margin = 3; // Margen en píxeles para detectar la figura doblemente  pulsada

    QRect boundRect = QRect(QPoint(e_trans.position().toPoint().x() - margin, e_trans.position().toPoint().y() - margin),
                            QPoint(e_trans.position().toPoint().x() + margin, e_trans.position().toPoint().y() + margin));

    QList<SolidObject*> reached_objects;

    // Obtenemos los objetos que colisionan con nuestro rectángulo
    reached_objects.clear();
    QList<QGraphicsItem*> collisions = mw_canvas->items(boundRect);
    QList<QGraphicsItem*>::Iterator it = collisions.begin();
    if(event->button() == Qt::RightButton) {
        for (; it != collisions.end(); ++it) {
            QGraphicsItem* hit = *it;
            if ((hit->type() > FIRST_COLLISION_RTTI) && (hit->type() < LAST_COLLISION_RTTI)) { // los define de los rtti están en world/rtti.h
                SolidObject* solid_object = dynamic_cast<SolidObject*> (hit);
                reached_objects.append(solid_object);
            }
        }
    }
    if(!reached_objects.isEmpty()) {
        SolidObject* object = reached_objects.first();
        if(object)
            object->editMaterials();
    }
    else {
        // new 04/04/2021 && dragRect.isVisible()
        if(dragRect.rect().contains(e_trans.pos()) && dragRect.isVisible())
            emit emitDragRect(&dragRect);
        else {
            if(dragRect.isVisible())
                dragRect.setVisible(false);
            emit mouseEventToRobot(&e_trans);
        }
        scene()->update();
    }

}

void WorldView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        dragStartPosition = e->pos();
    else {
        if(dragRect.isVisible())
            dragRect.setVisible(false);
        QMouseEvent e_trans = transformCoordinates(e);
        emit mouseEventToRobot(&e_trans);
        scene()->update();
    }

}

void WorldView::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        if(dragRect.isVisible())
            dragRect.setVisible(false);
        return;
    }
    QPoint point = event->pos();
    if ((point - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    updateDragRect(dragStartPosition, point);
    if(!dragRect.isVisible())
        dragRect.setVisible(true);
}

void WorldView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        if(dragRect.isVisible())
            dragRect.setVisible(false);
        return;
    }
    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;
    QPoint dragEndPosition = event->pos();
    updateDragRect(dragStartPosition, dragEndPosition);
}

void WorldView::keyPressEvent(QKeyEvent * event)
{
    int key_event = event->key();
    switch(key_event) {
    case Qt::Key_Home :
    case Qt::Key_Up :
    case Qt::Key_Down :
    case Qt::Key_Left :
    case Qt::Key_Right :
    case Qt::Key_PageUp :
    case Qt::Key_PageDown :
    case Qt::Key_A :
    case Qt::Key_Z :
    case Qt::Key_Space :
    case Qt::Key_V:
    case Qt::Key_R :
    case Qt::Key_S :
        emit keyToRobot(event, true);
        event->accept();
        break;
    default:
        event->ignore();
    }
}

void WorldView::keyReleaseEvent(QKeyEvent * event)
{
    int key_event = event->key();
    switch(key_event) {
    case Qt::Key_Up :
    case Qt::Key_Down :
    case Qt::Key_Right :
    case Qt::Key_Left :
    case Qt::Key_PageUp :
    case Qt::Key_PageDown :
    case Qt::Key_A :
    case Qt::Key_Z :
        emit keyToRobot(event, false);
        event->accept();
        break;
    default:
        event->ignore();
    }
}

void WorldView::resizeView(int width, int height)
{
    resize(width, height);
    resizeMdi(QSize(width, height));
    QSize s = size();
    QSize ss = getMdiSubWindow()->size();
    resizeMdi(2*ss - s);
}

void WorldView::resizeToCanvas(void)
{
    QSize s = sizeHint();
    resize(s);
    resizeMdi(s);
}

void WorldView::fitInWindow(void)
{
    double scale_x = (double) width()/scene()->width();
    double scale_y = (double)height()/scene()->height();
    double scale = scale_x < scale_y ? scale_x : scale_y;
    setZoom(scale);
    initZoom = zoom;
    zoom = 1;
}

void WorldView::restoreInitialZoom(void)
{
    setZoom(initZoom/zoom);
}

void WorldView::setZoom(int delta)
{
    double zoom_rate = 1 + delta/120.0/25; // 120 punto de rueda en Qt
    if(zoom_rate) {
        zoom *= zoom_rate;
        QTransform m = transform();
        m.scale( zoom_rate, zoom_rate );
        m = QTransform(m.m11(), m.m12(), m.m21(), m.m22(), m.dx()*zoom_rate, m.dy()*zoom_rate);
        setTransform(m);
    }
}

void WorldView::setZoom(double scale)
{
    int delta_wheel = NDMath::roundInt((scale - 1)*120*25);
    setZoom(delta_wheel);
}

void WorldView::wheelEvent ( QWheelEvent * e )
{
    setZoom(e->angleDelta().y());
    e->accept();
}

void WorldView::rotate(double ang)
{
    rotation += ang;
    QRectF canvasTransDims(0, 0, mw_canvas->width()*zoom, mw_canvas->height()*zoom);
    QTransform m = transform();
    m.rotate(ang);

    canvasTransDims = m.mapRect(canvasTransDims); // bounding rect
    m = QTransform(m.m11(), m.m12(), m.m21(), m.m22(),
                -canvasTransDims.x() + m.dx(), -canvasTransDims.y() + m.dy());
    setTransform(m);
}

void WorldView::resizeCanvas(double width, double height)
{
    mw_canvas->setSceneRect(mw_canvas->sceneRect().x(), mw_canvas->sceneRect().y(), NDMath::roundInt(width), NDMath::roundInt(height));
}

void WorldView::clear(void)
{
    QList<QGraphicsItem*> list = scene()->items();
    QList<QGraphicsItem*>::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
        QGraphicsItem *hit = *it;
        if ((hit->type() != SPOINT_RTTI) && (hit->type() != ROBOT_RTTI) && (hit != &dragRect))
            delete hit;
    }
    update();
}

int WorldView::findFigColor(const QString& color)
{
    return xfig_colors.indexOf(color.toLower());
}

void WorldView::xfigColors(void)
{

    xfig_colors.append("#000000");
    xfig_colors.append("#0000ff");
    xfig_colors.append("#00ff00");
    xfig_colors.append("#00ffff");
    xfig_colors.append("#ff0000");
    xfig_colors.append("#ff00ff");
    xfig_colors.append("#ffff00");
    xfig_colors.append("#ffffff");
    xfig_colors.append("#000090");
    xfig_colors.append("#0000b0");
    xfig_colors.append("#0000d0");
    xfig_colors.append("#87ceff");
    xfig_colors.append("#009000");
    xfig_colors.append("#00b000");
    xfig_colors.append("#00d000");
    xfig_colors.append("#009090");
    xfig_colors.append("#00b0b0");
    xfig_colors.append("#00d0d0");
    xfig_colors.append("#900000");
    xfig_colors.append("#b00000");
    xfig_colors.append("#d00000");
    xfig_colors.append("#900090");
    xfig_colors.append("#b000b0");
    xfig_colors.append("#d000d0");
    xfig_colors.append("#803000");
    xfig_colors.append("#a04000");
    xfig_colors.append("#c06000");
    xfig_colors.append("#ff8080");
    xfig_colors.append("#ffa0a0");
    xfig_colors.append("#ffc0c0");
    xfig_colors.append("#ffe0e0");
    xfig_colors.append("#ffd700");
}
