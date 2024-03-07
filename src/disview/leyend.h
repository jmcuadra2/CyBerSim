/***************************************************************************
                          leyend.h  -  description
                             -------------------
    begin                : mar dic 24 2002
    copyright            : (C) 2002 by Jose M. Cuadra Troncoso
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

#ifndef LEYEND_H
#define LEYEND_H

#include <QWidget>
#include <QPixmap>
#include <QStringList>
#include <QPaintEvent>
#include <QTextStream>
#include <QCloseEvent>

#define ORIG_X   200
#define ORIG_Y   7500

/**
  *@author Jose M. Cuadra Troncoso
  */

class Leyend : public QWidget
{
    Q_OBJECT

public:
    Leyend(QVector<double> weightsM, QStringList synapse_col,
           QWidget *parent=0, Qt::WindowFlags f = Qt::Dialog);
    ~Leyend();

    void createLeyend(void);
    void export_leyend(QTextStream &ts);

public slots:
    void updateLeyend(void);
    void updateLeyend(int id_color, double weight);

protected slots:
    void paintEvent( QPaintEvent * );
    void closeEvent(QCloseEvent *e);

signals:
    void closeLeyend(void);

protected:
    QPixmap leyendPix;
    QStringList synapse_colors;
    QVector<double> weightsMask;

};

#endif
