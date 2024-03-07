/***************************************************************************
    qcanvas_select_window.h  -  description
    -------------------
    begin		 : July 24 2007
    copyright		 : (C) 2007 by Mª Dolores Gómez Tamayo
    email		 : mgomez4@alumnos.uned.es
 ***************************************************************************/

/***************************************************************************
 *									   *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or	   *
 *   (at your option) any later version.				   *
 *									   *
 ***************************************************************************/
 
#include "qcanvas_select_window.h"




QCanvasSelectWindow::QCanvasSelectWindow ( int x, int y, int width, int height, QString pix_dir, Q3Canvas *c ) :Q3CanvasRectangle( x, y, width, height, c  )
{
	// directorio para los pixmaps
	this->pix_dir = pix_dir;
	
	// move
	QString file = pix_dir+"/netview_move.xpm";
	pixarr_move = new Q3CanvasPixmapArray (file);
	qc_sprite_move = new Q3CanvasSprite (pixarr_move, c);
	
	
	// zoom
	file = pix_dir+"/netview_zoom.xpm";
	pixarr_zoom = new Q3CanvasPixmapArray (file);
	qc_sprite_zoom = new Q3CanvasSprite (pixarr_move, c);
	
	// redim
	file = pix_dir+"/netview_redim.xpm";
	pixarr_redim = new Q3CanvasPixmapArray (file);
	qc_sprite_redim = new Q3CanvasSprite (pixarr_move, c);

	
}


QCanvasSelectWindow::~QCanvasSelectWindow()
{
	hide(); // Required to avoid segfault - see docs
}





void QCanvasSelectWindow::drawShape( QPainter &p )
{
	Q3CanvasRectangle::drawShape (p);
	
	qc_sprite_move->setX ( x() );
	qc_sprite_move->setY ( y() );
	qc_sprite_move->show();
	

	
}

