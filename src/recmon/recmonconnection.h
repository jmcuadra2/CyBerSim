/***************************************************************************
 *   Copyright (C) 2004 by jose manuel                                     *
 *   jose@pcjose                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/ 

#ifndef RECMONCONNECTION_H
#define RECMONCONNECTION_H

#include <QObject>
#include <QDomDocument>

/**
  *@author Jose M. Cuadra Troncoso
  */

class RecMonConnection : public QObject
{
	Q_OBJECT
	
	public:
    RecMonConnection(unsigned long n_id, bool recmon = false,
                      QObject *parent = 0, const char *name = 0);

    ~RecMonConnection();

    virtual bool edit(bool mode = true, bool added = false) = 0;
	  virtual const QString connName(bool isCap);
    virtual void write(QDomDocument& doc, QDomElement& e) = 0;
    virtual void write_constructor(QDomElement& e) = 0;
    virtual bool isRecMonable(void) { return is_recmonable ; };
    virtual void setRecMonable(bool recmon) { is_recmonable = recmon; };    
    virtual uint getNumId(void) { return num_id ; };
    virtual void setNumId(uint n_id);

  public slots:    
    virtual void emitToMonitor(void) = 0;
    virtual void emitToRecorder(void) = 0;

	signals:
		void emitMonVal(double, int);
    void emitRecVal(double, int);

  protected:
    bool is_recmonable;
 	  unsigned long num_id;    	

};

#endif
