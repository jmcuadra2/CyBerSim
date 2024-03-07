/***************************************************************************
                          world.cpp  -  description
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

#include <QMessageBox>
#include <QDomDocument>
#include <QBitmap>
#include <QPixmap>
#include <stdlib.h>

#include "world.h"
#include "rtti.h"
#include "material.h"

#include "worldview.h"
#include "mirectangle.h"
#include "miellipse.h"
#include "mipolygon.h"
#include "myline.h"
#include "collisionsolver.h"
#include "lamp.h"

#include "../neuraldis/settings.h"
#include "../neuraldis/xmloperator.h"

World::World(QMdiArea *w_work, QObject *parent, const char *name)
    : AbstractWorld(w_work, parent, name)
{
    w_canvas->setBackgroundBrush(QColor("#d1d1d1"));
}

World::~World()
{
    clearFree();

    QList<QPixmap*> itell(ellyp_pix_list);
    QPixmap *cpa;
    for(int i = 0; i < ellyp_pix_list.size(); i++) {
        cpa = itell.value(i);
        delete cpa;
    }
}

void World::clear(void) 
{

    QList<QPixmap*> itell(ellyp_pix_list);
    QPixmap *cpa;
    for(int i = 0; i < ellyp_pix_list.size(); i++) {
        cpa = itell.value(i);
        delete cpa;
    }
    ellyp_pix_list.clear();
    canvas_view->xfig_colors.clear();
    AbstractWorld::clear();

}

bool World::load(QTextStream& ts)
{

    QStringList dat;
    bool ok;
    bool ok2;

    for(uint i = 1; i < 8; i++) {
        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
    }

    line = ts.readLine();
    if(!reloading)
        worldText += line + "\n";

    if(!line.contains("# NeuralDis world")) {
//        QMessageBox::critical(nullptr, tr("Load world"),
//                              tr("Isn't a valid world file"), tr("&Cancel"));
        QMessageBox::critical(nullptr, tr("Load world"),
                              tr("Isn't a valid world file"), QMessageBox::Cancel);
        return false;
    }

    canvas_view->xfigColors();
    line = ts.readLine();   // Fin parámetros de la imagen total
    if(!reloading)
        worldText += line + "\n";

    while(!ts.atEnd()) {          // Colores personalizados
        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
        
        if(line.isEmpty())
            continue;
        dat = line.split(" ");
        if (dat[0] == "#")
            continue;
        object = dat[0].toInt(&ok);
        if(!ok) {
            QMessageBox::critical(nullptr, tr("Load world"),
                                  tr("Isn't a valid world file"), QMessageBox::Cancel);
            return false;
        }
        if(object != COLOR) {
            if(object == COMPOUND) {         // Tamaño del canvas
                x_min_position = dat[1].toInt()/SCALE_FIG_2_PIXEL;
                x_max_position = dat[3].toInt()/SCALE_FIG_2_PIXEL;
                width = x_max_position - x_min_position;

                y_min_position = dat[2].toInt()/SCALE_FIG_2_PIXEL;
                y_max_position = dat[4].toInt()/SCALE_FIG_2_PIXEL;
                height = y_max_position - y_min_position;


                QRectF worldRect(x_min_position, y_min_position, width, height);
                setUpScene(worldRect.toRect());
                break;
            }
            else {
                QMessageBox::critical(nullptr, tr("Load world"),
                                      tr("Isn't a valid world file,\n whole compound missing"),
                                      QMessageBox::Cancel);
                return false;
            }
        }
        else addColorObject(dat);
    }

    while(!ts.atEnd()) {            // Inicio objetos
        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
        if(line.isEmpty()) continue;
        dat = line.split(" ");
        if (dat[0] == "#") {
            if(dat[1] == "Lamp")
                object = LAMP;
            else if(dat[1] == "RadialFieldSource")
                object = RADIALFIELDSOURCE_RTTI;
            else continue;
        }
        else {
            object = dat[0].toInt(&ok);            
            if(!ok)
                continue;
        }

        if(object == ELLIPSE) {
            readCommonProps(dat);
            readEllipseProps(dat);

            QPolygonF pol_points;
            pol_points.append(QPointF(center_x, center_y));
            pol_points.append(QPointF(radius_x, radius_y));
            pol_points.append(QPointF(-angle, 0)); // 0 no se usa

            MiEllipse* ellip = new MiEllipse(pol_points, thickness,
                                             QColor(canvas_view->getXFigColor(pen_color)),
                                             QColor(canvas_view->getXFigColor(fill_color)),
                                             double(depth), area_fill);
            ellip->setData(1, "MiEllipse");
            ellip->setPoints(pa);

            solid_objects.append(factoryCollisionSolver(CollisionSolver::ELLIPSE_COLL,ellip));
            w_canvas->addItem(ellip);
        }
        else if(object == POLYLINE) {
            subtype = dat[1].toUInt(&ok2);
            if(!ok2)
                continue;
            if(subtype != PICTURE) {
                readCommonProps(dat);
                readPolygonProps(dat, ts);
                if(subtype == BOX) {
                    MiRectangle* rect = new MiRectangle(pa, thickness, QColor(canvas_view->getXFigColor(pen_color)),
                                                        QColor(canvas_view->getXFigColor(fill_color)),
                                                        depth, area_fill);
                    rect->setData(1, "MiRectangle");
                    solid_objects.append(factoryCollisionSolver(CollisionSolver::POLYGON_COLL, rect));
                    w_canvas->addItem(rect);
                }
                else if(subtype == LINE) {
                    for(int i = 0; i < pa.size() - 1;i++) {
                        QGraphicsLineItem* line = drawLine(pa.point(i).x(), pa.point(i).y(), pa.point(i+1).x(), pa.point(i+1).y(), QColor(canvas_view->getXFigColor(pen_color)), true);
                        CollisionSolver* solver = factoryCollisionSolver(CollisionSolver::POLYGON_COLL, (MyLine*)line);
                        solid_objects.append(solver);
                        //// Materiales
                        solver->addMaterial(new Material());
                    }
                }

                else if(subtype == POLYGON){
                    MiPolygon* poly = new MiPolygon(pa, thickness, QColor(canvas_view->getXFigColor(pen_color)),
                                                    QColor(canvas_view->getXFigColor(fill_color)),
                                                    depth, area_fill);
                    poly->setData(1, "MiPolygon");
                    solid_objects.append(factoryCollisionSolver(CollisionSolver::POLYGON_COLL, poly));
                    w_canvas->addItem(poly);
                }
            }
        }
        else if(object == SPLINE) {   // Se dibujan como poligonos
            readCommonProps(dat);
            readSplineProps(dat, ts);
            MiPolygon* poly = new MiPolygon(pa, thickness, QColor(canvas_view->getXFigColor(pen_color)),
                                            QColor(canvas_view->getXFigColor(fill_color)),
                                            depth, area_fill);
            poly->setData(1, "MiPolygon");
            solid_objects.append(factoryCollisionSolver(CollisionSolver::POLYGON_COLL, poly));
            w_canvas->addItem(poly);
        }
        else if(object == TEXT) {
            if(!readTextProps(dat))
                continue;
            addText(x, int(y - text_height),
                    uint(pen_color), depth,
                    font_size, subtype,    // la coordenada 'y'en Xfig
                    texto); // es la parte inferior
        }
        else if(object == LAMP || object == RADIALFIELDSOURCE_RTTI) {
            if(!readLampProps(dat, ts))
                continue;
            QList<RadialFieldSource*> sources;
            QString src_type = dat[1];
            QString src_nameId;
            QString str_svg = "";
            if(dat.size() > 4) {
                src_nameId = dat[3];
                str_svg = dat[4];
            }

            RadialFieldSource* rfSource;
            if(!sources_dict.contains(src_type)) {
                sources_dict.insert(src_type, sources);
            }
            else
                sources = sources_dict[src_type];
            if(object == LAMP) {
                rfSource = new Lamp(x, y, 1, src_nameId,
                                    Settings::instance()->getAppDirectory() + "src/world/imagenes/lamp.svg");
                rfSource->setData(1, "Lamp");
            }
            else {
                if(str_svg.isEmpty())
                    rfSource = new RadialFieldSource(x, y, 1, src_nameId, Settings::instance()->getAppDirectory() + "src/world/imagenes/radialfieldsource.svg");
                rfSource->setData(1, "RadialFieldSource");
            }

            w_canvas->addItem(rfSource);
            sources.append(rfSource);

        }
    }
    emit worldSources(sources_dict);
    emit worldLoaded();


    canvas_view->fitInWindow();

    world_empty = false;
    return true;

}

void World::addColorObject(QStringList dat)
{

    uint i;
    color_num =  dat[1].toUInt() + 1;
    c_col = canvas_view->xfig_colors.count();
    if(color_num > c_col) {
        for(i = c_col; i < color_num - 1; i++)
            canvas_view->xfig_colors.append("#000000");
    }
    color = dat[2];
    canvas_view->xfig_colors.append(color);

}

void World::readCommonProps(QStringList dat)
{

    subtype = dat[1].toUInt();
    thickness = dat[3].toUInt();
    pen_color = dat[4].toInt();
    if(pen_color < 0)
        pen_color = 0;
    fill_color = dat[5].toInt();
    if(fill_color < 0)
        fill_color = 0;
    depth = dat[6].toFloat();  // Es int en Xfig pero float en canvas->z()
    area_fill = dat[8].toInt();

}

void World::readEllipseProps(QStringList dat)
{

    angle = dat[11].toFloat();
    center_x = dat[12].toDouble()/SCALE_FIG_2_PIXEL;  // Es int en Xfig pero double en canvas->move()
    center_y = dat[13].toDouble()/SCALE_FIG_2_PIXEL;  // Idem
    radius_x = dat[14].toInt()/SCALE_FIG_2_PIXEL;
    radius_y = dat[15].toInt()/SCALE_FIG_2_PIXEL;

}

QList< QPixmap > World::drawEllipsePix(void )
{
    return AbstractWorld::drawEllipsePix(radius_x, radius_y, -angle, thickness, QColor(canvas_view->getXFigColor(pen_color)),
                                         QColor(canvas_view->getXFigColor(fill_color)), area_fill); // -angle por XFig
}


void World::readSplineProps(QStringList dat, QTextStream &ts)
{

    uint i, j, n_lines;
    n_points = dat[13].toUInt();
    n_lines = n_points/6 + 1;
    pa.resize(n_points);
    for(j = 0; j < n_lines - 1; j++) {
        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
        dat = line.split(" ");
        for(i = 0; i < 6; i++) {
            x = dat[2*i + 1].toInt()/SCALE_FIG_2_PIXEL;
            y = dat[2*i + 2].toInt()/SCALE_FIG_2_PIXEL;
            pa.setPoint(i + 6*j, x, y);
        }
    }
    line = ts.readLine();
    if(!reloading)
        worldText += line + "\n";
    
    dat = line.split(" ");
    for(i = 0; i < n_points %6; i++) {
        x = dat[2*i + 1].toInt()/SCALE_FIG_2_PIXEL;
        y = dat[2*i + 2].toInt()/SCALE_FIG_2_PIXEL;
        pa.setPoint(i + 6*j, x, y);
    }
}

void World::readPolygonProps(QStringList dat, QTextStream &ts)
{

    uint i, j, n_lines;
    n_points = dat[15].toUInt();
    if(subtype !=  LINE)
        --n_points;
    n_lines = n_points/6 + 1;
    pa.resize(n_points);
    for(j = 0; j < n_lines - 1; j++) {
        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";

        dat = line.split(" ");
        for(i = 0; i < 6; i++) {
            x = dat[2*i + 1].toInt()/SCALE_FIG_2_PIXEL;
            y = dat[2*i + 2].toInt()/SCALE_FIG_2_PIXEL;
            pa.setPoint(i + 6*j, x, y);
        }
    }
    line = ts.readLine();
    if(!reloading)
        worldText += line + "\n";
    
    dat = line.split(" ");
    for(i = 0; i < n_points%6; i++) {
        x = dat[2*i + 1].toInt()/SCALE_FIG_2_PIXEL;
        y = dat[2*i + 2].toInt()/SCALE_FIG_2_PIXEL;
        pa.setPoint(i + 6*j, x, y);
    }
}

bool World::readTextProps(QStringList dat)
{

    subtype = dat[1].toUInt();
    pen_color = dat[2].toInt();
    if(pen_color < 0)
        pen_color = 0;
    depth = dat[3].toFloat();  // Es int en Xfig pero float en canvas->z()
    font_size = dat[6].toFloat();
    depth = dat[6].toFloat();  // Es int en Xfig pero float en canvas->z()
    angle = dat[7].toFloat();
    if(fabs(angle) > 0.1)
        return false ;  // No rotacion

    area_fill = dat[8].toInt();
    text_height = dat[9].toFloat()/SCALE_FIG_2_PIXEL;  // Es int en QDrawText
    x = dat[11].toInt()/SCALE_FIG_2_PIXEL;
    y = dat[12].toInt()/SCALE_FIG_2_PIXEL;
    texto = line.section(" ", 13);
    texto = texto.section("\\", 0, 0);
    return true ;

}

void World::addText(int x, int y, uint pen_color,
                    float depth, float font_size, uint subtype,
                    QString texto)
{

    QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(texto);
    text->setData(1, texto);
    w_canvas->addItem(text);
    text->setZValue(1000 - depth);
    text->setX(double(x));
    text->setY(double(y));
    QFont font("Helvetica");
    font.setPointSizeF(font_size);
    text->setFont(font);
    text->setPen(QColor(canvas_view->getXFigColor(pen_color)));

    text->show();

}


bool World::readLampProps(QStringList dat, QTextStream &ts)
{

    bool ok;
    lamp_radius = dat[2].toUInt(&ok);
    if(!ok)
        lamp_radius = 100;
    line = ts.readLine();
    if(!reloading)
        worldText += line + "\n";
    
    dat = line.split(" ");
    if(dat[0].toInt() == COMPOUND)  {
        x = dat[1].toInt()/SCALE_FIG_2_PIXEL;
        y = dat[2].toInt()/SCALE_FIG_2_PIXEL;
    }
    else
        return false;

    line = ts.readLine();
    if(!reloading)
        worldText += line + "\n";
    
    dat = line.split(" ");
    while(dat[0].toInt() != END_COMPOUND && !ts.atEnd()) {
        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";

        dat = line.split(" ");
    }
    if(dat[0].toInt() == END_COMPOUND)
        return true;
    else
        return false;
}


void World::setRobotRadius(const double& rad)
{

    MiEllipse* ellip = 0;
    MiRectangle* rectangle = 0;
    MiPolygon* polygon = 0;
    AbstractWorld::setRobotRadius(rad);
    QList<QGraphicsItem*> list = w_canvas->items();
    QList<QGraphicsItem*>::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
        QGraphicsItem *hit = *it;
        if(hit->type() == ELLIPSE_RTTI) {
            ellip = (MiEllipse*) hit;
            ellip->getCollisionSolver()->setRobotRadius(rad);
        }
        else if(hit->type() == POLYGON_RTTI) {
            polygon = (MiPolygon*) hit;
            polygon->getCollisionSolver()->setRobotRadius(rad);
        }
        else if(hit->type() == RECTANGLE_RTTI) {
            rectangle = (MiRectangle*) hit;
            rectangle->getCollisionSolver()->setRobotRadius(rad);
        }
    }

}
