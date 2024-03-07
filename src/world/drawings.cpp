//
// C++ Implementation: drawings
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "drawings.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>

#include "../neuraldis/ndmath.h"

DrawingItem::DrawingItem(const QString& visible_color)
{
    QColor v_color(visible_color);
    if(visible_color.isEmpty() || !v_color.isValid())
        this->visible_color.setNamedColor("blue");
    else
        this->visible_color = v_color;
    
    main_color = this->visible_color;
    second_color.setRed(255 - main_color.red());
    second_color.setGreen(255 - main_color.green());
    second_color.setBlue(255 - main_color.blue());

    userVisible = false;
    validReading = false;
    drawingWidth = 5;
    drawingHeight = 5;
    x_org = 0;
    y_org = 0;
    x_end = 0;
    y_end = 0;

    x_reading = 0;
    y_reading = 0;
    l_canvas = 0;
    canvas_item = 0;

}

void DrawingItem::removeItem()
{
    if(canvas_item && l_canvas) {
        l_canvas->removeItem(canvas_item);
        canvas_item = 0;
    }
}

void DrawingItem::setReadingCoords( int x_read, int y_read) 
{ 
    x_reading = x_read;
    y_reading = y_read;
}

void DrawingItem::setSize(double width, double height)
{
    drawingWidth = width;
    drawingHeight = height;
}

void DrawingItem::setColor(QColor const& color)
{
    main_color = color;
    visible_color = main_color;
}

void DrawingItem::setSecondColor(const QString& color)
{
    QColor c_color(color);
    if(color.isEmpty() || !c_color.isValid()) {
        second_color.setRed(255 - main_color.red());
        second_color.setGreen(255 - main_color.green());
        second_color.setBlue(255 - main_color.blue());
    }
    else
        second_color = c_color;
}

void DrawingItem::resetColor(void)
{
    visible_color = main_color;
}

void DrawingItem::saturateColor(double value)
{  
    if(value < 0) {
        value = -value;
        value = value >= 2.0 ? 2.0 : value;
        if(value <= 1)
            visible_color = second_color.lighter(200 - NDMath::roundInt(value*100));
        else
            visible_color = second_color.darker(200 - NDMath::roundInt((2 - value)*100));
    }
    else {
        value = value >= 2.0 ? 2.0 : value;
        if(value <= 1)
            visible_color = main_color.lighter(200 - NDMath::roundInt(value*100));
        else
            visible_color = main_color.darker(200 - NDMath::roundInt((2 - value)*100));
    }
}

void DrawingItem::setGeometry(const int& x_org, const int& y_org, const int& x_end,
                              const int& y_end)
{
    this->x_org = x_org;
    this->y_org = y_org;
    this->x_end = x_end;
    this->y_end = y_end;
}

DrawingLine::DrawingLine(QGraphicsScene* w_canvas, const QString& visible_color) :
                DrawingItem (visible_color), QGraphicsLineItem()
{
    setZValue(1001);
    setPen(QPen(visible_color));
    setVisible(true);
    l_canvas = w_canvas;
    canvas_item = static_cast<QGraphicsLineItem*>(this);
}

DrawingLine::~DrawingLine() {
    removeItem();
}

int DrawingLine::type(void) const 
{ 
    return DRAWING_RTTI;
}

void DrawingLine::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{   
    QPen pen1(Qt::NoPen);
    p->setPen(pen1);
    if(userVisible) {
        pen1.setColor(visible_color);
        if(!validReading )
            pen1.setStyle(Qt::DashLine);
        else
            pen1.setStyle(Qt::SolidLine);
        p->setPen(pen1);
    }

    p->drawLine(x_org, y_org, x_reading, y_reading);
}

void DrawingLine::setReadingCoords( int x_read, int y_read) 
{ 
    DrawingItem::setReadingCoords(x_read, y_read);
    setLine(x_org, y_org, x_reading, y_reading);
}

void DrawingLine::setShowColor(bool visible)
{
    if(userVisible != visible) {
        userVisible = visible;
        if(userVisible) {
            QPen pen(visible_color);
            setPen(pen);
            show();
        }
        else {
            setPen(Qt::NoPen);
            hide();
        }
        update();
    }
}

DrawingEllipse::DrawingEllipse(QGraphicsScene* w_canvas, const QString& visible_color) :
                    DrawingItem (visible_color), QGraphicsEllipseItem()
{
    setZValue(1001);
    setPen(QColor(visible_color));
    setBrush(QColor(visible_color));
    setVisible(true);

    l_canvas = w_canvas;
    canvas_item = static_cast<QGraphicsEllipseItem*>(this);

    setRect(QRectF(0, 0, drawingWidth, drawingHeight));

//    setTransformOriginPoint(sceneBoundingRect().center());
}

DrawingEllipse::~DrawingEllipse()
{
    removeItem();
}

int DrawingEllipse::type(void) const 
{ 
    return DRAWING_RTTI;
}

void DrawingEllipse::setReadingCoords( int x_read, int y_read) 
{ 
    DrawingItem::setReadingCoords(x_read, y_read);

    setRect(QRectF(x_reading - drawingWidth/2.0, y_reading - drawingHeight/2.0, drawingWidth, drawingHeight));

}

void DrawingEllipse::paint(QPainter *p, const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    if(userVisible) {
        if(validReading)
            p->setBrush(visible_color);
        else
            p->setBrush(Qt::NoBrush);
        p->drawEllipse(x_reading - drawingWidth/2.0, y_reading - drawingHeight/2.0,
                       drawingWidth, drawingHeight);

    }
}

void DrawingEllipse::setShowColor(bool visible)
{
    if(userVisible != visible) {
        userVisible = visible;
        if(userVisible) {
            show();
        }
        else {
            hide();
        }
        update();
    }
}

DrawingRectangle::DrawingRectangle(QGraphicsScene* w_canvas, const QString& visible_color) :
                DrawingItem (visible_color), QGraphicsRectItem()
{
    setZValue(1001);
    setBrush(QBrush(QColor(visible_color)));
    setVisible(true);
    l_canvas = w_canvas;
    canvas_item = static_cast<QGraphicsRectItem*>(this);

    setRect(QRectF(0, 0, drawingHeight, drawingWidth));

//    setTransformOriginPoint(sceneBoundingRect().center());
}

DrawingRectangle::~DrawingRectangle() {
    removeItem();
}


int DrawingRectangle::type(void) const 
{ 
    return DRAWING_RTTI;
}

void DrawingRectangle::setReadingCoords( int x_read, int y_read) 
{ 
    DrawingItem::setReadingCoords(x_read, y_read);

    setRect(QRectF(x_reading - drawingWidth/2.0, y_reading - drawingHeight/2.0, drawingWidth, drawingHeight));

}

void DrawingRectangle::paint(QPainter* p, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if(userVisible) {
        p->setPen(Qt::NoPen);
        if(validReading)
            p->setBrush(visible_color);
        else
            p->setBrush(Qt::NoBrush);
        p->drawRect(QRectF(x_reading - drawingWidth/2.0, y_reading - drawingHeight/2.0,
                    drawingWidth, drawingHeight));
    }
}

void DrawingRectangle::setShowColor(bool visible)
{
    if(userVisible != visible) {
        userVisible = visible;
        if(userVisible) {
            show();
        }
        else {
            hide();
        }
        update();
    }
}

DrawingPath::DrawingPath(QGraphicsScene* w_canvas, const QString& visible_color, int length) :
                    DrawingItem(visible_color)

{
    this->length = length;
    l_canvas = w_canvas;
    userVisible = true;
}

DrawingPath::~DrawingPath()
{
    for(int i = 0; i < linesQueue.size(); i++) {
        delete linesQueue[i];
    }
}

void DrawingPath::setGeometry(const int& xx_org, const int& yy_org, const int& xx_end, const int& yy_end){

    if(linesQueue.isEmpty()) {
        x_org = xx_org;
        y_org = yy_org;
        x_end = xx_end;
        y_end = yy_end;

        for(int i = 0; i < length; i++) {
            index = 0;
            DrawingLine* line = dynamic_cast<DrawingLine*>  (DrawingsFactory::factoryDrawingItem(Line, l_canvas, main_color.name(), second_color.name()));
            line->setValidReading(true);
            line->setShowColor(true);
            line->setGeometry(xx_org, yy_org, xx_end, yy_end);
            line->setReadingCoords(xx_end, yy_end);
            line->setVisible(userVisible);
            linesQueue.append(line);
        }
    }
    else {
        DrawingLine* line = linesQueue.at(index);
        line->setColor(visible_color);
        line->setGeometry(xx_org, yy_org, xx_end, yy_end);
        line->setReadingCoords(xx_end, yy_end);
        index = (index + 1)%length;
        x_end = xx_end;
        y_end = yy_end;
    }
    DrawingItem::setGeometry(x_org, y_org, xx_end, yy_end);
}

void DrawingPath::addPoint(QPoint point)
{
    setGeometry(x_end, y_end, point.x(), point.y());
}

void DrawingPath::resetPath(int x, int y)
{
    x_org = x;
    y_org = y;
    x_end = x;
    y_end = y;
    if(linesQueue.isEmpty())
        setGeometry(x, y, x, y);
    else {
        QListIterator<DrawingLine*> it(linesQueue);
        while (it.hasNext()) {
            DrawingLine* line = it.next();
            line->setGeometry(x, y, x, y);
            line->setReadingCoords(x, y);
        }
    }
    setGeometry(x, y, x, y);
}

void DrawingPath::collapse(void)
{
    if(!linesQueue.isEmpty())
        resetPath(x_end, y_end);
}


void DrawingPath::setVisible(bool on) 
{
    if(userVisible == on) return;

    userVisible = on;
    QListIterator<DrawingLine*> it(linesQueue);
    while (it.hasNext())
        it.next()->setVisible(on);
}

DrawingItem* DrawingsFactory::factoryDrawingItem(DrawingItem::Drawing typeDrawing,
                                    QGraphicsScene* w_canvas, const QString& visible_color,
                                    const QString& second_color, int length)
{
    DrawingItem* drawing = 0;
    QString name;
    if(typeDrawing == DrawingItem::Line) {
        drawing = new DrawingLine(w_canvas, visible_color);
        name = "DrawingItem::Line";
    }
    else if(typeDrawing == DrawingItem::Ellipse) {
        drawing = new DrawingEllipse(w_canvas, visible_color);
        name = "DrawingItem::Ellipse";
    }
    else if(typeDrawing == DrawingItem::Rectangle) {
        drawing = new DrawingRectangle(w_canvas, visible_color);
        name = "DrawingItem::Rectangle";
    }
    else if(typeDrawing == DrawingItem::Path) {
        drawing = new DrawingPath(w_canvas, visible_color, length);
        name = "DrawingItem::Path";
    }

    if(drawing) {
        drawing->setSecondColor(second_color);
        if(drawing->canvas_item) {
            drawing->canvas_item->setData(1, name);
            w_canvas->addItem(drawing->canvas_item);
        }
    }
    return drawing;
}

DrawingPath* DrawingsFactory::factoryDrawingPath(QGraphicsScene* w_canvas,
                                                 const QString& visible_color, int length)
{   
    DrawingPath* path = new DrawingPath(w_canvas, visible_color, length);
    path->canvasItem()->setData(1, "DrawingItem::Path");
    return path;
}
