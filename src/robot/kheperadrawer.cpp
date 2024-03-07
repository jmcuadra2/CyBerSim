/***************************************************************************
                          kheperadrawer.cpp  -  description
                             -------------------
    begin                : Tue Apr 12 2005
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

#include "kheperadrawer.h"
#include <QPainter>
#include <QPixmap>
#include <QSvgGenerator>
#include <QDir>

#include "../neuraldis/ndmath.h"
#include "../neuraldis/settings.h"

KheperaDrawer::KheperaDrawer()
{

}
KheperaDrawer::~KheperaDrawer()
{

}

RobotSprite* KheperaDrawer::dibujaRobotSprite(QGraphicsScene *w_canvas, const QString& s_inner_color,
                                              const QString& s_sensor_color)
{

    RobotSprite* robot_sprite = 0;
    fileName = "";

    QColor inner_color;
    QColor sensor_color;
    if(!s_inner_color.isEmpty()) {
        inner_color = QColor(s_inner_color);
        if(!inner_color.isValid())
            inner_color = QColor("#009000");
    }
    else
        inner_color = QColor("#009000");
    
    if(!s_sensor_color.isEmpty()) {
        sensor_color = QColor(s_sensor_color);
        if(!sensor_color.isValid())
            sensor_color = QColor(Qt::red);
    }
    else
        sensor_color = QColor(Qt::red);

    QSvgGenerator generator;

    fileName = QDir::cleanPath(Settings::instance()->getRobotDirectory() + QDir::separator() + "khepera.svg");
//    QString fileName = /*QDir::current().absolutePath() + */"/home/jose/cybersim/head/src/robot/imagenes/khepera.svg";
    generator.setFileName(fileName);

    int diam = NDMath::roundInt(diamtr);
    generator.setSize(QSize(diam + 4, diam + 4));
    generator.setViewBox(QRect(-1, -1, diam + 3, diam + 3));
    generator.setTitle("SVG Generator Example Drawing");
    generator.setDescription("An SVG drawing created by the SVG Generator "
                                "Example provided with Qt.");

    QPainter p; // dibuja el robot

    bool paintingDone = p.begin(&generator);
    p.translate(diamtr/2, diamtr/2);
    p.setPen(QColor("#808080"));
    p.setBrush(QColor("#808080"));
    p.drawEllipse(QRectF(-diamtr/2, -diamtr/2,
                  diamtr + 1, diamtr + 1));

    p.setPen(QColor("#a0a0a0"));
    p.setBrush(QColor("#808080"));
    p.drawEllipse(QRectF(-diamtr/2 + 1, -diamtr/2 + 1,
                  diamtr-1, diamtr-1));

    p.setPen(QColor("#c0c0c0"));
    p.setBrush(QColor("#c0c0c0"));
    p.drawEllipse(QRectF(-diamtr/2 + 2, -diamtr/2 + 2,
                  diamtr-3, diamtr-3));

    p.save();   		//  rota el interior del robot

    p.setPen(QColor("#f0f0f0"));
    p.setBrush(QColor(inner_color));
    p.drawEllipse(QRectF(-diamtr/2 + 3, -diamtr/2 + 3,
                  diamtr-5, diamtr-5));

    p.save();
    p.rotate(-90);
    p.setPen(QPen(QColor(sensor_color), 2));
    p.drawLine(QLineF(-5, diamtr/2-6, -2, diamtr/2-6)); // front vert down
    p.drawLine(QLineF(1, diamtr/2-6, 4, diamtr/2-6)); // front vert up
    p.save();
    p.rotate(45);
    p.drawLine(QLineF(-3, diamtr/2-5.5, 0, diamtr/2-5.5)); // 45 down
    p.restore();
    p.save();
    p.rotate(65);
    p.save();
    p.translate( -1, diamtr/2-6);
    p.rotate(25);
    p.drawLine(QLineF(-3, 1, 0, 1)); // horz down
    p.restore();
    p.restore();
    p.save();
    p.rotate(-45);
    p.drawLine(QLineF(-1, diamtr/2-6, 2, diamtr/2-6)); // 45 up
    p.restore();
    p.save();
    p.rotate(-65);
    p.save();
    p.translate(1, diamtr/2-6);
    p.rotate(-25);
    p.drawLine(QLineF(0, 0, 3, 0)); // horz down
    p.restore();
    p.restore();
    p.drawLine(QLineF(-6, 8-diamtr/2, -3, 8-diamtr/2)); // back vert down
    p.drawLine(QLineF(2.5, 8-diamtr/2, 5.5, 8-diamtr/2));// back vert up
    p.restore();

    p.restore();

    paintingDone = paintingDone && p.end();

    if(paintingDone) {
        robot_sprite = new RobotSprite(w_canvas, fileName);
        robot_sprite->setData(1, "RobotSprite");
    }
    else {
        fileName = "";
    }
    return robot_sprite;

}

void KheperaDrawer::setPaintingDiameter(double robot_diamter)
{
    diamtr = robot_diamter;
}
