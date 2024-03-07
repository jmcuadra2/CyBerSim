//
// C++ Interface: animatedpixmapitem
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ANIMATEDPIXMAPITEM_H
#define ANIMATEDPIXMAPITEM_H

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
#include <QGraphicsItem>

class AnimatedPixmapItem : public QGraphicsItem
{
public:
    AnimatedPixmapItem(const QList<QPixmap *> &animation);

    void setFrame(int frame);
    inline int frame() const
    { return currentFrame; }
    inline int frameCount() const
    { return frames.size(); }
    inline QPixmap image(int frame) const
    { return frames.isEmpty() ? QPixmap() : frames.at(frame % frames.size()).pixmap; }
    inline void setVelocity(qreal xvel, qreal yvel) //TODO: creo que no necesito nada relacionado con la velocidad
    { vx = xvel; vy = yvel; }
    inline qreal xVelocity() const
    { return vx; }
    inline qreal yVelocity() const
    { return vy; }

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
    void advance(int phase);
    
private:
    struct Frame {
        QPixmap pixmap;
        QPainterPath shape;
        QRectF boundingRect;
    };
    
    int currentFrame;
    QList<Frame> frames;
    qreal vx, vy;
};

#endif
