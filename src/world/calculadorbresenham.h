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


#ifndef CALCULADORBRESENHAM_H
#define CALCULADORBRESENHAM_H
#include <QList>
#include <QPoint>

class CalculadorBresenham
{

public:
    CalculadorBresenham();
    ~CalculadorBresenham();
    QList<QPoint> BresenhamLine(int x0, int y0, int x1, int y1);
    QList<QPoint> BresenhamLine(int x0, int y0, int x1, int y1, float wd);
    
    QList<QPoint> BresenhamLine(double x0, double y0, double x1, double y1);
    
    QList<QPoint> BresenhamCircle(int x0, int y0, int radius);
    QList<QPoint> BresenhamElipse(int xm, int ym, int a, int b);
    void setParameters(QPoint origin, int cellSize);

private:
    int cellSize;
    QPoint origin;
};

#endif // CALCULADORBRESENHAM_H
