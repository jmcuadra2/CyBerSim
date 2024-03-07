//
// C++ Interface: nomadrobot
//
// Description:
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADROBOT_H
#define NOMADROBOT_H

#include "robot.h"

#include <QPair>
#include <QQueue>

namespace Nomad{
    class Connector;
}

/**
    @author Javier Garcia Misis
*/
class NomadRobot : public Robot
{
    Q_OBJECT

public:
    NomadRobot(int n_id, QObject* parent, QString name);

    virtual ~NomadRobot();

    virtual bool okPosition();
    virtual bool stop();
    virtual const QString type();
    virtual double getRotation();
    virtual double getX();
    virtual double getY();
    virtual void advance(int stage);
    void* getClient(void) {return  (void*)connector_ ; }
    virtual void initSim(bool reset);
    virtual void outputMotors();
    virtual void setMainMotor();

    virtual void getKeyEvent(QKeyEvent* event, bool key_on);
    virtual void getMouseEvent(QMouseEvent* e);

    bool Connet(int id, QString host, QString port, bool direct, bool create);
    bool switchToReal();
    bool switchtoSim();
    bool Talk();

    void setMaxRot(double m_vel){ max_rot_ = m_vel;}
    double getMaxRot(void) { return max_rot_ ; }

    void MostrarVentana();

    void setInitRot(double val){ initRot_ = val;}
    void setInitX(double val){ initX_ = val;}
    void setInitY(double val){ initY_ = val;}

public slots:
    void command(QString cmd, int value);

signals:
    void setVelocity(double vel, double turret, double ster);
    void setPower(double motor, double system);
    void close();
    void setReal(bool val);
    void setFixed(bool val);

protected:
    void setCanvas(QGraphicsScene* w_canvas);

private:
    Nomad::Connector * connector_;
    QString host_;
    unsigned int port_;
    bool direct_;
    double max_rot_;
    QQueue< QPair<QString,int> > comandos_;
    bool fixed_;
    double initX_;
    double initY_;
    double initRot_;
    static QString TYPE_NAME;
};

#endif
