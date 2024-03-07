//
// C++ Interface: editlinedialog
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITLINEDIALOG_H
#define EDITLINEDIALOG_H

#include <qewextensibledialog.h>

#include "ui_editlinedialog.h"
#include "../material.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

class Material;

class editLineDialog : public QewExtensibleDialog, public Ui::editLineDialog
{
    Q_OBJECT

public:
    editLineDialog(QWidget *parent = 0);
    ~editLineDialog();

    //     void setParams(double w_rand = 0.0, double a_hit = 0.0, double a_max = 0.0, transparency = 0.0);
    virtual void init(Material * mat);

public slots:
    void setW_Rand(double d);
    void setA_Hit(double d);
    void setA_Max(double d);
    int getSensorType(QString id);
    void setTransparency(double t);

protected:
    double w_rand;
    double a_hit;
    double a_max;
    double transparency;
    Material* material;
    QString idName;

    void saveThis(void);
};

#endif
