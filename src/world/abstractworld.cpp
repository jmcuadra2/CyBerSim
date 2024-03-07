/***************************************************************************
                          abstractworld.cpp  -  description
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

#include <QtGui>
#include <QDir>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <qewdialogfactory.h>
#include <QtGlobal>

#include "abstractworld.h"
#include "qmessagebox.h"
#include "rectanglecollisionsolver.h"
#include "ellipsecollisionsolver.h"
#include "polygoncollisionsolver.h"
#include "segmentcollisionsolver.h"
#include "singularpointcollisionsolver.h"
#include "linecollisionsolver.h"
#include "drawings.h"
#include "material.h"
#include "quadtree.h"
#include "collisionsolver.h"

#include "worldview.h"
#include "rtti.h"
#include "mirectangle.h"
#include "miellipse.h"
#include "mipolygon.h"
#include "myline.h"
#include "gui/simfilmdialog.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/ndmath.h"
#include "../robot/sensorline.h"
#include "gui/worldscaleform.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/neuraldisdoc.h"
#include "gui/defaultmaterialsdialog.h"

AbstractWorld::AbstractWorld(QMdiArea *w_work, QObject *parent, const char *name) : QObject(parent), sources_dict()
{

    w_space = w_work;
    w_canvas = new QGraphicsScene();
    dragRect = nullptr;
    canvas_view = new WorldView(w_canvas, 0);

    connect(canvas_view, SIGNAL(emitDragRect(QGraphicsRectItem* const )), this, SLOT(setDragRect(QGraphicsRectItem* const  )));

    canvas_view->setMaximumSize(1600, 1000);
    canvas_view->move(0,0);

    QMdiSubWindow* subw = w_work->addSubWindow(canvas_view);
    canvas_view->setMdiSubWindow(subw);

    world_empty = true;
    world_scale = 1;
    x_min_position = 0.0;
    y_min_position = 0.0;
    x_max_position = 1000.0;
    y_max_position = 1000.0;
    width = 1000.0;
    height = 1000.0;
    margin = 0;
    sceneMargin = 100;
    x_robot_home = 0;
    y_robot_home = 0;

    reloading = false;
    materialsDefined = false;

    w_rand = 0;
    lambda = 0;
    w_short = 0;
    a_hit = 90;
    a_max = 90;
    transparency = 0;

    isFilmed = false;
    sequenceResolution = R_1024x768;
    sequenceResolutionValues = QPoint(1024, 768);
    sequenceFormat = PNG;
    sequenceFormatName = "PNG";
    sequenceName = tr("Untitled_sequence");
    fullSmallView = nullptr;
    embeddedFullView = false;

    xRobot = 0;
    yRobot = 0;
    prevXRobot = xRobot;
    prevYRobot = yRobot;
    dist = 0.0;
    centeredInRobot = false;

    quadtree = nullptr;
    cellSize = 50;
    quadtreeCreated = false;

    icon_goal_side = 60; // 60 cm
    c_orange = QColor("#fdaa00");
    c_black = QColor("#000000");
    c_goal = QColor("lime");
    c_home = QColor("#80f0a0");
    c_head = QColor("#0ef0f0");

}

AbstractWorld::~AbstractWorld()
{
    clearFree();
}

void AbstractWorld::clearFree(void)
{


    if(canvas_view) {
        w_space->removeSubWindow(canvas_view);
        delete canvas_view;
        canvas_view = nullptr;
    }

    if(w_canvas) {
        delete w_canvas;
        w_canvas = nullptr;
    }

    if(quadtree) {
        delete quadtree;
        quadtree = nullptr;
        quadtreeCreated = false;
    }
}

void AbstractWorld::clear(void)
{
    canvas_view->clear();
    sources_dict.clear();
    solid_objects.clear();
    sensorLines.clear();
    freeAreaRibbing.clear();
    emit worldCleared();
    world_empty = true;
    if(fullSmallView)
        delete fullSmallView;
    fullSmallView = nullptr;

}

QGraphicsScene * AbstractWorld::getCanvas(void )
{
    return w_canvas;
}

bool AbstractWorld::isEmpty(void)
{
    return world_empty;
}

QMdiArea * AbstractWorld::getWorkspace(void)
{
    return w_space;
}

WorldView * AbstractWorld::view()
{
    return canvas_view;
}

void AbstractWorld::zoomOut(void)
{

    canvas_view->setZoom(.8);

    if(centeredInRobot) {
        if(robotItems.count() > 1) {
            for(int i = 0; i < robotItems.count(); i++)
                canvas_view->ensureVisible(robotItems.at(i));
        }
        else if(robotItems.count() == 1) {
            canvas_view->centerOn(robotItems.at(0));
        }
    }

}

void AbstractWorld::zoomIn(void)
{

    canvas_view->setZoom(1.25);

    if(centeredInRobot) {
        if(robotItems.count() > 1) {
            for(int i = 0; i < robotItems.count(); i++)
                canvas_view->ensureVisible(robotItems.at(i));
        }
        else if(robotItems.count() == 1) {
            canvas_view->centerOn(robotItems.at(0));
        }
    }

}

void AbstractWorld::restoreInitialZoom(void)
{
    canvas_view->restoreInitialZoom();

    if(centeredInRobot) {
        if(robotItems.count() > 1) {
            for(int i = 0; i < robotItems.count(); i++)
                canvas_view->ensureVisible(robotItems.at(i));
        }
        else if(robotItems.count() == 1) {
            canvas_view->centerOn(robotItems.at(0));
        }
    }
}

void AbstractWorld::rotateView(void)
{
    canvas_view->rotate(/*headingTransform()**/-90);
}

QGraphicsLineItem* AbstractWorld::drawLine(const int& x_min_pos, const int& y_min_pos, const int& x_max_pos, const int& y_max_pos,  QColor color, const bool& line_obstacle)

{

    QGraphicsLineItem* canvas_line =  0;

    if(line_obstacle) {
        canvas_line = new MyLine();
        canvas_line->setData(1, "MyLine");
    }

    else {
        canvas_line = new QGraphicsLineItem();
        canvas_line->setData(1, "Other Line");
    }

    w_canvas->addItem(canvas_line);
    canvas_line->setLine(x_min_pos, y_min_pos, x_max_pos, y_max_pos);
    canvas_line->setZValue(400);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(color);
    canvas_line->setPen(pen);
    canvas_line->show();

    return canvas_line;

}


void AbstractWorld::setRobotRadius(const double& rad)
{

    QList<QGraphicsItem*> list = w_canvas->items();
    QList<QGraphicsItem*>::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
        QGraphicsItem *hit = *it;
        if((hit->type() > FIRST_COLLISION_RTTI)  && (hit->type() < LAST_COLLISION_RTTI)) {
            SolidObject* solid_object = dynamic_cast<SolidObject*> (hit);
            solid_object->getCollisionSolver()->setRobotRadius(rad);
            //       solid_objects.append(solid_object->getCollisionSolver());
        }
    }
}

CollisionSolver* AbstractWorld::factoryCollisionSolver(const int& type, SolidObject* coll_object)
{

    CollisionSolver* solver = 0;
    QDomDocument doc_collision("collision");
    if(!coll_object)
        return solver;
    QPolygonF points = coll_object->getPoints();

    switch(type) {
    case CollisionSolver::ELLIPSE_COLL :
        if(coll_object->getRtti() == ELLIPSE_RTTI) {
            QDomElement collision_params = doc_collision.createElement("parameters");
            collision_params.setAttribute("center_x", QString::number(double(points.at(0).x())));
            collision_params.setAttribute("center_y", QString::number(double(points.at(0).y())));
            collision_params.setAttribute("radius_x", QString::number(points.at(1).x()));
            collision_params.setAttribute("radius_y", QString::number(points.at(1).y()));
            collision_params.setAttribute("angle", QString::number(points.at(2).x()));

            solver = (CollisionSolver*)new EllipseCollisionSolver();
            solver->addLista(bresenhamCalculador.BresenhamElipse(points.at(0).toPoint().x(),points.at(0).toPoint().y(), points.at(1).toPoint().x(),points.at(1).toPoint().y()));

            if(solver) {
                solver->setParameters(collision_params);
                coll_object->setCollisionSolver(solver);
                quadtree->addCollisionSolver(solver);
                //No le sumo porque eso ya lo hago en el readXML
            }
        }
        break;
    case CollisionSolver::POLYGON_COLL :
        if(coll_object->getRtti() == RECTANGLE_RTTI || coll_object->getRtti() == POLYGON_RTTI || coll_object->getRtti() == LINE_RTTI ) {

            QDomElement collision_params = doc_collision.createElement("parameters");
            QDomElement num_points = doc_collision.createElement("num_points");
            collision_params.appendChild(num_points);
            QDomText text = doc_collision.createTextNode(QString::number(points.count()));
            num_points.appendChild(text);
            for(int k = 0; k < points.count(); k++) {
                QDomElement point = doc_collision.createElement("point" + QString::number(k).rightJustified(4,'0'));
                collision_params.appendChild(point);
                point.setAttribute("x", points.at(k).x());
                point.setAttribute("y", points.at(k).y());
            }
            if(coll_object->getRtti() == LINE_RTTI) {
                solver = (CollisionSolver*)new LineCollisionSolver();
                solver->setParameters(collision_params);

                solver->addLista(bresenhamCalculador.BresenhamLine(solver->getParamsPoints().at(0).toPoint().x(), solver->getParamsPoints().at(0).toPoint().y(), solver->getParamsPoints().at(1).toPoint().x(), solver->getParamsPoints().at(1).toPoint().y(), 1.5));

            }
            else {
                solver = new PolygonCollisionSolver();
            }
            if(solver) {
                //                 solver->setParameters(collision_params);
                coll_object->setCollisionSolver(solver);
                //No le sumo porque eso ya lo hago en el readXML

                if(coll_object->getRtti() == POLYGON_RTTI || coll_object->getRtti() == RECTANGLE_RTTI) {
                    solver->setParameters(collision_params);
                    // Los elementos de la lista de orden impar son SingularPointCollisionSolver
                    // y no se usan
                    QList<CollisionSolver*> segmentSolvers = solver->getCollisionsSolver();
                    CollisionSolver* segmentSolver = nullptr;
                    QPointF startPoint, endPoint;
                    int i = 0;
                    // la primera mitad de la lista son segmentSolver
                    // la segunda singular points
                    for(; i < segmentSolvers.size() / 2 - 1; i++) { // l
                        segmentSolver = segmentSolvers.at(i);
                        startPoint = segmentSolver->getParamsPoints().at(0);
                        endPoint = segmentSolver->getParamsPoints().at(1);

                        segmentSolver->addLista(bresenhamCalculador.BresenhamLine(startPoint.toPoint().x(),startPoint.toPoint().y(), endPoint.toPoint().x(), endPoint.toPoint().y(), 1.5));
                        quadtree->addCollisionSolver(segmentSolver);
                    }
                    segmentSolver = segmentSolvers.at(i);
                    startPoint = segmentSolver->getParamsPoints().at(0);
                    endPoint = segmentSolver->getParamsPoints().at(1);
                    segmentSolver->addLista(bresenhamCalculador.BresenhamLine(startPoint.toPoint().x(),startPoint.toPoint().y(), endPoint.toPoint().x(), endPoint.toPoint().y(), 1.5));
                    quadtree->addCollisionSolver(segmentSolver);
                }
                else
                    quadtree->addCollisionSolver(solver);
            }
        }
        break;
    }
    return solver;

}

void AbstractWorld::robotPosition(int x, int y)
{
    xRobot = x;
    yRobot = y;
}

void AbstractWorld::setDragRect(QGraphicsRectItem* const rect)
{
    dragRect = rect;
    QList<QGraphicsItem*> items = selectedItems(DRAWING_RTTI);
    QList<int> drawingsId;

    for(QList<QGraphicsItem*>::Iterator it=items.begin();
        it!=items.end(); ++it) {
        QGraphicsItem* drawing = *it;
        if(sensorLines.contains(drawing)) {
            // por jose para world-new
            if(sensorLines[drawing]->isUserVisible())
                drawingsId.append(sensorLines[drawing]->getIdent());
            //       qDebug() << sensorLines[drawing]->getIdent();
        }
    }
    dragRect->setVisible(false);
    emit drawingsIdent(drawingsId);
}

QList<QGraphicsItem*> AbstractWorld::selectedItems(int rtti)
{
    QList<QGraphicsItem*> items;
    QList<QGraphicsItem*> l = dragRect->collidingItems(Qt::IntersectsItemBoundingRect);

    for(QList<QGraphicsItem*>::Iterator it=l.begin(); it!=l.end(); ++it) {
        QGraphicsItem *hit = *it;
        if(hit->type() == rtti) {
            if(dragRect->collidesWithItem(hit))
                items.append(hit);
        }
    }
    return items ;
}

void AbstractWorld::addSensorLine(SensorLine* const sensorLine, QGraphicsItem* const drawing)
{
    // por jose para world-new
    if(!sensorLines.contains(drawing)) {
        sensorLines[drawing] = sensorLine;
    }
}

void AbstractWorld::addFreeAreaRib(SensorLine* const sensorLine)
{
    // por jose para world-new
    freeAreaRibbing.append(sensorLine);
}

void AbstractWorld::changeRibColor(int lineIdx, double value)  // out = false --> mainColor
{
    // por jose para world-new
    freeAreaRibbing[lineIdx]->saturateColor(value);
}

void AbstractWorld::sendSources(void)
{
    emit worldSources(sources_dict);
}

void AbstractWorld::scaleByUser(void)
{
    WorldScaleForm* dialog = new WorldScaleForm();
    dialog->setUpDialog();
    dialog->init(this);
    if(dialog) {
        if(dialog->exec() == QewExtensibleDialog::Accepted) {
            clear();
            reload();
        }
        delete dialog;
    }
}

void AbstractWorld::reload(void)
{
    reloading = true;
    QTextStream ts(&worldText, QIODevice::ReadOnly);
    load(ts);
    reloading = false;
}

void AbstractWorld::writeXML(void)

{

    QFile file_world;
    QTextStream ts;

    // XMLOperator para insertar los elementos en el documento XML
    XMLOperator* xml_operator = XMLOperator::instance();
    // Tipo del documento XML que vamos a crear
    QDomDocument doc("CyberSim_world");
    // Elemento Raiz
    QDomElement root = doc.createElement("world");

    doc.appendChild(root);

    QDomElement e_geom = doc.createElement("geometry");

    // Establecemos las coordenadas del mundo para calcular su ancho y alto

    e_geom.setAttribute("x_min_position", (x_min_position + margin)*world_scale);
    e_geom.setAttribute("x_max_position", (x_max_position - margin)*world_scale);
    e_geom.setAttribute("y_min_position", (y_min_position + margin)*world_scale);
    e_geom.setAttribute("y_max_position", (y_max_position - margin)*world_scale);
    root.appendChild(e_geom);

    // Lista de líneas que tendrá el mundo
    QDomElement e_elems_list = doc.createElement("Elements_list");
    QList<QGraphicsItem*> list = w_canvas->items();
    QList<QGraphicsItem*>::Iterator it = list.begin();

    for (; it != list.end(); ++it) {
        QGraphicsItem* hit = *it;
        if (((hit->type() > FIRST_COLLISION_RTTI)  && (hit->type() < LAST_COLLISION_RTTI)) || hit->type() == RADIALFIELDSOURCE_RTTI/* || hit->data(1) == "Goal"*/) {
            // los define de los rtti están en world/rtti.h
            QDomElement e_tag_line = doc.createElement("Element");
            e_elems_list.appendChild(e_tag_line);
            SolidObject* solid_object;
            QPolygonF points;
            if(hit->type() == RADIALFIELDSOURCE_RTTI) {
                RadialFieldSource* source = dynamic_cast<RadialFieldSource*> (hit);

                e_tag_line.appendChild(xml_operator->createTextElement(doc, "type", "RadialFieldSource"));
                e_tag_line.appendChild(xml_operator->createTextElement(doc, "nameId", source->getNameId()));

                points.append(QPoint(NDMath::roundInt(hit->x()), NDMath::roundInt(hit->y())));
            }
            else {
                solid_object = dynamic_cast<SolidObject*> (hit);
                e_tag_line.appendChild(xml_operator->createTextElement(doc, "type", solid_object->getTypeName()));
                e_tag_line.appendChild(xml_operator->createTextElement(doc, "nameId", solid_object->getIdName()));
                points = solid_object->getPoints();
            }

            QDomElement e_graph = doc.createElement("graphics");
            e_tag_line.appendChild(e_graph);
            e_graph.setAttribute("depth", QString::number(hit->zValue()));
            QGraphicsItem* item = dynamic_cast<QAbstractGraphicsShapeItem*>(hit);
            if(item) {
                QBrush b(dynamic_cast<QAbstractGraphicsShapeItem*>(item)->brush());
                QColor fill_color = b.color();
                e_graph.setAttribute("fill_color", fill_color.name());
                QPen p(dynamic_cast<QAbstractGraphicsShapeItem*>(item)->pen());
                QColor pen_color = p.color();
                e_graph.setAttribute("pen_color", pen_color.name());
            }
            else {
                item = dynamic_cast<QGraphicsLineItem*>(hit);
                if(item) {
                    QPen p(dynamic_cast<QGraphicsLineItem*>(item)->pen());
                    QColor pen_color = p.color();
                    e_graph.setAttribute("pen_color", pen_color.name());
                }
            }
            if(item) {
                // Posicion
                QDomElement e_points = doc.createElement("Points");
                for(int i = 0; i < points.size(); i++) {
                    QDomElement e_p = doc.createElement("point");
                    if(item->type() == ELLIPSE_RTTI) {
                        if(i==0) {
                            e_p.setAttribute("x", QString::number(NDMath::roundInt(inverseXTransform(points.at(i).x()))));
                            e_p.setAttribute("y", QString::number(NDMath::roundInt(inverseYTransform(points.at(i).y()))));
                        }
                        else if(i==2) {
                            e_p.setAttribute("x", QString::number(points.at(i).x()));
                            e_p.setAttribute("y", QString::number(points.at(i).y()));
                        }
                        else {  // para que solo escale y no lo mueva, son los radios
                            e_p.setAttribute("x", QString::number(points.at(i).x()*getWorldScale()));
                            e_p.setAttribute("y", QString::number(points.at(i).y()*getWorldScale()));
                        }
                    }
                    else {
                        e_p.setAttribute("x", QString::number(NDMath::roundInt(inverseXTransform(points.at(i).x()))));
                        e_p.setAttribute("y", QString::number(NDMath::roundInt(inverseYTransform(points.at(i).y()))));
                    }
                    e_points.appendChild(e_p);
                }
                e_tag_line.appendChild(e_points);
            }

            int r = hit->type();
            if(r == RADIALFIELDSOURCE_RTTI)
                continue;

            // Material
            QDomElement materials_list = doc.createElement("Materials");

            // Recorro los materiales del mundo
            CollisionSolver* cs = solid_object->getCollisionSolver();
            QMapIterator<int, Material*> iteratorMaterials(cs->getMaterials());
            while (iteratorMaterials.hasNext()) {
                iteratorMaterials.next();
                Material* m = iteratorMaterials.value();
                QDomElement constructor = doc.createElement("constructor");
                m->getParams(constructor);
                materials_list.appendChild(constructor);
            }

            e_tag_line.appendChild(materials_list);
            // Dynamic params
            QDomElement e_dynamic = doc.createElement("dynamic_params");
            e_dynamic.setAttribute("lambda", cs->getLambda());
            e_dynamic.setAttribute("w_short", cs->getW_short());
            e_tag_line.appendChild(e_dynamic);
        }
    }

    // Lista de líneas del mundo
    root.appendChild(e_elems_list);

    // Salvamos el fichero
    NeuralDisDoc* doc_manager = NeuralDisDoc::instance();
    QString ret_name = doc_manager->saveDocument(worldName,
                                                 doc.toString(4),tr("World XML file saving"), "world",
                                                 tr("World XML file saving"), true);
}

bool AbstractWorld::readXML(QString fileName) {

    // Cargamos un mundo a partir de un fichero .world
    bool ret = false;
    bool ok;
    int depth = 400;

    // Algunas varaibles que utilizaré mas adelante

    QColor pen_color("black");
    QColor fill_color("silver"); // #c0c0c0
    CollisionSolver* solver;
    XMLOperator* xml_operator = XMLOperator::instance();

    // Recuperamos el fichero
    NeuralDisDoc* doc_manager = NeuralDisDoc::instance();
    QDomDocument doc = doc_manager->loadXML(fileName, "", "");

    // Recorremos el fichero a través de los elementos <line>
    if(!doc.isNull()) {
        QDomNode root = doc.firstChild();
        if(!root.isNull()) {
            QDomElement e_geom = xml_operator->findTag("geometry", root.toElement());
            if(e_geom.isNull()) {
                QMessageBox::critical(nullptr, tr("Load world"),
                                      tr("Isn't a valid world file"),
                                      QMessageBox::Cancel);
                return false;
            }

            x_min_position = e_geom.attribute("x_min_position").toDouble(&ok)/world_scale - margin;
            x_max_position = e_geom.attribute("x_max_position").toDouble(&ok)/world_scale + margin;
            y_min_position = e_geom.attribute("y_min_position").toDouble(&ok)/world_scale - margin;
            y_max_position = e_geom.attribute("y_max_position").toDouble(&ok)/world_scale + margin;
            width = x_max_position - x_min_position;
            height = y_max_position - y_min_position;

            QRectF worldRect(x_min_position * world_scale, y_min_position * world_scale, width * world_scale, height * world_scale);
            setUpScene(worldRect.toRect());
            QDomElement e_elem_list = xml_operator->findTag("Elements_list", root.toElement());
            QString type;
            QDomNode n = e_elem_list.firstChild();
            while(!n.isNull()) {
                QDomElement e = n.toElement();
                if(e.isNull()) {
                    n = n.nextSibling();
                    continue;
                }

                QDomElement e_type = xml_operator->findTag("type", e);
                QDomElement e_name = xml_operator->findTag("nameId", e);
                QDomElement e_graph = xml_operator->findTag("graphics", e);
                QDomElement e_points = xml_operator->findTag("Points", e);
                QDomElement e_materials = xml_operator->findTag("Materials", e);
                QDomElement e_dynamic_params = xml_operator->findTag("dynamic_params", e);

                if(e_type.isNull() || e_points.isNull()) {
                    n = n.nextSibling();
                    continue;
                }

                type = e_type.text();
                if(type == "RadialFieldSource") {
                    QPolygonF position = getXMLPoints(e_points, type);
                    addRFSource(w_canvas, NDMath::roundInt(position.at(0).x()),
                                NDMath::roundInt(position.at(0).y()), e_name.text());
                    drawGoalIcon(NDMath::roundInt(position.at(0).x()),
                                 NDMath::roundInt(position.at(0).y()), c_goal, e_name.text(),
                                 Qt::Dense3Pattern);
                }
                else {
                    if(!e_graph.isNull()) {
                        QString name_pen_color = e_graph.attribute("pen_color");
                        pen_color.setNamedColor(name_pen_color);
                        if(!pen_color.isValid())
                            pen_color.setNamedColor("black");

                        QString name_fill_color = e_graph.attribute("fill_color");
                        fill_color.setNamedColor(name_fill_color);
                        if(!fill_color.isValid())
                            fill_color.setNamedColor("silver");

                        depth = e_graph.attribute("depth").toInt(&ok);
                        depth = ok ? depth : 400;
                    }

                    SolidObject* solid_object = factorySolidObject(w_canvas, type, getXMLPoints(e_points, type), 2,
                                                                   pen_color, fill_color, 1000 - depth, 1);
                    solid_object->setIdName(e_name.text());

                    if(type == "solid_ellipse")
                        solver = factoryCollisionSolver(CollisionSolver::ELLIPSE_COLL, solid_object);
                    else
                        solver = factoryCollisionSolver(CollisionSolver::POLYGON_COLL, solid_object);

                    if(solver) {
                        setXMLMaterials(e_materials, solver);
                        getXMLDynamics(e_dynamic_params, solver);
                        solid_objects.append(solver);
                    }
                }
                n = n.nextSibling();
            }

            emit worldLoaded();
            emit worldSources(sources_dict);

            canvas_view->fitInWindow();

            world_empty = false;
            ret = true;
        }
    }

    return ret;

}

void AbstractWorld::drawTree(QVector<QPair<QPoint, uint> > *cells)
{
    for(int i = 0; i < cells->size(); i++) {
        drawCell(cells->at(i).first, cells->at(i).second);
    }
}

void AbstractWorld::drawMovingBresenham(QList<QPoint> lineBres, Qt::GlobalColor color) 
{

    QList<QGraphicsItem*> list = w_canvas->items();
    QList<QGraphicsItem*>::Iterator it = list.begin();

    for (; it != list.end(); ++it) {
        QGraphicsItem *hit = *it;
        if(hit->data(0).toString() == "MovingBresenham")
            w_canvas->removeItem(hit);
    }

    QColor p_color = QColor(color);
    QPen pen(p_color);
    QBrush brush(Qt::NoBrush);
    QGraphicsItem *item = nullptr;
    double l_size = quadTreeRect.width()/quadCellSize;

    for(int i = 0; i < lineBres.size(); i++) {
        QPoint center(lineBres.at(i).y(), lineBres.at(i).x());
        QPointF centerF(center.x() * quadTreeRect.width()/quadCellSize, center.y() * quadTreeRect.width()/quadCellSize);

        QRectF rect;
        rect.setSize(QSize(l_size, l_size));
        rect.translate(centerF.x() + quadTreeRect.x(), centerF.y() + quadTreeRect.y());
        item = w_canvas->addRect(rect, pen, brush);
        item->setZValue(1000);
        item->setData(0, "MovingBresenham");
    }
}

void AbstractWorld::drawCell(QPoint center, uint _size, Qt::GlobalColor color)
{
    if(center == prevCellCenter)
        return;
    
    QPointF centerF(center.x() * quadTreeRect.width()/quadCellSize, center.y() * quadTreeRect.width()/quadCellSize);
    double l_size = _size * quadTreeRect.width()/quadCellSize;

    QPointF start(centerF.x() - l_size, centerF.y());
    QPointF end(centerF.x() + l_size, centerF.y());

    QLineF line(start, end);
    line.translate(quadTreeRect.x(), quadTreeRect.y());
    
    QColor p_color = QColor(color);
    QPen pen(p_color);
    QGraphicsItem *item = w_canvas->addLine(line, pen);
    quadTreeItems.append(item);
    
    item->setZValue(1000);
    item->setData(0, "Quadtree");

    start.setX(centerF.x());
    start.setY(centerF.y() - l_size);
    end.setX(centerF.x());
    end.setY(centerF.y() + l_size);

    line.setP1(start);
    line.setP2(end);
    line.translate(quadTreeRect.x(), quadTreeRect.y());
    item = w_canvas->addLine(line, pen);
    quadTreeItems.append(item);
    
    item->setZValue(1000);
    item->setData(0, "Quadtree");
    
    QColor b_color = QColor(p_color.red(), p_color.green(), p_color.blue(), 128);
    QBrush brush(b_color);
    if(_size == 1) {
        if(quadtree->getCollisionSolvers(center.y(), center.x())->size()) {
            QRectF rect;
            rect.setSize(QSize(l_size, l_size));
            rect.translate(centerF.x() + quadTreeRect.x(), centerF.y() + quadTreeRect.y());
            item = w_canvas->addRect(rect, pen, brush);
            quadTreeItems.append(item);
            item->setZValue(1000);
            item->setData(0, "Quadtree");
        }
        if(quadtree->getCollisionSolvers(center.y() - 1, center.x())->size()) {
            QRectF rect;
            rect.setSize(QSize(l_size, l_size));
            rect.translate(center.x() * quadTreeRect.width()/quadCellSize  + quadTreeRect.x(), (center.y() - 1) * quadTreeRect.width()/quadCellSize + quadTreeRect.y());
            item = w_canvas->addRect(rect, pen, brush);
            quadTreeItems.append(item);
            item->setZValue(1000);
            item->setData(0, "Quadtree");
        }
        if(quadtree->getCollisionSolvers(center.y(), center.x() - 1)->size()) {
            QRectF rect;
            rect.setSize(QSize(l_size, l_size));
            rect.translate((center.x() - 1) * quadTreeRect.width()/quadCellSize  + quadTreeRect.x(), center.y() * quadTreeRect.width()/quadCellSize + quadTreeRect.y());
            item = w_canvas->addRect(rect, pen, brush);
            quadTreeItems.append(item);
            item->setZValue(1000);
            item->setData(0, "Quadtree");
        }
        if(quadtree->getCollisionSolvers(center.y() - 1, center.x() - 1)->size()) {
            QRectF rect;
            rect.setSize(QSize(l_size, l_size));
            rect.translate((center.x() - 1) * quadTreeRect.width()/quadCellSize  + quadTreeRect.x(), (center.y() - 1) * quadTreeRect.width()/quadCellSize + quadTreeRect.y());
            item = w_canvas->addRect(rect, pen, brush);
            quadTreeItems.append(item);
            item->setZValue(1000);
            item->setData(0, "Quadtree");
        }

    }
    
    prevCellCenter = center;

}

QList< CollisionSolver* > AbstractWorld::getCollisionSolvers(void )
{

    QList<CollisionSolver*> solvers;

    if(boundRect.isNull())
        solvers = solid_objects;
    else {
        if(oldBoundRect == boundRect)
            solvers = reached_objects;
        else {
            reached_objects.clear();

            QList<QGraphicsItem*> collisions = w_canvas->items(boundRect);
            QList<QGraphicsItem*>::Iterator it = collisions.begin();
            for (; it != collisions.end(); ++it) {
                QGraphicsItem* hit = *it;
                int tipo = hit->type();
                if ((tipo > FIRST_COLLISION_RTTI)  && (tipo < LAST_COLLISION_RTTI)) { // los define de los rtti están en world/rtti.h
                    SolidObject* solid_object = dynamic_cast<SolidObject*> (hit);
                    reached_objects.append(solid_object->getCollisionSolver());
                }

            }
            solvers = reached_objects;
            oldBoundRect = boundRect;
        }
    }
    return solvers;
}

void AbstractWorld::setProbabilisticParams(double lambda, double w_short)
{

    this->lambda = lambda;
    this->w_short = w_short;

    QList<CollisionSolver*> it(solid_objects);

    for(int j=0; j<it.size(); j++) {
        it.value(j)->setW_short(w_short);
        it.value(j)->setLambda(lambda);
    }
}

void AbstractWorld::update(int crono, int update_period)
{
    if(isFilmed) {
        //QPixmap pix = QPixmap::grabWidget(canvas_view, canvas_view->rect());
        QPixmap pix = canvas_view->grab(canvas_view->rect());
        QPainter p(&pix);
        p.setPen(QColor("dimgrey"));
        QFont font;
        int pointSize = 16;
        font.setPointSize(pointSize);
        font.setStyleHint(QFont::TypeWriter);
        p.setFont(font);

        QString ms = QString::number(crono%1000).rightJustified(3,'0'); // milisegundos
        QString s = QString::number((crono/1000)%60).rightJustified(2,'0');
        QString m = QString::number((crono/60000)%60).rightJustified(2,'0');
        QString h = QString::number(crono/3600000).rightJustified(2,'0');
        QString sCrono = h + ":" + m + ":" + s + ":" + ms;
        int marginYTime = pointSize/3;
        int marginYMeters = pointSize*2;

        if(fullSmallView) {
            marginYTime += fullSmallView->height();
            marginYMeters += fullSmallView->height();
        }
        p.drawText(canvas_view->width() - pointSize*sCrono.length(), canvas_view->height() - marginYTime, sCrono);

        dist += sqrt((xRobot-prevXRobot)*(xRobot-prevXRobot) + (yRobot-prevYRobot)*(yRobot-prevYRobot));

        p.drawText(canvas_view->width() - pointSize*(7 + 1) + 2, canvas_view->height() - marginYMeters, QString::number(dist/1000*world_scale, 'f', 2).rightJustified(7,'0'));

        prevXRobot = xRobot;
        prevYRobot = yRobot;

        QImage img = pix.toImage();
        QString filename = QString::number(crono/update_period).rightJustified(6,'0');
        filename = sequenceName + QDir::toNativeSeparators("/") +
                            filename + "." + sequenceFormatName.toLower();
        bool sav = img.save(filename, sequenceFormatName.toLatin1().data());

    }

    if(centeredInRobot) {
        if(robotItems.count() > 1) {
            for(int i = 0; i < robotItems.count(); i++)
                canvas_view->ensureVisible(robotItems.at(i));
        }
        else if(robotItems.count() == 1) {
            canvas_view->centerOn(robotItems.at(0));
        }
    }
}

void AbstractWorld::centerInRobot(bool centeredInRobot)
{
    this->centeredInRobot = centeredInRobot;
    canvas_view->centerInRobot(centeredInRobot);

    if(centeredInRobot) {
        if(robotItems.count() > 1) {
            for(int i = 0; i < robotItems.count(); i++)
                canvas_view->ensureVisible(robotItems.at(i));
        }
        else if(robotItems.count() == 1) {
            canvas_view->centerOn(robotItems.at(0));
        }
    }
}

void AbstractWorld::setFilmed(bool on)
{
    if(isFilmed == on) return;

    isFilmed = on;
    if(isFilmed) {
        isFilmed = videoPreferences();
        if(isFilmed) {
            if(sequenceResolution != R_NATIVE) {
                canvas_view->resizeView(sequenceResolutionValues.x(),
                                        sequenceResolutionValues.y());
            }

            if(embeddedFullView) {
                double scaleSmallView = 0.1;

                fullSmallView = new QGraphicsView(w_canvas, canvas_view);
                fullSmallView->setBackgroundBrush(w_canvas->backgroundBrush());
                fullSmallView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                fullSmallView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

                QRectF rect_small( 0, 0 , width*scaleSmallView, height*scaleSmallView);
                QTransform m_w;
                m_w.rotate(canvas_view->getRotation());
                rect_small = m_w.mapRect(rect_small);

                fullSmallView->resize(NDMath::roundInt(rect_small.width()),
                                      NDMath::roundInt(rect_small.height()));
                fullSmallView->move(canvas_view->width() - fullSmallView->width(),
                                    canvas_view->height() - fullSmallView->height());

                QTransform m = fullSmallView->transform();
                m.rotate(canvas_view->getRotation());
                m.scale(scaleSmallView, scaleSmallView);
                m = QTransform(m.m11(), m.m12(), m.m21(), m.m22(),
                            -rect_small.x(), -rect_small.y());
                fullSmallView->setTransform(m);
                fullSmallView->show();
            }

            dist = 0.0;
            prevXRobot = xRobot;
            prevYRobot = yRobot;

            canvas_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            canvas_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    else {
        if(sequenceResolution != R_NATIVE) {
            canvas_view->resizeToCanvas();
        }
        if(fullSmallView)
            delete fullSmallView;
        fullSmallView = nullptr;

        canvas_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        canvas_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

}

AbstractWorld::SequenceResolution AbstractWorld::getSequenceResolution() const
{
    return sequenceResolution;
}

void AbstractWorld::setSequenceResolution(const SequenceResolution& resolution)
{
    sequenceResolution = resolution;
    if(sequenceResolution == R_800x600)
        sequenceResolutionValues = QPoint(800, 600);
    else if(sequenceResolution == R_1024x768)
        sequenceResolutionValues = QPoint(1024, 768);
    else if(sequenceResolution == R_NATIVE)
        sequenceResolutionValues = QPoint(NDMath::roundInt(width), NDMath::roundInt(height));
}

AbstractWorld::SequenceFormat AbstractWorld::getSequenceFormat() const
{
    return sequenceFormat;
}

void AbstractWorld::setSequenceFormat(const SequenceFormat& format)
{
    sequenceFormat = format;
    if(sequenceFormat == PNG)
        sequenceFormatName = "PNG";
    else if(sequenceFormat == JPG)
        sequenceFormatName = "JPG";
    else if(sequenceFormat == TIFF)
        sequenceFormatName = "TIFF";
    else if(sequenceFormat == BMP)
        sequenceFormatName = "BMP";
    else
        sequenceFormatName = "";
}


QString AbstractWorld::getSequenceName() const
{
    return sequenceName;
}

void AbstractWorld::setSequenceName(const QString& sequenceName)
{
    this->sequenceName = sequenceName;
}

bool AbstractWorld::videoPreferences(void)
{
    bool ret = false;

    QewDialogFactory* factory = new QewDialogFactory();
    QewExtensibleDialog* dialog = factory->createDialog(QewExtensibleDialog::QewSimpleWidget);
    if(dialog->setUpDialog()) {
        SimFilmDialog* filmDialog = new SimFilmDialog();
        if(filmDialog->setUpDialog()) {
            dialog->addExtensibleChild(filmDialog, "Film simulation");
            filmDialog->init(this);
            if(dialog->exec() == QewExtensibleDialog::Accepted)
                ret = true;
            delete dialog;
        }
        else
            delete filmDialog;
    }
    else
        delete dialog;
    return ret;
}

SolidObject* AbstractWorld::factorySolidObject(QGraphicsScene *w_canvas, int rtti, QPolygonF pa, uint thickness, QColor pen_color, QColor fill_color, float depth,  int area_fill)
{
    SolidObject* solidObject = nullptr;
    if(rtti == POLYGON_RTTI || rtti == RECTANGLE_RTTI) {
        MiPolygon *poly = new MiPolygon(pa, thickness, pen_color,fill_color, depth, area_fill);
        poly->setData(1, "MiPolygon");
        w_canvas->addItem(poly);
        solidObject = poly;
    }
    else if(rtti == LINE_RTTI) {
        MyLine* line  = new MyLine();
        line->setData(1, "MyLine");

        w_canvas->addItem(line);
        solidObject = line;
        solidObject->setPoints(pa);
        line->setZValue(1000 - depth);
        QPen pen;
        pen.setWidth(thickness);
        pen.setColor(pen_color);
        line->setPen(pen);
        line->show();
    }
    else if(rtti == ELLIPSE_RTTI) {
        MiEllipse* ellip = new MiEllipse(pa, thickness, pen_color, fill_color,  double(depth), area_fill);
        ellip->setData(1, "MiEllipse");
        w_canvas->addItem(ellip);
        ellip->setPoints(pa);
        solidObject = ellip;
    }
    return solidObject;
}

SolidObject* AbstractWorld::factorySolidObject(QGraphicsScene *w_canvas, const QString& type, QPolygonF pa, uint thickness, QColor pen_color, QColor fill_color, float depth, int area_fill)
{
    int rtti = 0;
    if(type == "solid_ellipse")
        rtti = ELLIPSE_RTTI;
    else if(type == "solid_polygon")
        rtti = POLYGON_RTTI;
    else if(type == "solid_line")
        rtti = LINE_RTTI;
    else if(type == "solid_rectangle")
        rtti = RECTANGLE_RTTI;

    return factorySolidObject(w_canvas, rtti, pa, thickness, pen_color, fill_color, depth, area_fill);

}

QPolygonF AbstractWorld::getXMLPoints(const QDomElement& e_points, const QString& type)
{
    QPolygonF points;
    bool ok;
    QDomNodeList l_points = e_points.childNodes();
    for (int j = 0; j < l_points.count(); ++j) {
        // Recorro los atributos de cada punto
        QDomNode n_point = l_points.item(j);
        if(!n_point.isNull()) {
            QDomElement e_point = n_point.toElement();
            if(!e_point.isNull()) {
                double x, y;
                if((type == "solid_ellipse")) {
                    if(j==0) {
                        x = xTransform(e_point.attribute("x").toDouble(&ok));
                        y = yTransform(e_point.attribute("y").toDouble(&ok));
                    }
                    else if(j==2) {
                        x = e_point.attribute("x").toDouble(&ok);
                        y = e_point.attribute("y").toDouble(&ok);
                    }
                    else {  // para que solo escale y no lo mueva, son los radios
                        x = e_point.attribute("x").toDouble(&ok)/getWorldScale();
                        y = e_point.attribute("y").toDouble(&ok)/getWorldScale();
                    }
                }
                else {
                    x = xTransform(e_point.attribute("x").toDouble(&ok));
                    y = yTransform(e_point.attribute("y").toDouble(&ok));
                }
                points.append(QPointF(x, y));
            }
        }
    }
    return points;
}

void AbstractWorld::setXMLMaterials(const QDomElement& e_materials, CollisionSolver* solver)
{
    QDomNodeList l_cons = e_materials.childNodes();

    for (int j = 0; j < l_cons.count(); ++j) {
        // Recorro los constructores de cada material
        QDomNode n_cons = l_cons.item(j);
        if(!n_cons.isNull()) {
            QDomElement e_cons = n_cons.toElement();
            if(!e_cons.isNull()) {
                Material* m = new Material();
                m->setParams(e_cons);
                solver->addMaterial(m);
            }
        }
    }
}

void AbstractWorld::getXMLDynamics(const QDomElement& e_dynamics, CollisionSolver* solver)
{
    bool ok;
    QDomNodeList l_dynamics = e_dynamics.childNodes();
    for (int j = 0; j < l_dynamics.count(); ++j) {
        // Recorro los atributos de cada punto
        QDomNode n_dynamic = l_dynamics.item(j);
        if(!n_dynamic.isNull()) {
            QDomElement e_dynamic = n_dynamic.toElement();
            if(!e_dynamic.isNull()) {
                solver->setLambda(e_dynamic.attribute("lambda").toDouble(&ok));
                solver->setW_short(e_dynamic.attribute("w_short").toDouble(&ok));
            }
        }
    }
}

void AbstractWorld::addRFSource(QGraphicsScene* canvas, int x, int y, const QString& nameId)
{

    QList<RadialFieldSource*> sources;//lista vacia
    QString src_name = "RFSource";
    RadialFieldSource* RFSource;

    if(!sources_dict.contains(src_name)) {
        sources_dict.insert(src_name, sources);
    }
    else {
        sources = sources_dict[src_name];
    }

    RFSource = new RadialFieldSource(x, y, 1, nameId, Settings::instance()->getAppDirectory() + "/src/world/imagenes/radialfieldsource.svg");
    RFSource->setData(1, "RadialFieldSource");
    sources.append(RFSource);
    sources_dict[src_name] = sources;
    w_canvas->addItem(RFSource);
}


void AbstractWorld::drawGoalIcon(const int& x, const int& y, QColor color, const QString& texto, Qt::BrushStyle sty)
{

    int x_trans = x - icon_goal_side/2;
    int y_trans = y - icon_goal_side/2;
    QGraphicsRectItem* rect = new QGraphicsRectItem(x_trans, y_trans,
                                                    icon_goal_side, icon_goal_side);
    rect->setData(1, "Goal");
    w_canvas->addItem(rect);
    rect->setZValue(300);
    QPen pen(Qt::NoPen);
    QBrush brush(color, sty);
    rect->setBrush(brush);
    rect->setPen(pen);
    addText(x_trans + icon_goal_side, y_trans, texto);
    rect->show();
}

void AbstractWorld::addText(const int& x, const int& y, const QString& texto)
{

    if(texto.isEmpty())
        return;

    QGraphicsSimpleTextItem* text = new QGraphicsSimpleTextItem(texto);
    text->setData(1, texto);
    w_canvas->addItem(text);

    text->setZValue(600);
    text->setX(double(x) + 3);
    text->setY(double(y) + icon_goal_side/3.0);
    QFont font("Helvetica");
    font.setPointSizeF(16);
    text->setFont(font);
    text->show();
}

void AbstractWorld::getDefaultMaterials(void)
{
    DefaultMaterialsDialog* dialog = new DefaultMaterialsDialog();
    dialog->setMaterials(defaultMaterials);
    dialog->setWindowTitle(tr("Default materials"));
    if(dialog->exec() == QDialog::Accepted)
        defaultMaterials = dialog->getMaterials();
    delete dialog;
    if(defaultMaterials.isEmpty())
        defaultMaterials.append(new Material());
    else
        materialsDefined = true;

    QDomDocument doc("materials");

    // Elemento Raiz
    defaultMaterialsDom = doc.createElement("Materials");
    doc.appendChild(defaultMaterialsDom);
    for(int i = 0; i < defaultMaterials.size(); i++) {
        Material* m = defaultMaterials.at(i);
        QDomElement constructor = doc.createElement("constructor");
        m->getParams(constructor);
        defaultMaterialsDom.appendChild(constructor);
    }
}

void AbstractWorld::changeMaterials(void)
{
    QList<CollisionSolver*> it(solid_objects);
    for(int k=0; k<it.size(); k++) {
        if(materialsDefined)
            setXMLMaterials(defaultMaterialsDom, it.value(k));
    }
}

QList<QPixmap> AbstractWorld::drawEllipsePix(double radius_x, double radius_y, double angle, int thickness, QColor pen_color, QColor fill_color, int area_fill)
{

    int diamtr_x, diamtr_y;
    double a2, b2, a2b;
    a2 = cos(-angle)*cos(-angle)/(radius_x*radius_x)+sin(-angle)*sin(-angle)/(radius_y*radius_y);
    b2 = sin(-angle)*sin(-angle)/(radius_x*radius_x)+cos(-angle)*cos(-angle)/(radius_y*radius_y);
    a2b = -sin(-angle)*cos(-angle)/(radius_x*radius_x) + sin(-angle)*cos(-angle)/(radius_y*radius_y);

    double d_x = fabs(sqrt(b2)/sqrt(a2*b2-a2b*a2b));
    double d_y = fabs(sqrt(a2)/sqrt(a2*b2-a2b*a2b));
    diamtr_x = (d_x == double(int(d_x))) ? 2*int(d_x) : 2*(int(d_x) + 1);
    diamtr_y = (d_y == double(int(d_y))) ? 2*int(d_y) : 2*(int(d_y) + 1);

    QPixmap pix(diamtr_x, diamtr_y);
    QBitmap pix_mask(diamtr_x, diamtr_y);
    pix_mask.clear();
    QList<QPixmap> l_pix;
    QBrush brush;
    QPen pen;
    pen.setWidth(thickness);
    pen.setColor(pen_color);
    if(area_fill == -1)
        brush.setStyle(Qt::NoBrush);
    else
        brush.setStyle(Qt::SolidPattern);
    brush.setColor(fill_color);

    QPainter p(&pix);
    p.setPen(pen);
    p.setBrush(brush);
    p.translate(diamtr_x/2, diamtr_y/2);
    p.rotate(-angle*NDMath::RAD2GRAD);
    p.drawEllipse( -radius_x, -radius_y,
                   2*radius_x, 2*radius_y );
    p.end();

    QPainter pm(&pix_mask);
    pm.setPen(QColor(Qt::color1));
    pm.setBrush(QColor(Qt::color1));
    pm.translate(diamtr_x/2, diamtr_y/2);
    pm.rotate(-angle*NDMath::RAD2GRAD);
    pm.drawEllipse( -radius_x, -radius_y,
                    2*radius_x, 2*radius_y );
    pm.end();

    pix.setMask(pix_mask);
    l_pix.append(pix);
    return l_pix;

}

void AbstractWorld::addRobotItem(QGraphicsItem* robotItem) {
    robotItems.append(robotItem);
}

void AbstractWorld::simClosed() {
    robotItems.clear();
    
    QList<QGraphicsItem*> list = w_canvas->items();
    QList<QGraphicsItem*>::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
        QGraphicsItem *hit = *it;
        if(hit->data(0).toString() == "MovingBresenham")
            w_canvas->removeItem(hit);
    }
}
QuadTree* AbstractWorld::getQuadTree() {
    return quadtree;
}

void AbstractWorld::setUpScene(QRect worldRect) {
    if(quadtree)
        delete quadtree;
    quadtree = new QuadTree(worldRect, cellSize);

    quadtree->setSceneScale(world_scale);
    connect(quadtree, SIGNAL(drawCell(QPoint , uint )), this, SLOT(drawCell(QPoint , uint )));

    quadTreeRect = quadtree->calculateGrid(levels);
    quadCellSize = 1 << levels;

    NDMath::scaleRect(quadTreeRect, 1.0/world_scale);

    QRectF wF(worldRect);
    NDMath::scaleRect(wF, 1.0/world_scale);
    QRectF sceneRect;
    sceneRect.setX(wF.x() - sceneMargin/2);
    sceneRect.setY(wF.y() - sceneMargin/2);
    sceneRect.setWidth(wF.width() + sceneMargin);
    sceneRect.setHeight(wF.height() + sceneMargin);
    w_canvas->setSceneRect(sceneRect);
    w_canvas->setBackgroundBrush(Qt::white);

    canvas_view->resizeToCanvas();

    bresenhamCalculador.setParameters(QPoint(quadTreeRect.topLeft().toPoint()), cellSize/world_scale);
}

void AbstractWorld::showQuadtree(bool show) {
    if(show) {
        prevCellCenter.setX(-1); // fuera del Quadtree
        prevCellCenter.setY(-1);
        QVector<QPair<QPoint, uint> >* nodes;
        nodes = quadtree->nodesForDrawing();
        drawTree(nodes);
        quadtreeCreated = true;
    }
    else {
        QList<QGraphicsItem*>::Iterator it = quadTreeItems.begin();
        for (; it != quadTreeItems.end(); ++it) {
            QGraphicsItem *hit = *it;
            w_canvas->removeItem(hit);
        }
        quadTreeItems.clear();
    }
}
