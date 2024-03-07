/***************************************************************************
                          radialfieldsource.cpp  -  description
                             -------------------
    begin                : Mon Feb 7 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *
 *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "radialfieldsource.h"
#include "rtti.h"
#include "../neuraldis/settings.h"

#include <QGraphicsItem>
#include <QSvgWidget>
#include <QSvgRenderer>
#include <QPainter>

#include "../neuraldis/ndmath.h"

RadialFieldSource::RadialFieldSource(int x, int y, int scale, const QString& nameId, const QString& str_radialfieldsource_svg, QGraphicsItem * parent) : QGraphicsSvgItem(str_radialfieldsource_svg, parent)
{

    this->nameId = nameId;
    int pix_width = NDMath::roundInt(boundingRect().width()/(double)scale);
    pix_width = pix_width < 10 ? 10 : pix_width;
    int pix_height= NDMath::roundInt(boundingRect().height()/(double)scale);
    pix_height = pix_height < 10 ? 10 : pix_height;

    setX(x - pix_width/2.0);
    setY(y - pix_height/2.0);
    setZValue(1100);
    on = true;
}

RadialFieldSource::~RadialFieldSource()
{

}

int RadialFieldSource::type(void) const
{   
    return RADIALFIELDSOURCE_RTTI;
}

bool RadialFieldSource::isOn(void)
{
    return on;
}

void RadialFieldSource::setOn(bool on_)
{
    on = on_;
}

double  RadialFieldSource::xCenter(void)
{
    return x() + boundingRect().width()/2.0;
}

double  RadialFieldSource::yCenter(void)
{
    return y() + boundingRect().height()/2.0;
}
