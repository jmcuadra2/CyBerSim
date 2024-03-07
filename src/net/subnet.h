/***************************************************************************
                         subnet.h  -  description
                             -------------------
    begin                : July 8 2007
    copyright            : (C) 2007 by Mº Dolores Gómez Tamayo
    email                : mgomez4@alumno.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SUBNET_H
#define SUBNET_H

#include <qewdialogfactory.h>

#include <QPixmap>
#include <QList>
#include "../net/animatedpixmapitem.h"

#include "neuron.h"
#include "synapse.h"
#include "subnetsynapse.h"

class SubNetSynapse;

// nueva clase para las subredes     
class SubNet: public QObject 
{
    Q_OBJECT

public:
    SubNet(unsigned long n_id, int lay,const QString& path, const QString& filename);
    SubNet(unsigned long n_id);
    void initValues(void);

    virtual ~SubNet();

    // hay que definir todos los métodos que necesitemos para crear la subred
    int rtti_dev(void) const;
    bool setParams(const QDomElement& e);
    void write(QDomDocument& doc, QDomElement& e);

    // métodos para indicar el path y fichero
    const QString& getPath(void);
    virtual void setPath(const QString path);
    const QString& getFileName(void);
    virtual void setFileName(const QString filename);
    void setFile (QString path, QString file);

    virtual bool getSelected(void);
    virtual void setSelected(bool sel);

    void setPix(QString pix_file, QString pix_sel_file);

    // bool edit(bool mode = true, bool adding = false);

    // nuevo para los axones
    QList<SubNetSynapse *> & getAxon(void) { return ExternalSynapses ; }
    void setAxon(QList<SubNetSynapse *> & axon) { ExternalSynapses = axon ; }
    bool addSynapse (SubNetSynapse *synapse);
    bool delSynapse (SubNetSynapse *synapse);

    unsigned long getNumInputNeurons(void);
    unsigned long getNumHiddenNeurons(void);
    unsigned long getNumOutputNeurons(void);


    // nuevos métodos para dibujar sinapsisconst
    int& getX(void) { return x ; }
    void setX(const int& x_) { x = x_ ; }
    const int& getY(void) { return y ; }
    void setY(const int& y_) { y = y_ ; }
    virtual int getWidth(void) const { return width; }
    virtual int getHeight(void) const { return height; }
    unsigned long getNumId(void) { return id; }

    const QString& displayInfo(void) { return infoWhatsThis; }
    void draw(QGraphicsView* view);
    void drawSynapses (QGraphicsView* view);
    // inicialización de subobjetos nuevo
    
    virtual void initSubObjects(const QDomElement& e);
    
    // método para leer estructura de la red y obtener num. de neuronas de cada capa
    void readSubNet(void);
    

public slots:
    void num_idChange(ulong prev_n_i);


signals:
    void changed(void);
    // esta señal es de recmondevice pero como no forma parte de simulación la indico aquí
    // debería derivar de RecmonDevice esta clase preguntar???
    void num_idChanged(ulong num_id);

protected:
    QewExtensibleDialog* createDialog(bool mode, bool added);
    // para escribir los subobjetos dentro de la subred
    void write_subobjects(QDomDocument& doc, QDomElement& e);
    
protected:
    bool selected;
    SubNet* sub_net;
    QString file_name;
    QString path_file;

    unsigned long id;

    // estructura basica (num neuronas en cada capa)
    unsigned long num_InputN;
    unsigned long num_HiddenN;
    unsigned long num_OutputN;

    // por si en el futuro hay varios tipos de subredes
    int type;
    // nuevo para tratar los axones

    QList<SubNetSynapse *> ExternalSynapses;

    QString infoWhatsThis;

    // nuevas variables para sinapsis entre subredes
    int x;
    int y;
    int width, height;

    QList<QPixmap> listpixmaps;
    QPixmap *qc_pix, *qc_pix_sel;
    QList<QPixmap *> qc_pix_a;
    AnimatedPixmapItem *qc_subnet_pix;
    QGraphicsSimpleTextItem *qc_texto;

};


#endif
