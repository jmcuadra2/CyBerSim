/***************************************************************************
			  qcanvasarc.h  -  description
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
 
#ifndef QCANVASSELECTWIN_H
#define QCANVASSELECTWIN_H
 
#include <q3canvas.h>
#include <qpainter.h>
 

class QCanvasSelectWindow : public Q3CanvasRectangle
{
	public:
		QCanvasSelectWindow ( int x, int y, int width, int height, QString pix_dir, Q3Canvas *c );
		
		~QCanvasSelectWindow();
   
		// QPointArray areaPoints() const;
		
		// void setWidth(int width);
		// int getWidth();
		
		// void setHeight(int height);
		// int getHeight();
		
	protected:
		void drawShape( QPainter &p );
   
	private:
		
		QString pix_dir;
		Q3CanvasPixmapArray  *pixarr_move, *pixarr_zoom, *pixarr_redim;
		Q3CanvasSprite *qc_sprite_move, *qc_sprite_zoom, *qc_sprite_redim;
		
		int minimum_width;
		int minimum_height;

};

#endif
