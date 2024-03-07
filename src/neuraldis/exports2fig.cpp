/***************************************************************************
                          exports2fig.cpp  -  description
                             -------------------
    begin                : Thu Apr 14 2005
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

#include "exports2fig.h"
#include "stroke.h"
#include "ndmath.h"

#include <QStringList>
#include <QMessageBox>
#include <QXmlStreamWriter>

Exports2Fig* Exports2Fig::ex_this = 0;

Exports2Fig::Exports2Fig(QObject* parent)
    : QObject(parent)
{
    prevFilePos = 0;
}

Exports2Fig::~Exports2Fig(){
}

Exports2Fig* Exports2Fig::instance(void)
{
    if(ex_this == 0)
        ex_this = new Exports2Fig();
    return ex_this;

}  

void Exports2Fig::exportPath(QTextStream &ts_dat, QTextStream &ts_new, ExportModes exportMode)
{

    QString line;
    QStringList dat;
    toFigUnits = 1;
    //   double x, y, worldXMin, worldYMin;
    worldXMin = 0;
    worldYMin = 0;
    int  n_lines = 0;
    bool ok = false;
    inverseY = false;
    world_scale = 10.0;
    robot_scale = 2.0;
    robot_or_ac = true;

    while(!ts_dat.atEnd()) {
        prevFilePos = ts_dat.pos();
        line = ts_dat.readLine();

        if(line.trimmed().isEmpty()) {
            if(exportMode == ToAscii)
                ts_new << line << Qt::endl;
            continue;
        }
        else if((exportMode == ToAscii) && (!line.trimmed().at(0).isDigit()))
            ts_new << line << Qt::endl;

        dat = line.split(" ");
        if(dat[0] == "#world_scale") {
            world_scale = dat[1].toDouble(&ok);
            if(!ok)
                world_scale = 10.0;
        }
        else if(dat[0] == "#width") {
            worldWidth = dat[1].toInt(&ok);
            if(!ok)  {
                messageNoPath();
                return;
            }
        }
        else if(dat[0] == "#height") {
            worldHeight = dat[1].toInt(&ok);
            if(!ok) {
                messageNoPath();
                return;
            } /*else*/
            //         ts_new << "6 0 0 " << worldWidth*world_scale << " " << worldHeight*world_scale << "\n";
        }
        else if(dat[0] == "#robot_scale") {
            robot_scale = dat[1].toDouble(&ok);
            if(!ok)
                robot_scale = 2.0;
        }
        else if(dat[0] == "#world_xmin") {
            //       worldXMin = int(dat[1].toInt(&ok)*world_scale + 0.5);
            worldXMin = dat[1].toDouble(&ok);
        }
        else if(dat[0] == "#world_ymin") {
            //       worldYMin = int(dat[1].toInt(&ok)*world_scale + 0.5);
            worldYMin = dat[1].toDouble(&ok);
        }
        else if(dat[0] == "#inverseY") {
            inverseY = (bool) dat[1].toInt(&ok);
        }
        else if(dat[0] == "#toFigUnits") {
            toFigUnits = dat[1].toInt(&ok);
            if(!ok)
                toFigUnits = 1;
        }
        else if(dat[0] == "#AC")
            robot_or_ac = false;
        else if(line.trimmed().at(0).isDigit()) {
            if(exportMode == ToFig)
                exportToFig(ts_dat, ts_new, line);
            else if(exportMode == ToAscii)
                exportToAscii(ts_dat, ts_new, line);
            else if(exportMode == ToSvg)
                exportToSvg(ts_dat, ts_new, line);
            break;
        }
        else
            continue;
    }

}

void Exports2Fig::messageNoPath(void)
{
//    QMessageBox::critical(nullptr, tr("exportPath"), tr("Invalid path file"), tr("&Cancel"));
    QMessageBox::critical(nullptr, tr("exportPath"), tr("Invalid path file"), QMessageBox::Cancel);
}

void Exports2Fig::exportToFig(QTextStream &ts_dat, QTextStream &ts_fig, QString line)
{

    int n_points = 0;
    double x, y;
    bool ok1, ok2;
    QStringList dat;
//    QString l;
//    bool r;
    
    while(!ts_dat.atEnd()) {
        if(line.isEmpty())
            break;
//        l = line.trimmed().at(0);
//        r = line.trimmed().at(0).isDigit();
        if(!line.trimmed().at(0).isDigit())
            break;
        else
            ++n_points;
        line = ts_dat.readLine();
    }
    if(!n_points) {
//        QMessageBox::critical(nullptr, tr("exportPath"),
//                              tr("Empty path file"), tr("&Cancel"));
        QMessageBox::critical(nullptr, tr("exportPath"),
                              tr("Empty path file"), QMessageBox::Cancel);
        return;
    }
    ts_dat.seek(prevFilePos);
    
    ts_fig << "6 0 0 " << worldWidth*world_scale << " " << worldHeight*world_scale << Qt::endl;
    ts_fig << "2 1 0 31 0 7 60 0 -1 0.000 0 0 -1 1 0 " << n_points << "\n";
    ts_fig << "\t1 1 2.00 120.00 240.00\n";

    for(int i = 0; i < n_points; i += 6) {
        ts_fig << "\t";
        for(int j = 0; j < 6; j++) {
            line = ts_dat.readLine();
            if(line.isEmpty()) {
                ts_fig << "\n";
                break;
            }

            dat = line.split("\t");
            x = dat[1].toDouble(&ok1);
            y = dat[2].toDouble(&ok2);
            if(!ok1 || !ok2) {
                messageNoPath();
                return;
            }

            if(robot_scale != 0.0) {// esto es un apano para las trayectoria grabadas de Aria, que van en coordenadas reales
//                x = (x/robot_scale + worldXMin)*world_scale;
                x = x/robot_scale*world_scale;
                if(inverseY)
                    y = -(worldHeight + 2*worldYMin - y/robot_scale)*world_scale;
                else
                    y = (y/robot_scale + worldYMin)*world_scale;
            }
            else if(inverseY)
                y = -y;

            ts_fig << NDMath::roundInt(x*toFigUnits) << " ";
            ts_fig << NDMath::roundInt(y*toFigUnits);

            if(j < 5) ts_fig << " ";
            else ts_fig << "\n";
        }
    }
    ts_fig << "-6\n";

}

void Exports2Fig::exportToAscii(QTextStream &ts_dat, QTextStream &ts_new, QString line)
{
    double x, y, ang;
    bool ok1, ok2;
    QStringList dat;
    //  bool robot_or_ac = true; // robot path no AC path

    if(line.isEmpty() || !line.trimmed().at(0).isDigit())
        ts_new << line << Qt::endl;
    else {
        dat = line.split("\t");
        x = dat[1].toDouble(&ok1);
        y = dat[2].toDouble(&ok2);
        if(!ok1 || !ok2) {
            messageNoPath();
            return;
        }
        else {
//            x = (dat[1].toDouble()/robot_scale  + worldXMin)*world_scale; // pasan en cm
            x = dat[1].toDouble()/robot_scale; // pasan en cm
            if(inverseY)
//                y = (worldHeight + worldYMin - dat[2].toDouble()/robot_scale)*world_scale;
                y = (worldHeight + 2*worldYMin - dat[2].toDouble()/robot_scale);
            else
//                y = (dat[2].toDouble()/robot_scale  + worldYMin)*world_scale;
                y = dat[2].toDouble()/robot_scale;

            dat[1] = QString::number(x);
            dat[2] = QString::number(y);

            //      robot_or_ac = (dat.length() == 6); // apano por ahora
            if(robot_or_ac) {
                ang = dat[3].toDouble();
                ang = ang >180 ? ang - 360 : ang;
                if(inverseY)
                    ang = -ang;
                dat[3] = QString::number(ang);
            }

            ts_new << dat.join("\t") << Qt::endl;
        }
    }
    
    while(!ts_dat.atEnd()) {
        line = ts_dat.readLine();

        if(line.isEmpty() || !line.trimmed().at(0).isDigit()) {
            ts_new << line << Qt::endl;
            continue;
        }

        dat = line.split("\t");
        x = dat[1].toDouble(&ok1);
        y = dat[2].toDouble(&ok2);
        if(!ok1 || !ok2) {
            messageNoPath();
            return;
        }
        else {
            ang = dat[3].toDouble();
            ang = ang >180 ? ang - 360 : ang;
//            x = (x/robot_scale  + worldXMin)*world_scale;
            x = dat[1].toDouble()/robot_scale; // pasan en cm
            if(inverseY) {
//                y = (worldHeight + worldYMin - y/robot_scale)*world_scale;
                y = (worldHeight + 2*worldYMin - dat[2].toDouble()/robot_scale);
                ang = -ang;
            }
            else
//                y = (y/robot_scale  + worldYMin)*world_scale;
                y = dat[2].toDouble()/robot_scale;

            dat[1] = QString::number(x);
            dat[2] = QString::number(y);

            robot_or_ac = (dat.length() == 6); // apano por ahora
            if(robot_or_ac) {
                ang = dat[3].toDouble();
                ang = ang >180 ? ang - 360 : ang;
                if(inverseY)
                    ang = -ang;
                dat[3] = QString::number(ang);
            }

            ts_new << dat.join("\t") << Qt::endl;
        }
    }

}

void Exports2Fig::exportToSvg(QTextStream &ts_dat, QTextStream &ts_new, QString line)
{
    int n_points = 0;
    double x, y;
    bool ok1, ok2;
    QStringList dat;
    Stroke stroke;

    QMessageBox::StandardButton ret = QMessageBox::question(nullptr, tr("Svg Export"), tr("Change coordinates to Inkspace ?"),
                                    QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::No);

    if(ret == QMessageBox::Cancel) {
        return;
    }

    while(!ts_dat.atEnd()) {
        if(line.isEmpty())
            break;
        if(!line.trimmed().at(0).isDigit())
            break;
        else {
            dat = line.split("\t");
            x = dat[1].toDouble(&ok1);
            y = dat[2].toDouble(&ok2);
            if(!ok1 || !ok2)
                line = ts_dat.readLine();
            if(ret == QMessageBox::Yes) {
                if(robot_scale != 0.0) {// esto es un apano para las trayectoria grabadas de Aria, que van en coordenadas reales
//                    x = (x/robot_scale  + worldXMin)*world_scale;
                    x = x/robot_scale;
                    if(inverseY)
//                        y = -(worldHeight + worldYMin - y/robot_scale)*world_scale;
                        y = -(worldHeight + 2*worldYMin - y/robot_scale);
                    else
//                        y = (y/robot_scale  + worldYMin)*world_scale;
                        y = y/robot_scale;
                }
                else if(inverseY)
                    y = -y;
            }

            stroke.addPoint(x, y);
            n_points++;
        }
        line = ts_dat.readLine();
    }
    if(!n_points) {
//        QMessageBox::critical(nullptr, tr("exportPath"),
//                              tr("Empty path file"), tr("&Cancel"));
        QMessageBox::critical(nullptr, tr("exportPath"),
                              tr("Empty path file"), QMessageBox::Cancel);
        return;
    }

    QDomDocument doc = QDomDocument();
    QDomElement rootElement = doc.createElement("svg");
    doc.appendChild(rootElement);

    QDomAttr id = doc.createAttribute("id");
    id.setValue("svg");
    rootElement.setAttributeNode(id);
    QDomAttr height = doc.createAttribute("height");
    height.setValue(QString::number(worldHeight) + "cm");
    rootElement.setAttributeNode(height);
    QDomAttr width = doc.createAttribute("width");
    width.setValue(QString::number(worldWidth) + "cm");
    rootElement.setAttributeNode(width);
    QDomAttr viewBox = doc.createAttribute("viewBox");
    viewBox.setValue(QString::number(worldXMin) + " " + QString::number(worldYMin) + " " +
                     QString::number(worldWidth) + " " + QString::number(worldHeight));
    rootElement.setAttributeNode(viewBox);

    QDomElement gElement = doc.createElement("g");
    rootElement.appendChild(gElement);
    QDomAttr fill = doc.createAttribute("fill");
    fill.setValue("none");
    gElement.setAttributeNode(fill);
    QDomAttr strokeColor = doc.createAttribute("stroke");
    strokeColor.setValue("#000000");
    gElement.setAttributeNode(strokeColor);
    QDomAttr strokeWidth = doc.createAttribute("stroke-width");
    strokeWidth.setValue(QString::number(1));
    gElement.setAttributeNode(strokeWidth);

    QDomElement pathElement = doc.createElement("path");
    gElement.appendChild(pathElement);

    stroke.createSvgPoints();
    stroke.toCubicSpline(doc, pathElement);

    // write the content into xml file
    QString xml = doc.toString(4);
    QXmlStreamWriter stream(ts_new.device());
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeCharacters("\n");
    stream.device()->write(xml.toUtf8().constData(), xml.toUtf8().length());
    stream.writeEndDocument();
}
