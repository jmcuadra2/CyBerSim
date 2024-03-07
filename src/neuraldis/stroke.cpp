#include "stroke.h"

Stroke::Stroke()
{

}

void Stroke::addPoint(double x, double y)
{
    points.append(QPointF(x, y));
}

void Stroke::createSvgPoints()
{
    QList<QPointF> p;
    QList<QPointF> svgP;
    double x;
    double y;

    svgPoints.clear();

    initialPoint.setX(points.at(0).x());
    initialPoint.setY(points.at(0).y());

    for(int i = 0; i < points.size() - 1; i++) {
        if(i == 0)
            p.append(QPointF(points.at(i).x(), points.at(i).y()));
        else
            p.append(QPointF(points.at(i-1).x(), points.at(i-1).y()));
        p.append(QPointF(points.at(i).x(), points.at(i).y()));
        p.append(QPointF(points.at(i+1).x(), points.at(i+1).y()));
        if(i >= points.size() - 2)
            p.append(QPointF(points.at(points.size() - 1).x(), points.at(points.size() - 1).y()));
        else
            p.append(QPointF(points.at(i+2).x(), points.at(i+2).y()));

        // Catmull-Rom to Cubic Bezier conversion matrix
        //    0       1       0       0
        //  -1/6      1      1/6      0
        //    0      1/6      1     -1/6
        //    0       0       1       0

        x = (-p.at(0).x() + 6 * p.at(1).x() + p.at(2).x()) / 6;
        y = (-p.at(0).y() + 6 * p.at(1).y() + p.at(2).y()) / 6;
        svgP.append(QPointF(x, y));

        x = (p.at(1).x() + 6 * p.at(2).x() - p.at(3).x()) / 6;
        y = (p.at(1).y() + 6 * p.at(2).y() - p.at(3).y()) / 6;
        svgP.append(QPointF(x, y));

        x = p.at(2).x();
        y = p.at(2).y();
        svgP.append(QPointF(x, y));

        svgPoints.append(svgP); //.clone()

        svgP.clear();
        p.clear();
    }
    points.clear();
}

void Stroke::toCubicSpline(QDomDocument doc, QDomElement pathElement)
{
    if(svgPoints.isEmpty())
        return;
    QString catmull = "M " + QString::number(initialPoint.x()) + "," +
            QString::number(initialPoint.y()) + " ";
    for(int i = 0; i < svgPoints.size(); i++) {
        catmull += "C " + QString::number(svgPoints.at(i).at(0).x()) + "," +
                QString::number(svgPoints.at(i).at(0).y()) + " " +
                QString::number(svgPoints.at(i).at(1).x()) + "," +
                QString::number(svgPoints.at(i).at(1).y()) + " " +
                QString::number(svgPoints.at(i).at(2).x()) + "," +
                QString::number(svgPoints.at(i).at(2).y());
        if(i < svgPoints.size() - 1)
             catmull += " ";
    }

    QDomAttr d = doc.createAttribute("d");
    d.setValue(catmull);
    pathElement.setAttributeNode(d);
}
