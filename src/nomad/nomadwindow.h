//
// C++ Interface: nomadwindow
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADWINDOW_H
#define NOMADWINDOW_H


#include "../neuraldis/mdisubwininterface.h"
#include "ui_nomadwindow.h"

/**
    @author Javier Garcia Misis
*/
namespace Nomad{

class Window : public QWidget, public MdiSubWinInterface, public Ui::nomadwindow
{
    Q_OBJECT
public:
    Window(QWidget *parent = 0);
    ~Window();

public slots:
    void setVelocity(double vel, double turret, double ster);
    void setPower(double motor, double system);
    void setReal(bool val);

    virtual void BotonTalk();
    virtual void BotonStop();
    virtual void BotonZR();
    virtual void Simulado(bool value);
    virtual void fixedTurret(bool value);

signals:
    void command(QString cmd, int value);


protected:
    virtual void keyPressEvent ( QKeyEvent * keyEvent );
};


};

#endif
