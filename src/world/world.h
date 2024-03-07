/***************************************************************************
                          world.h  -  description
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

#ifndef WORLD_H
#define WORLD_H

#include <QStringList>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QPixmap>
#include <QDomDocument>

#include "abstractworld.h"

#define ALIG_LEFT     0
#define ALIG_CENTR      1
#define ALIG_RIGHT      2

#define SCALE_FIG_2_PIXEL       10  // unidades xfig por pixel

/**
  *@author Jose M. Cuadra Troncoso
  */

class World : public AbstractWorld
{

    Q_OBJECT

    friend class WorldFactory;
    
protected:
    World( QMdiArea *w_work = 0, QObject *parent=0, const char *name=0);
    
public:
    ~World();

public slots:
    void setRobotRadius(const double& rad);

protected:
    bool load(QTextStream& ts);
    void clear(void);

    bool readTextProps(QStringList dat) ;
    void readCommonProps(QStringList dat);
    void addColorObject(QStringList dat);
    void readEllipseProps(QStringList dat);
    void readPolygonProps(QStringList dat, QTextStream &ts);
    void readSplineProps(QStringList dat, QTextStream &ts);
    bool readLampProps(QStringList dat, QTextStream &ts);
    QList<QPixmap> drawEllipsePix(void);
    void addText(int x, int y, uint pen_color,
                 float depth, float font_size, uint subtype, QString texto);

protected:
    QList<QPixmap*> ellyp_pix_list;
    QPolygon pa;
       
    //Fig format
    uint color_num, c_col, subtype;
    QString color, line;
    int object, pen_color, fill_color, area_fill;
    int x, y, radius_x, radius_y;
    uint thickness, n_points;
    float depth, angle, font_size, text_height;
    double center_x, center_y;
    QString texto;

    // Simulador format
    uint lamp_radius;

};

#endif
