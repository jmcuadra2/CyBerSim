#ifndef STROKE_H
#define STROKE_H

#include <QList>
#include <QPointF>
#include <QDomDocument>
#include <QDomElement>

class Stroke
{
public:
    Stroke();
    void addPoint(double x, double y);
    void createSvgPoints();
    void toCubicSpline(QDomDocument doc, QDomElement pathElement);

private:
    QList<QPointF> points;
    QList< QList<QPointF> > svgPoints;
    QPointF initialPoint;
};

#endif // STROKE_H
