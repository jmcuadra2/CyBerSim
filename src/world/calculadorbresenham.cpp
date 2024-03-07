/*
    Copyright (c) 2014 <copyright holder> <email>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include "calculadorbresenham.h"
#include <iostream>
#include <QList>
#include <QPoint>
#include <QtGui>

CalculadorBresenham::CalculadorBresenham()
{
    cellSize = 5;
}

CalculadorBresenham::~CalculadorBresenham()
{

}

QList<QPoint> CalculadorBresenham::BresenhamCircle(int x0, int y0, int radius)
{

    QList<QPoint> bresenhamCirclePoints;
    
    x0 = (x0 - origin.x()) / cellSize;
    y0 = (y0 - origin.y()) / cellSize;

    radius = ceil((double)radius / cellSize);
    int org_radius = radius;
    
    int x = -radius, y = 0, err = 2-2*radius;
    do {
        bresenhamCirclePoints.append(QPoint(y0-y, x0+x)); /*   I. Quadrant */
        bresenhamCirclePoints.append(QPoint(y0-x, x0-y)); /*  II. Quadrant */
        bresenhamCirclePoints.append(QPoint(y0+y, x0-x)); /* III. Quadrant */
        bresenhamCirclePoints.append(QPoint(y0+x, x0+y));

        radius = err;
        if (radius <= y) err += ++y*2+1;
        if (radius > x || err > y)
            err += ++x*2+1;
    } while (x < 0);
    
    x0--;
    radius = org_radius;
    x = -radius, y = 0, err = 2-2*radius;
    do {
        bresenhamCirclePoints.append(QPoint(y0-y, x0+x)); /*   I. Quadrant */
        bresenhamCirclePoints.append(QPoint(y0-x, x0-y)); /*  II. Quadrant */
        bresenhamCirclePoints.append(QPoint(y0+y, x0-x)); /* III. Quadrant */
        bresenhamCirclePoints.append(QPoint(y0+x, x0+y));

        radius = err;
        if (radius <= y) err += ++y*2+1;
        if (radius > x || err > y)
            err += ++x*2+1;
    } while (x < 0);
    
    return bresenhamCirclePoints;
}

QList<QPoint>  CalculadorBresenham::BresenhamElipse(int x0, int y0, int width, int height)

{

    QList<QPoint> bresenhamElipsePoints;
    
    x0 = (x0 - origin.x()) / cellSize;
    y0 = (y0 - origin.y()) / cellSize;
    
    width = ceil((double)width / cellSize);
    height = ceil((double)height / cellSize);

    int x = - width, y = 0;
    long bs = (long)height*height, as = (long)width*width;
    long e2 = height, dx = (1+2*x)*e2*e2;
    long dy = x*x, err = dx+dy;

    do {
        bresenhamElipsePoints.append(QPoint(y0-y, x0+x));
        bresenhamElipsePoints.append(QPoint(y0+y, x0+x));
        bresenhamElipsePoints.append(QPoint(y0+y, x0-x));
        bresenhamElipsePoints.append(QPoint(y0-y, x0-x));
        e2 = 2*err;
        if (e2 >= dx) {
            x++;
            err += dx += 2*bs;
        }
        if (e2 <= dy) {
            y++;
            err += dy += 2*as;
        }
    } while (x <= 0);

    while (y++ < height) {
        bresenhamElipsePoints.append(QPoint(y0, x0+x));
        bresenhamElipsePoints.append(QPoint(y0, x0-x));
    }

    x = - width, y = 0;
    e2 = height, dx = (1+2*x)*e2*e2;
    dy = x*x, err = dx+dy;
    x0--;

    do {
        bresenhamElipsePoints.append(QPoint(y0-y, x0+x));
        bresenhamElipsePoints.append(QPoint(y0+y, x0+x));
        bresenhamElipsePoints.append(QPoint(y0+y, x0-x));
        bresenhamElipsePoints.append(QPoint(y0-y, x0-x));
        e2 = 2*err;
        if (e2 >= dx) {
            x++;
            err += dx += 2*bs;
        }
        if (e2 <= dy) {
            y++;
            err += dy += 2*as;
        }
    } while (x <= 0);


    while (y++ < height) {
        bresenhamElipsePoints.append(QPoint(y0, x0+x));
        bresenhamElipsePoints.append(QPoint(y0, x0-x));
    }

    return bresenhamElipsePoints;
}

QList<QPoint> CalculadorBresenham::BresenhamLine(int x0, int y0, int x1, int y1)
{
    bool exactEnd = (x1 - origin.x()) % cellSize == 0 || (y1 - origin.y()) % cellSize == 0;
    bool exactStartY = (y0 - origin.y()) % cellSize == 0;
    bool exactEndY = (y1 - origin.y()) % cellSize == 0;
    
    bool exactStartX = (x0 - origin.x()) % cellSize == 0;
    bool exactEndX = (x1 - origin.x()) % cellSize == 0;

    x0 = (x0 - origin.x()) / cellSize;
    x1 = (x1 - origin.x()) / cellSize;
    y0 = (y0 - origin.y()) / cellSize;
    y1 = (y1 - origin.y()) / cellSize;
    
    bool upCell = (x0 - x1) * (y0 - y1) > 0;

    QList<QPoint> bresenhamLinePoints;
    int dx = abs(x1-x0);
    int dy = abs(y1-y0);
    int sx = 0, sy = 0;

    (x0 < x1)? sx = 1 : sx = -1;
    (y0 < y1)? sy = 1 : sy = -1;
    int err = dx-dy;

    bool ppp = dx && dy && (sx * sy < 0);
    if(ppp) {
        if(exactStartY && y0 % 2 != 0)
            y0 += sy;
        if(exactEndY && y1 % 2 != 0)
            y1 += sy;

        if(exactStartX && x0 % 2 != 0)
            x0 += sx;
        if(exactEndX && x1 % 2 != 0)
            x1 += sx;
    }

    while((x0 != x1) || (y0 != y1)) {
        bresenhamLinePoints.append(QPoint(y0, x0));
        int e2 = 2*err;
        if(e2 > -dy) {
            err = err - dy;
            if(x0 != x1)
                x0 = x0 + sx;
        }
        if(e2 > -dy && e2 <  dx) {
            if(upCell)
                bresenhamLinePoints.append(QPoint(y0 + sy, x0 - sx));
            else
                bresenhamLinePoints.append(QPoint(y0, x0));
        }
        if(e2 < dx) {
            err = err + dx;
            if(y0 != y1)
                y0 = y0 + sy;
        }
    }

    if(!exactEnd)
        bresenhamLinePoints.append(QPoint(y0, x0));

    return bresenhamLinePoints;
}

QList<QPoint> CalculadorBresenham::BresenhamLine(int x0, int y0, int x1, int y1, float wd)
{ 
    QList<QPoint> bresenhamLinePoints;

    bool exactStartX = (x0 - origin.x()) % cellSize == 0;
    bool exactEndX = (x1 - origin.x()) % cellSize == 0;
    bool exactStartY = (y0 - origin.y()) % cellSize == 0;
    bool exactEndY = (y1 - origin.y()) % cellSize == 0;

    x0 = (x0 - origin.x()) / cellSize;
    x1 = (x1 - origin.x()) / cellSize;
    y0 = (y0 - origin.y()) / cellSize;
    y1 = (y1 - origin.y()) / cellSize;
    int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;

    if(exactStartY && sy < 0 && dx != 0 && dy != 0)
        y0 += sy;
    if(exactEndY && sy < 0 && dx != 0 && dy != 0)
        y1 += sy;

    if(exactStartX && sx < 0 && dx != 0 && dy != 0)
        x0 += sx;
    if(exactEndX && sx < 0 && dx != 0 && dy != 0)
        x1 += sx;

    int err = dx-dy, e2, x2, y2;  /* error value e_xy */
    float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

    for (wd = (wd+1)/2; ; ) {  /* pixel loop */
        bresenhamLinePoints.append(QPoint(y0,x0));
        e2 = err; x2 = x0;
        if (2*e2 >= -dx) {    /* x step */
            for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx) {
                bresenhamLinePoints.append(QPoint(y2 += sy, x0));
            }
            if (x0 == x1)
                break;
            e2 = err;
            err -= dy;
            x0 += sx;
        }
        if (2*e2 <= dy) {    /* y step */
            for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy) {
                bresenhamLinePoints.append(QPoint(y0, x2 += sx));
            }
            if (y0 == y1)
                break;
            err += dx;
            y0 += sy;
        }
    }
    return bresenhamLinePoints;
}

QList<QPoint> CalculadorBresenham::BresenhamLine(double x0, double y0, double x1, double y1)
{

    QList<QPoint> bresenhamLinePoints;
    int sx = 0, sy = 0;

    bool exactStartY = false;
    bool exactEndY = false;

    bool exactStartX = false;
    bool exactEndX = false;

    if(fabs(ceil(x0) - x0) < 1e-3)
        exactStartX = (int(x0) - origin.x()) % cellSize == 0;
    if(fabs(ceil(x1) - x1) < 1e-3)
        exactEndX = (int(x1) - origin.x()) % cellSize == 0;
    if(fabs(ceil(y0) - y0) < 1e-3)
        exactStartY = (int(y0) - origin.y()) % cellSize == 0;
    if((fabs(ceil(y1) - y1) < 1e-3))
        exactEndY = (int(y1) - origin.y()) % cellSize == 0;

    x0 = (x0 - origin.x()) / cellSize;
    x1 = (x1 - origin.x()) / cellSize;
    y0 = (y0 - origin.y()) / cellSize;
    y1 = (y1 - origin.y()) / cellSize;

    double dx = fabs(x1-x0);
    double dy = fabs(y1-y0);
    (x0 < x1)? sx = 1 : sx = -1;
    (y0 < y1)? sy = 1 : sy = -1;

    if(exactStartY && sy < 0)
        y0 += sy;
    if(exactEndY && sy < 0)
        y1 += sy;

    if(exactStartX && sx < 0)
        x0 += sx;
    if(exactEndX && sx < 0)
        x1 += sx;

    dy = dy * 2;
    dx = dx * 2;

    bresenhamLinePoints.append(QPoint((int)y0, (int)x0));
    if (dx > dy) {
        float error = (2 - 2*(y0 - (int)y0)) * dy - dx;
        while (fabs(x0 - x1) > 0.5) {
            if (error >= 0) {
                y0 += sy;
                error -= dx; // same as error -= 2*dx
            }
            x0 += sx;
            error += dy; // same as error -= 2*dy
            bresenhamLinePoints.append(QPoint((int)y0, (int)x0));
        }
    }
    else {
        float error = (2  - 2*(x0 - (int)x0)) * dx - dy;
        while (fabs(y0 - y1) > 0.5) {
            if (error >= 0) {
                x0 += sx;
                error -= dy;
            }
            y0 += sy;
            error += dx;
            bresenhamLinePoints.append(QPoint((int)y0, (int)x0));
        }
    }
    return bresenhamLinePoints;
}

void CalculadorBresenham::setParameters(QPoint origin, int cellSize) {

    this->origin = origin;
    this->cellSize = cellSize ? cellSize : 1;
}
