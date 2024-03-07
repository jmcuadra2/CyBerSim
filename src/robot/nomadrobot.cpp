//
// C++ Implementation: nomadrobot
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
//conectores hacia el robot
//#include "nomad/nomadclientconnector.h"
#include "../nomad/nomadrobotconnector.h"
#include "../nomad/nomadserverconnector.h"
#include "../nomad/nomadwindow.h"
#include "../neuraldis/settings.h"

#include "nomadrobot.h"
#include "synchronousmotor.h"
#include "turretmotor.h"
#include "nomadsprite.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QMdiArea>
#include <QMdiSubWindow>


QString NomadRobot::TYPE_NAME = "RealNomadRobot";


NomadRobot::NomadRobot(int n_id, QObject* parent, QString name) :
    Robot(n_id, parent, name)
{
    host_ = "";
    port_ = 0;
    direct_ = false;
    connector_ = 0;
    max_rot_ = 0;
    initX_ = 0;
    initY_ = 0;
    initRot_ = 0;

    fixed_ = true;
}


NomadRobot::~NomadRobot()
{
    emit close();

    if (connector_ != 0){
        connector_->st();
        connector_->disconnect_robot();
        delete connector_;
        connector_ = 0;
    }
}


bool NomadRobot::Connet(int id, QString host, QString port, bool direct, bool create){

    if (connector_ != 0)//ya esta iniciado
        return false;

    bool ok;
    host_ = host;
    port_ = port.toUInt(&ok);
    direct_ = direct;

    if(!ok || port_>65535)//nuemro de puerto no valido
        return false;

    if(direct_){
        connector_ = new Nomad::RobotTCPConnector(host_.toStdString(),port_);
        emit setReal(true);
    }
    else {
        connector_ = new Nomad::ServerConnector(id,host_.toStdString(),port_);
    }

    if(create){
        if (!connector_->create_robot()){
            //no se pudo conectar o crear.
            return false;
        }
    }
    
    return connector_->connect_robot();

}

void NomadRobot::setCanvas(QGraphicsScene* w_canvas)
{

    robot_sprite->setX(0);
    robot_sprite->setY(0);
    robot_sprite->diameter = NDMath::roundInt(diamtr);
    robot_sprite->wheelsSep = 1;
    robot_sprite->rotation = rot;
    robot_sprite->parent();

    Robot::setCanvas(w_canvas);
    robot_sprite->setCollisioner(new SpriteCollisioner(robot_sprite));

}

bool NomadRobot::okPosition()
{
    return true;
}

bool NomadRobot::stop()
{    
    return true;
}

const QString NomadRobot::type()
{
    return TYPE_NAME;
}

double NomadRobot::getRotation()
{
    return world->inverseHeadingTransform(robot_sprite->getRotation());
}

double NomadRobot::getX()
{
    return world->inverseXTransform(robot_sprite->x());
}

double NomadRobot::getY()
{
    return world->inverseYTransform(robot_sprite->y());
}

void NomadRobot::advance(int stage)
{
    switch(stage){
    case 0:
        robot_sprite->advance(0);
        break;
    case 1:
        robot_sprite->advance(1);
        sensorsSet->advance(1);
        break;
    }
}

void NomadRobot::initSim(bool reset)
{
    initSensors();
    robot_sprite->clearPath(reset);
    //inicializamos la posicion.
    connector_->zr();
    comandos_.clear();
}

void NomadRobot::outputMotors()
{
    TurretMotor * motorT = dynamic_cast<TurretMotor *>(motors.value("Turret",0));
    SynchronousMotor * motor = dynamic_cast<SynchronousMotor *>(mainMotor);

    while (!comandos_.isEmpty()){
        QPair<QString,int> pair;
        pair = comandos_.dequeue();

        if(pair.first == "ZR"){
            connector_->zr();
        }else if(pair.first == "FIXED"){
            bool bval = (bool) pair.second;
            if (fixed_ != bval){
                fixed_ = bval;
                if(fixed_){
                    //sincronizamos la posicion de las ruedas con la torreta.
                    int OriValue = connector_->getStatus()->getSteeringOrient();
                    connector_->da(OriValue,OriValue);
                }
            }
        }else{
            std::cout << "Comando no reconocido: " << pair.first.toStdString() << std::endl;
        }
    }
    if(driving) {
        double linearVel  = motor->getRealVel1() /* mm/s */ / 25.4 /* mm/inch */ * 10.0; // unit 1/10 inch/sec
        double angularVel = motor->getRealVel2() * 10.0; // unit 1/10 deg/sec
        double torretVel;
        if (fixed_){
            torretVel = angularVel;
        }else{
            torretVel = motorT->getRealVel1() * 10.0; // unit 1/10 deg/sec
        }
        connector_->vm(linearVel,angularVel,torretVel);
    }
    else{
        connector_->gs();
    }

    
    double mi_x = connector_->getStatus()->getX() / 10  /* inch */ * 25.4 /* mm/inch */ ;
    double mi_y = connector_->getStatus()->getY() / 10  /* inch */ * 25.4 /* mm/inch */ ;
    double steer  = (connector_->getStatus()->getSteeringOrient() / 10);
    double turret = 360.0 - (connector_->getStatus()->getTurretOriente() / 10);
    
    rawOdometry.setCoordinates(mi_x, mi_y,turret);
    exactOdometry.setCoordinates(mi_x, mi_y,turret);
    slamOdometry.setCoordinates(mi_x, mi_y,turret);
    
    robot_sprite->calcAdvance(world->xTransform(mi_x),
                              world->yTransform(mi_y),
                              turret,
                              false);

    robot_sprite->updateSlamOdometry(world->xTransform(mi_x), world->yTransform(mi_y));
    robot_sprite->updateRawOdometry(world->xTransform(mi_x), world->yTransform(mi_y));

    NomadSprite * nsprite = dynamic_cast<NomadSprite *>(robot_sprite);
    nsprite->updatePathLine(steer / NDMath::RAD2GRAD);

    emit setVelocity(connector_->getStatus()->getTransVel() / 10.0 /* inch/sec */ * 25.4 /* mm/inch */
                     ,connector_->getStatus()->getTurretVel() / 10.0 /* deg/sec */
                     ,connector_->getStatus()->getSteeringVel() / 10.0 /* deg/sec */);

    emit setPower(connector_->getStatus()->voltMotorGet()
                  ,connector_->getStatus()->voltCpuGet());
}

void NomadRobot::setMainMotor()
{
    if(!mainMotor) {
        SynchronousMotor* motor = new SynchronousMotor("MainMotor", this);
        mainMotor = motor;
        motor->setMaxRotVel(max_rot_);
        addMotor("MainMotor", mainMotor);
    }
    else
        cerr << "Warning: Robot::setMainMotor() MainMotor already assigned" << endl;
    
    if (motors.value("Turret",0)==0){
        TurretMotor * motor = new TurretMotor("TurretMotor",this);
        motor->setMaxRotVel(max_rot_);
        addMotor("Turret",motor);
    }
    else
        cerr << "Warning: Robot::setMainMotor() TurretMotor already assigned" << endl;
}

void NomadRobot::getKeyEvent(QKeyEvent* event, bool key_on)
{
    Robot::getKeyEvent(event, key_on);

    if(key_on && (event->key() == Qt::Key_Home)){
        connector_->dp(0,0);
        connector_->zr();
    }

    if(key_on && (event->key() == Qt::Key_V)){
        //Talk();
        MostrarVentana();
    }

    if(key_on && (event->key() == Qt::Key_R)){
        if(!switchToReal()){
//            QMessageBox::critical(nullptr, Robot::tr("Switch To Real"),
//                                  Robot::tr("Error to connect to real Robot"), Robot::tr("&Ok"));
            QMessageBox::critical(nullptr, Robot::tr("Switch To Real"),
                                  Robot::tr("Error to connect to real Robot"), QMessageBox::Ok);
        }
    }

    if(key_on && (event->key() == Qt::Key_S)){
        if(!switchtoSim()){
//            QMessageBox::critical(nullptr, Robot::tr("Switch To Simulated"),
//                                  Robot::tr("Error to connect to simulated Robot"), Robot::tr("&Ok"));
            QMessageBox::critical(nullptr, Robot::tr("Switch To Simulated"),
                                  Robot::tr("Error to connect to simulated Robot"), QMessageBox::Ok);
        }
    }

}

void NomadRobot::getMouseEvent(QMouseEvent* e)
{
    Robot::getMouseEvent(e);
}


bool NomadRobot::switchToReal(){
    if(connector_->real_robot()){
        emit setReal(true);
        return true;
    }
    else{
        emit setReal(false);
        return false;
    }
}

bool NomadRobot::switchtoSim(){
    if(connector_->simulated_robot()){
        emit setReal(false);
        return true;
    }
    else{
        emit setReal(true);
        return false;
    }
}

bool NomadRobot::Talk(){
    bool ok;
    QString text = QInputDialog::getText(0, tr("Nomad voice synthesizer"),
                                         tr("that you want to speak?"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty()){
        char msg[500];
        strncpy(msg,text.toStdString().c_str(),500);
        msg[499] = 0;
        connector_->tk(msg);
    }
    return true;
}


void NomadRobot::MostrarVentana(){
    Nomad::Window * window;
    Settings* prog_settings = Settings::instance();
    window = new Nomad::Window(prog_settings->getWorkspace());
    QMdiSubWindow* subw = prog_settings->getWorkspace()->addSubWindow(window);
    window->setMdiSubWindow(subw);

    QObject::connect(this, SIGNAL(setVelocity(double, double, double)),
                     window, SLOT(setVelocity(double, double, double)));
    QObject::connect(this, SIGNAL(setPower(double, double)),
                     window, SLOT(setPower(double, double)));
    QObject::connect(this, SIGNAL(close()),
                     window, SLOT(close()));
    QObject::connect(this, SIGNAL(setReal(bool)),
                     window, SLOT(setReal(bool)));
    QObject::connect(this, SIGNAL(setFixed(bool)),
                     window->cBFixed, SLOT(setChecked(bool)));
    QObject::connect(window, SIGNAL(command(QString, int)),
                     this, SLOT(command(QString, int)));
    
    window->show ();
}

void NomadRobot::command(QString cmd, int value){
    std::cout << cmd.toStdString()  << " value=" << value << std::endl;
    
    if(cmd == "TALK"){
        Talk();
    }
    else if(cmd == "TO_REAL"){
        if(!switchToReal()){
//            QMessageBox::critical(nullptr, Robot::tr("Switch To Real"),
//                                  Robot::tr("Error to connect to real Robot"), Robot::tr("&Ok"));
            QMessageBox::critical(nullptr, Robot::tr("Switch To Real"),
                                  Robot::tr("Error to connect to real Robot"), QMessageBox::Ok);
        }
    }
    else if(cmd == "TO_SIM"){
        if(!switchtoSim()){
//            QMessageBox::critical(nullptr, Robot::tr("Switch To Simulated"),
//                                  Robot::tr("Error to connect to simulated Robot"), Robot::tr("&Ok"));
            QMessageBox::critical(nullptr, Robot::tr("Switch To Simulated"),
                                  Robot::tr("Error to connect to simulated Robot"), QMessageBox::Ok);
        }
    }
    else if(cmd == "STOP"){
        //enviamos un evento para generar la parada en el DriveNomadControl
        QKeyEvent key(QEvent::KeyPress,Qt::Key_Space,Qt::NoModifier);
        emit sendKeyEvent(&key,true);
    }
    else {
        QPair<QString,int> pair;
        pair.first = cmd;
        pair.second = value;

        comandos_.enqueue(pair);
    }
}
