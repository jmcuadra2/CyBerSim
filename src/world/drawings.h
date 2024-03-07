//
// C++ Interface: drawings
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DRAWINGS_H
#define DRAWINGS_H

#include <QColor>
#include <QGraphicsScene>
#include <QList>
#include <QGraphicsItem>

#include "rtti.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

class DrawingItem 
{
protected:
    DrawingItem(const QString& visible_color);

public:
    virtual ~DrawingItem() {}
    enum Drawing {NoDrawing = -1, Line = 0, Ellipse = 1, Rectangle = 2, Path = 3};
    
    virtual void setVisible(bool on) = 0;
    void setValidReading(bool valid) { validReading = valid ; }
    bool isValidReading(void) { return validReading ; }
    virtual void setGeometry(const int& x_org, const int& y_org,
                             const int& x_end, const int& y_end);
    virtual void setShowColor(bool visible) = 0;
    virtual void setReadingCoords( int x_read, int y_read);
    virtual void addPoint(QPoint /*point*/) {}

    QGraphicsItem* canvasItem(void) {return canvas_item;}
    bool isUserVisible(void) const { return userVisible ; }
    void setColor(QColor const& color);
    void setSecondColor(const QString& color);
    void resetColor(void);
    void saturateColor(double value);
    
    void setSize(double width, double height);
    
    inline int getXReading() const { return x_reading; }

    inline int getYReading() const { return y_reading; }

protected:
    void removeItem();
    
protected:
    friend class DrawingsFactory;
    QGraphicsScene* l_canvas;
    QGraphicsItem* canvas_item;
    QColor visible_color;
    QColor main_color, second_color;
    bool userVisible;
    bool validReading;
    int x_org, y_org, x_end, y_end, x_reading, y_reading;
    double drawingHeight, drawingWidth;
};

class DrawingLine : public DrawingItem, public QGraphicsLineItem
{
protected:
    DrawingLine(QGraphicsScene* w_canvas, const QString& visible_color);

public:
    ~DrawingLine();
    void setVisible(bool on) { QGraphicsLineItem::setVisible(on) ; }
    void setReadingCoords( int x_read, int y_read);
    void setShowColor(bool visible);
    int type(void) const;
    void paint (QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
protected:
    friend class DrawingsFactory;
    friend class DrawingPath;
};

class DrawingEllipse : public DrawingItem, public QGraphicsEllipseItem
{
protected:
    DrawingEllipse(QGraphicsScene* w_canvas, const QString& visible_color);

public:
    ~DrawingEllipse();
    void setVisible(bool on) { QGraphicsEllipseItem::setVisible(on) ; }
    void setShowColor(bool visible);
    int type(void) const;
    void paint (QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setReadingCoords( int x_read, int y_read);
    
protected:
    friend class DrawingsFactory;
};

class DrawingRectangle : public DrawingItem, public QGraphicsRectItem
{
protected:
    DrawingRectangle(QGraphicsScene* w_canvas, const QString& visible_color);
    
public:
    ~DrawingRectangle();
    void setVisible(bool on) { QGraphicsRectItem::setVisible(on) ; }
    void setShowColor(bool visible);
    int type(void) const;
    void paint (QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setReadingCoords( int x_read, int y_read);
    
protected:
    friend class DrawingsFactory;
};


class DrawingPath : public DrawingItem
{

    friend class DrawingsFactory;

protected:
    DrawingPath(QGraphicsScene* w_canvas, const QString& visible_color, int length = 20);
    
public:
    ~DrawingPath();
    void setVisible(bool on);
    void setShowColor(bool /*visible*/) {}
    void addPoint(QPoint point);
    void setGeometry(const int& x_org, const int& y_org, const int& x_end, const int& y_end);
    void resetPath(int x, int y);
    void collapse(void);

protected:
    int length;
    int index;
    QList<DrawingLine *> linesQueue;
    
};

class DrawingsFactory
{

public:
    static DrawingItem* factoryDrawingItem(DrawingItem::Drawing typeDrawing, QGraphicsScene* w_canvas,
                                           const QString& visible_color,
                                           const QString& second_color, int length = 0);
    static DrawingPath* factoryDrawingPath(QGraphicsScene* w_canvas,
                                           const QString& visible_color, int length = 30);
    
};

#endif
