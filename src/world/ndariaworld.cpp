/***************************************************************************
 *   Copyright (C) 2004 by jose manuel                                     *
 *   jose@pcjose                                                           *
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

#include <QMessageBox>
#include <QStringList>

#include "../neuraldis/ndmath.h"
#include "gui/defaultmaterialsdialog.h"
#include "rtti.h"
#include "ndariaworld.h"
#include "material.h"
#include "worldview.h"
#include "mirectangle.h"
#include "miellipse.h"
#include "mipolygon.h"
#include "myline.h"

#include <iostream>

NDAriaWorld::NDAriaWorld(QMdiArea* w_work, QObject* parent, const char* name): AbstractWorld(w_work, parent, name)
{
    world_scale = 10;  // dada escala=10 implica 1 pixel = 10 mm = 1 cm en los map de Aria
    icon_goal_side = 60; // 60 cm
    margin = 0;
    x_robot_home = 0;
    y_robot_home = 0;
}


NDAriaWorld::~NDAriaWorld()
{

}

bool NDAriaWorld::load(QTextStream& ts)
{

    bool ok = true;
    QString extra_line;
    ok = ok && readFirstLines(extra_line, ts);
    ok = ok && readCairns(extra_line, ts);
    ok = ok && readLinesData(extra_line, ts);
    ok = ok && readPointsData(extra_line, ts);

    emit worldLoaded();
    emit worldSources(sources_dict);
    canvas_view->fitInWindow();
    world_empty = false;

    return ok;
}

bool NDAriaWorld::readFirstLines(QString& line, QTextStream& ts)
{

    QStringList dat;
    bool ok;
    double x_min_pos = 1000000.0;
    double y_min_pos = 1000000.0;
    double x_max_pos = -1000000.0;
    double y_max_pos = -1000000.0;

    double x_line_min_pos = 1000000.0;
    double y_line_min_pos = 1000000.0;
    double x_line_max_pos = -1000000.0;
    double y_line_max_pos = -1000000.0;

    int num_points = 0;
    int num_lines = 0;

    line = ts.readLine();
    if(!reloading)
        worldText += line + "\n";
    dat = line.split(" ", Qt::SkipEmptyParts);
    while(dat[0].contains(":") && !ts.atEnd()) {
        if(dat[0] == "Cairn:")
            break;
        else {
            if(dat[0] == "MinPos:") {
                x_min_pos = dat[1].toDouble(&ok)/world_scale;
                y_min_pos = dat[2].toDouble(&ok)/world_scale;
            }
            else if(dat[0] == "MaxPos:") {
                x_max_pos = dat[1].toDouble(&ok)/world_scale;
                y_max_pos = dat[2].toDouble(&ok)/world_scale;
            }
            else if(dat[0] == "NumPoints:")
                num_points = dat[1].toInt(&ok);
            else if(dat[0] == "Resolution:") {
                /* no usado*/
            }
            else if(dat[0] == "LineMinPos:") {
                x_line_min_pos = dat[1].toDouble(&ok)/world_scale;
                y_line_min_pos = dat[2].toDouble(&ok)/world_scale;
            }
            else if(dat[0] == "LineMaxPos:") {
                x_line_max_pos = dat[1].toDouble(&ok)/world_scale;
                y_line_max_pos = dat[2].toDouble(&ok)/world_scale;
            }
            else if(dat[0] == "NumLines:")
                num_lines = dat[1].toInt(&ok);
            else if(dat[0] == "MapInfo:") {
                /* no usado*/
            }
        }
        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
        dat = line.split(" ", Qt::SkipEmptyParts);
    }

    x_min_position =  (x_min_pos < x_line_min_pos ? x_min_pos : x_line_min_pos) - margin;
    y_min_position = (y_min_pos < y_line_min_pos ? y_min_pos : y_line_min_pos) - margin;
    x_max_position = margin + (x_max_pos > x_line_max_pos ? x_max_pos : x_line_max_pos);
    y_max_position = margin + (y_max_pos > y_line_max_pos ? y_max_pos : y_line_max_pos);

    width = x_max_position - x_min_position;
    height = y_max_position - y_min_position;

    QRectF worldRect(x_min_position * world_scale, y_min_position * world_scale,
                     width * world_scale, height * world_scale);
    setUpScene(worldRect.toRect());

    return true;
}

bool NDAriaWorld::readCairns(QString& line, QTextStream& ts)
{

    QStringList dat;
    bool ok;
    double x_min_pos, y_min_pos, x_max_pos, y_max_pos;
    double x_center, y_center;
    QString text;

    if(line.isEmpty()) {
        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
    }
    dat = line.split(" ", Qt::SkipEmptyParts);
    while(dat[0] == "Cairn:" && !ts.atEnd()) {
        if(dat[0] == "LINES")
            break;
        if(dat[0] == "DATA")
            break;
        else {
            if(dat[1] == "ForbiddenArea") {
                text = dat[7].remove('"');
                if(dat[8].contains('"')) { // repasar un nombre puede tener muchos espacios en blanco
                    text += " " + dat[8].remove('"');
                }
                x_min_pos = xTransform(dat[8].toDouble(&ok));
                y_min_pos = yTransform(dat[9].toDouble(&ok));
                x_max_pos = xTransform(dat[10].toDouble(&ok));
                y_max_pos = yTransform(dat[11].toDouble(&ok));

                QGraphicsRectItem* rect = new QGraphicsRectItem(NDMath::roundInt(x_min_pos), NDMath::roundInt(y_min_pos), NDMath::roundInt(x_max_pos - x_min_pos),
                                                                NDMath::roundInt(y_max_pos - y_min_pos));
                w_canvas->addItem(rect);
                rect->setZValue(300);
                QPen pen;
                QBrush brush;
                brush.setStyle(Qt::Dense4Pattern);
                brush.setColor(c_orange);
                pen.setWidth(2);
                pen.setColor(c_orange);
                rect->setPen(pen);
                rect->setBrush(brush);
                addText(NDMath::roundInt(x_max_pos), NDMath::roundInt(y_min_pos), text);
                rect->show();
            }

            if(dat[1] == "ForbiddenLine") {
                text = dat[7].remove('"');
                int increment = 0;
                x_min_pos = dat[8 + increment].toDouble(&ok);
                while(!ok) {
                    text += " " + dat[8+ increment].remove('"');
                    ++increment;
                    x_min_pos = dat[8 + increment].toDouble(&ok);
                }

                x_min_pos = xTransform(x_min_pos);
                y_min_pos = yTransform(dat[9 + increment].toDouble(&ok));
                x_max_pos = xTransform(dat[10 + increment].toDouble(&ok));
                y_max_pos = yTransform(dat[11 + increment].toDouble(&ok));

                drawLine(NDMath::roundInt(x_min_pos), NDMath::roundInt(y_min_pos),
                         NDMath::roundInt(x_max_pos), NDMath::roundInt(y_max_pos), c_orange);

                addText(NDMath::roundInt(x_max_pos), NDMath::roundInt(y_min_pos), text);
            }

            if(dat[1].contains("Goal")) {
                text = dat[7].remove('"');
                int increment = 0;
                while(dat.size() > 8 + increment) {
                    text += " " + dat[8 + increment].remove('"');
                    ++increment;
                }

                x_center = xTransform(dat[2].toDouble(&ok));
                y_center = yTransform(dat[3].toDouble(&ok));

                drawGoalIcon(NDMath::roundInt(x_center), NDMath::roundInt(y_center),
                             c_goal, text);

                if(dat[1] == "GoalWithHeading") {
                    double ang = headingTransform(dat[4].toDouble(&ok));
                    drawHeadingLine(NDMath::roundInt(x_center),
                                    NDMath::roundInt(y_center), ang);
                }

                if(text.contains("RFSource")) {
                    addRFSource(w_canvas, NDMath::roundInt(x_center),
                                NDMath::roundInt(y_center), text);
                }

            }

            if(dat[1] == "RobotHome") {
                text = "Home";
                x_robot_home = dat[2].toDouble(&ok);
                y_robot_home =  dat[3].toDouble(&ok);
                drawGoalIcon(NDMath::roundInt(xTransform(x_robot_home)), NDMath::roundInt(yTransform(y_robot_home)), c_home, text, Qt::Dense3Pattern);
                heading_robot_home = dat[4].toDouble(&ok);
                drawHeadingLine(NDMath::roundInt(x_robot_home), NDMath::roundInt(y_robot_home), headingTransform(heading_robot_home), true);
            }
        }

        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
        dat = line.split(" ", Qt::SkipEmptyParts);
    }
    return true;

}

void NDAriaWorld::drawHeadingLine(const int& x, const int& y, const double& ang, const bool& black)
{

    double x_fin = icon_goal_side/2.0;
    double y_fin = 0;
    double x_center = x;
    double y_center = y;

    NDMath::selfRotateGrad(x_fin, y_fin, x_center, y_center, ang);
    QGraphicsLineItem* canvas_line = new QGraphicsLineItem();
    w_canvas->addItem(canvas_line);
    canvas_line->setLine((int) x_center, (int) y_center, (int) x_fin,(int) y_fin);
    canvas_line->setZValue(500);

    QPen pen;
    pen.setWidth(1);
    if(!black)
        pen.setColor(c_head);
    else
        pen.setColor(c_black);
    canvas_line->setPen(pen);
    canvas_line->show();

}

bool NDAriaWorld::readLinesData(QString& line, QTextStream& ts)
{

    QStringList dat;
    bool ok;
    double x_min_pos, y_min_pos, x_max_pos, y_max_pos;

    if(line.isEmpty()) {
        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
    }
    dat = line.split(" ", Qt::SkipEmptyParts);
    if(dat[0] != "LINES") return true;

    if(!reloading)
        getDefaultMaterials();
    line = ts.readLine();
    if(!reloading)
        worldText += line + "\n";

    dat = line.split(" ", Qt::SkipEmptyParts);
    while(dat[0] != "DATA" && !ts.atEnd()) {
        x_min_pos = xTransform(dat[0].toDouble(&ok));
        y_min_pos = yTransform(dat[1].toDouble(&ok));
        x_max_pos = xTransform(dat[2].toDouble(&ok));
        y_max_pos = yTransform(dat[3].toDouble(&ok));

        if((x_min_pos != x_max_pos) || (y_min_pos != y_max_pos)) {
            QGraphicsLineItem* world_line = drawLine(NDMath::roundInt(x_min_pos),
                                                     NDMath::roundInt(y_min_pos), NDMath::roundInt(x_max_pos),
                                                     NDMath::roundInt(y_max_pos), c_black, true);

            CollisionSolver* solver = factoryCollisionSolver(CollisionSolver::POLYGON_COLL, (MyLine*)world_line);
            solid_objects.append(solver);
            // Materiales
            setXMLMaterials(defaultMaterialsDom, solver);
            solver->setLambda(lambda);
            solver->setW_short(w_short);
        }

        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
        dat = line.split(" ", Qt::SkipEmptyParts);
    }
    return true;
}

bool NDAriaWorld::readPointsData(QString& line, QTextStream& ts)
{

    QStringList dat;
    bool ok;
    double x_min_pos, y_min_pos, x_max_pos, y_max_pos;

    if(line.isEmpty()) {
        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
    }
    dat = line.split(" ", Qt::SkipEmptyParts);
    if(dat[0] != "DATA")
        return true;

    line = ts.readLine();
    if(!reloading)
        worldText += line + "\n";
    
    dat = line.split(" ", Qt::SkipEmptyParts);
    while(!ts.atEnd()) {
        x_min_pos = xTransform(dat[0].toDouble(&ok));
        y_min_pos = yTransform(dat[1].toDouble(&ok));
        x_max_pos = x_min_pos + 1;
        y_max_pos = y_min_pos + 1;

        QGraphicsLineItem* world_line = drawLine(NDMath::roundInt(x_min_pos),
                                                 NDMath::roundInt(y_min_pos), NDMath::roundInt(x_max_pos),
                                                 NDMath::roundInt(y_max_pos), c_black, true);

        CollisionSolver* solver = factoryCollisionSolver(CollisionSolver::POLYGON_COLL, (MyLine*)world_line);
        solid_objects.append(solver);
        // Materiales
        setXMLMaterials(defaultMaterialsDom, solver);
        solver->setLambda(lambda);
        solver->setW_short(w_short);

        line = ts.readLine();
        if(!reloading)
            worldText += line + "\n";
        dat = line.split(" ", Qt::SkipEmptyParts);
    }
    return true;
}


void NDAriaWorld::exportToFig(QTextStream& ts, bool changeCoordinates)
{
    int x, y, xw, yw;
    QList<QGraphicsItem*> list = w_canvas->items();
    QList<QGraphicsItem*>::Iterator it = list.begin();
    //   QString ini_linea = "2 1 0 2 0 0 65 -1 -1 0.000 0 0 7 0 0 2\n\t";
    int thickness = changeCoordinates ? toFigUnits()*3 : 1;
    QString type, subtype, depth;
    int area_fill, pen_color, fill_color;
    QString ini_linea;
    int num_points = 0;
    fill_color = 32; // por ahora

    ts << "0 " << 32  << "#c0c0c0" << "\n";
    for (; it != list.end(); ++it) {
        QGraphicsItem *hit = *it;
        QGraphicsPolygonItem* item = dynamic_cast<QGraphicsPolygonItem*>(hit);
        w_canvas->addItem(item);
        if(item) {
            pen_color = view()->findFigColor(item->pen().color().name());
            pen_color = -1 ? 0 : pen_color;
            depth = QString::number(1000 - item->zValue());
            area_fill = item->brush().style() == Qt::NoBrush ? -1 : 20;
        }

        if((hit->type() > FIRST_COLLISION_RTTI)  && (hit->type() < LAST_COLLISION_RTTI)) {
            SolidObject* object = dynamic_cast<SolidObject*> (hit);
            if(hit->type() == LINE_RTTI) {
                type = QString::number(POLYLINE);
                subtype = QString::number(LINE);
            }
            else if(hit->type() == POLYGON_RTTI) {
                type = QString::number(POLYLINE);
                subtype = QString::number(POLYGON);
            }
            else if(hit->type() == RECTANGLE_RTTI) {
                type = QString::number(POLYLINE);
                subtype = QString::number(BOX);
            }
            else if(hit->type() == ELLIPSE_RTTI) {
                type = QString::number(ELLIPSE); // TODO
                subtype = QString::number(ELLIPSE_DIAM);
            }

            QPolygonF polygon = object->getPoints();
            if(hit->type() == LINE_RTTI)
                num_points = polygon.size();
            else if(hit->type() != ELLIPSE_RTTI)
                num_points = polygon.size() + 1; // +1 para cerrar
            else {
                // TODO
            }

            ini_linea = type + " " + subtype + " 0 " + QString::number(thickness) + " " +
                    QString::number(pen_color)+ " " + QString::number(fill_color)+ " " +
                    depth + " -1 " + QString::number(area_fill) + " 0.000 0 0 7 0 0 " +
                    QString::number(num_points) + "\n\t";

            num_points = polygon.size();
            ts << ini_linea; //  10 mm = 1 cm

            for(int i = 0; i < num_points; i++) {
                if(changeCoordinates) {
                    x = NDMath::roundInt(inverseXTransform(polygon.at(i).x()) * toFigUnits());
                    y = -NDMath::roundInt(inverseYTransform(polygon.at(i).y()) * toFigUnits());
                }
                else {
                    x = NDMath::roundInt(inverseXTransform(polygon.at(i).x())*world_scale);
                    y = NDMath::roundInt(inverseYTransform(polygon.at(i).y())*world_scale);
                }
                if(!((i+1)%6))
                    ts << x << " " << y << "\n\t";
                else
                    ts << x << " " << y << " ";
            }
            if(num_points > 2) { // no líneas
                if(changeCoordinates) {
                    x = NDMath::roundInt(inverseXTransform(polygon.at(0).x()) * toFigUnits());
                    y = -NDMath::roundInt(inverseYTransform(polygon.at(0).y()) * toFigUnits());
                }
                else {
                    x = NDMath::roundInt(inverseXTransform(polygon.at(0).x())*world_scale);
                    y = NDMath::roundInt(inverseYTransform(polygon.at(0).y())*world_scale);
                }
                ts << x << " " << y << "\n";
            }
            else
                ts <<"\n";
        }
        else if (hit->type() == RADIALFIELDSOURCE_RTTI) {
            RadialFieldSource* source = dynamic_cast<RadialFieldSource*> (hit);
            w_canvas->addItem(source);
            ts << "# " << "RadialFieldSource" << " " << -1 << " " << source->getNameId() << "\n";
            const QString ini_box1 = "2 2 0 2 0 ";
            const QString ini_box2 = " 50 -1 30 0.000 0 0 -1 0 0 5 ";
            const double width = 50;

            ts << ini_box1 << 12 << ini_box2 << "\n\t"; // 12 verde claro
            if(changeCoordinates) {
                x = NDMath::roundInt(inverseXTransform(hit->x()) * toFigUnits());
                y = -NDMath::roundInt(inverseYTransform(hit->y()) * toFigUnits());
                xw = NDMath::roundInt(inverseXTransform(hit->x() + width) * toFigUnits());
                yw = -NDMath::roundInt(inverseYTransform(hit->y() + width) * toFigUnits());
            }
            else {
                x = NDMath::roundInt(inverseXTransform(hit->x())*world_scale);
                y = NDMath::roundInt(inverseYTransform(hit->y())*world_scale);
                xw = NDMath::roundInt(inverseXTransform(hit->x() + width)*world_scale);
                yw = NDMath::roundInt(inverseYTransform(hit->y() + width)*world_scale);
            }
            ts << x << " " << y << " " << xw << " " << y <<  " ";
            ts << xw  << " " << yw << " "  << x << " " << yw << " ";
            ts << x << " " << y << "\n";

        }
    }
}
