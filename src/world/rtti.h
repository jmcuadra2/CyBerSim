/***************************************************************************
                          rtti.h  -  description
                             -------------------
    begin                : Wed Dec 17 2003
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

#define RADIALFIELDSOURCE_RTTI					    20000
#define LAMP_RTTI					 RADIALFIELDSOURCE_RTTI*100 + 1
#define FIRST_COLLISION_RTTI  100000
#define ELLIPSE_RTTI			    100001
#define POLYGON_RTTI			    100002
#define RECTANGLE_RTTI        100003
#define ROBOT_RTTI			    77777
#define SPOINT_RTTI 			    300050
#define LINE_RTTI             100004
#define LAST_COLLISION_RTTI   200000
#define DRAWING_RTTI          600000
#define REAL_TIME_ADV_PERIOD  20    // ms.

#define COLOR           0
#define ELLIPSE         1
#define ELLIPSE_DIAM    1
#define POLYLINE        2
#define POLYGON         3
#define LINE            1
#define BOX             2
#define PICTURE         5
#define TEXT            4
#define SPLINE          3
#define ARC             5
#define COMPOUND        6
#define END_COMPOUND   -6
#define LAMP            7
