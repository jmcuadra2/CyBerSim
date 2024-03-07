/***************************************************************************
                          inputgenerator.h  -  description
                             -------------------
    begin                : Fri Apr 29 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INPUTGENERATOR_H
#define INPUTGENERATOR_H

#include <QObject>
#include <QDomDocument>

/**
  *@author Jose M. Cuadra Troncoso
  */

class InputGenerator : public QObject  {

  Q_OBJECT
  public:
  	InputGenerator(int id, QObject *parent = 0, const char *name = 0);
  	~InputGenerator();
   
    virtual double sample(void) = 0;
    virtual void write(QDomDocument& doc, QDomElement& e);
    double Out(void) {return out ; };
    void setOut(double o) {out = o ; };
    int getIdent(void) { return ident ; };

  signals:
    void signalOut(double, int);

  protected:
    double out, prev_out;
    int ident;     
};


#endif
