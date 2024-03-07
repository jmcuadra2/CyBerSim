/***************************************************************************
                          leyend.cpp  -  description
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

#include <cmath>

#include <QPainter>
#include <QPixmap>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QTextStream>

#include "leyend.h"

Leyend::Leyend(QVector<double> weightsM, QStringList synapse_col,
               QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    leyendPix = QPixmap(238, 100);
    setWindowTitle(tr("Weights legend"));
    setGeometry(32, 460, 240, 102);
    QPalette pal(palette());
    pal.setColor(QPalette::Window, QColor("gainsboro"));
    setAutoFillBackground(true);
    setPalette(pal);
    weightsMask = weightsM;
    synapse_colors = synapse_col;
}

Leyend::~Leyend()
{

}

void Leyend::createLeyend(void)
{

    int i, ent , x, y;
    int n_weights = weightsMask.size();
    double frac;
    QString sent, sfrac;
    int n_dec = 4, mant = 3;
    QPen  pen(Qt::black, 1);
    QFont font("Times", 10);
    QPainter p(&leyendPix);
    p.setPen( pen );
    p.setFont(font);
    p.save();
    leyendPix.fill(QColor("gainsboro"));

    for(i=0; i<n_weights; i++) {
        if(weightsMask[i] < 0) {
            ent = int(ceil(weightsMask[i]));
            frac=fabs(weightsMask[i] - ent) * pow(10,n_dec);
        }
        else {
            ent = int(floor(weightsMask[i]));
            frac=(weightsMask[i] - ent) * pow(10,n_dec);
        }
        sent = QString::number(ent);
        sfrac = QString::number(frac);
        if(weightsMask[i] < 0 && weightsMask[i] > -1)
            sent.insert(0, '-');
        p.save();
        p.setBrush(QBrush(QColor(synapse_colors[6 + i])));
        x =  60 * (i/8) + 2;
        y = 12 * (i%8) + 2;
        p.drawRect(x, y, 10, 10 );
        p.drawText(x + 12, y + 10,
                   QString("%1.%2").arg(sent.rightJustified(mant, ' ')).
                   arg(sfrac.rightJustified(n_dec, '0')));
        p.restore();
    }
    p.restore();
    p.end();

}

void Leyend::paintEvent( QPaintEvent * )
{

    QPainter painter(this);
    painter.drawPixmap(0, 0, leyendPix);

}

void Leyend::closeEvent(QCloseEvent *e)
{
    emit closeLeyend();
    e->ignore();

}

void Leyend::updateLeyend(void)
{
    createLeyend();
    repaint();
}

void Leyend::updateLeyend(int id_color, double weight)
{

    int ent, x, y;
    double frac;
    QString sent, sfrac;
    int n_dec = 4, mant = 3;
    QPen  pen(Qt::black, 1);
    QFont font("Times", 10);
    QPainter p(&leyendPix);

    p.setPen( pen );
    p.setFont(font);
    p.save();

    if(weight < 0) {
        ent = int(ceil(weight));
        frac=fabs(weight - ent) * pow(10,n_dec);
    }
    else {
        ent = int(floor(weight));
        frac=(weight - ent) * pow(10,n_dec);
    }
    sent = QString::number(ent);
    sfrac = QString::number(frac);
    if(weight < 0 && weight > -1)
        sent.insert(0, '-');
    p.save();
    x =  60 * (id_color/8) + 2;
    y = 12 * (id_color%8) + 2;
    p.fillRect(x + 12, y, 47, 10, QColor("grey82"));
    p.drawText(x + 12, y + 10,
               QString("%1.%2").arg(sent.rightJustified(mant)).
               arg(sfrac.rightJustified(n_dec, '0')));
    p.restore();
    repaint();
}

void Leyend::export_leyend(QTextStream &ts)
{

    int i, ent;
    int x_cuad, y_cuad,l_cuad, x_sep_cuad, y_sep_cuad;
    int  h_text, l_text, x_sep_text, y_sep_text;
    int marg;
    int n_weights = weightsMask.size();
    double frac;
    QString sent, sfrac;
    QString ini_cuad1, ini_cuad2, ini_text, ini_titl, ini_marco1, ini_marco2, ini_lin;
    int n_dec = 4, mant = 3;

    ini_cuad1 =  "2 2 0 1 0 ";
    ini_cuad2 =  " 45 -1 20 0.000 0 0 -1 0 0 5\n\t";
    l_cuad = 120;
    x_sep_cuad = 920;
    y_sep_cuad = 175;
    ini_text = "4 2 0 45 -1 0 10 0.0000 4 ";
    h_text = 105;
    x_sep_text = 750;
    y_sep_text = (l_cuad + h_text)/2;
    ini_titl = "4 1 0 45 -1 2 12 0.0000 4 135 510 ";
    ini_marco1 = "2 4 0 1 0 ";
    ini_marco2 = " 45 -1 20 0.000 0 0 5 0 0 5\n\t";
    ini_lin = "2 1 0 1 0 7 45 -1 -1 0.000 0 0 -1 0 0 2\n\t";
    marg = 100;

    ts << ini_marco1 << n_weights + 32 + 1 << ini_marco2;
    ts << ORIG_X - marg << " " << ORIG_Y - marg - 200 << " ";
    ts << ORIG_X - marg << " " << ORIG_Y + marg + 1300 << " ";
    ts << ORIG_X + marg + 3500 << " " << ORIG_Y + marg + 1300 << " ";
    ts << ORIG_X + marg + 3500 << " " << ORIG_Y - marg - 200 << " ";
    ts << ORIG_X - marg << " " << ORIG_Y - marg - 200 << "\n";

    ts << ini_lin << ORIG_X - marg << " " << ORIG_Y - marg - 200 + 235 << " ";
    ts << ORIG_X + marg + 3500 << " " << ORIG_Y - marg - 200 + 235 << "\n";

    ts << ini_titl << ORIG_X + 3500/2 << " " << ORIG_Y - marg - 200 + 235/2 + 135/2 << " ";
    ts << QString(tr("Weights")) << "\\001" << "\n";

    for(i=0; i<n_weights; i++) {
        if(weightsMask[i] < 0) {
            ent = int(ceil(weightsMask[i]));
            frac=fabs(weightsMask[i] - ent) * pow(10,n_dec);
            if(weightsMask[i] <= -10)
                l_text = 555;
            else
                l_text = 480;
        }
        else {
            ent = int(floor(weightsMask[i]));
            frac=(weightsMask[i] - ent) * pow(10,n_dec);
            if(weightsMask[i] >= 10)
                l_text = 495;
            else
                l_text = 420;
        }
        sent = QString::number(ent);
        sfrac = QString::number(frac);
        if(weightsMask[i] < 0 && weightsMask[i] > -1)
            sent.insert(0, '-');
        x_cuad =  ORIG_X + x_sep_cuad * (i/8);
        y_cuad = ORIG_Y + y_sep_cuad * (i%8);

        ts << ini_cuad1 << i + 32 << ini_cuad2 << x_cuad << " " << y_cuad << " " << x_cuad + l_cuad << " ";
        ts << y_cuad << " " << x_cuad + l_cuad << " " << y_cuad + l_cuad << " " << x_cuad << " " << y_cuad + l_cuad << " ";
        ts << x_cuad << " " << y_cuad << "\n";

        ts << ini_text << h_text << " " << l_text << " " << x_cuad + x_sep_text << " " << y_cuad + y_sep_text << " " ;
        ts << QString("%1.%2").arg(sent.rightJustified(mant)).arg(sfrac.rightJustified(n_dec, '0')) << "\\001" << "\n";
    }

}
