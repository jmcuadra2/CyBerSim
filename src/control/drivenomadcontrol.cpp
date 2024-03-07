//
// C++ Implementation: drivenomadcontrol
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "drivenomadcontrol.h"
#include "../robot/synchronousmotor.h"

DriveNomadControl::DriveNomadControl(): DriveControl()
{
    motorPlanT_ = new AbstractMotorPlan();

    defaultCommand.setPriority(0.5);
    defaultCommand.setCommandId(MotorCommand::PolarVel);
    NDPose* commandValue = new NDPose2D();
    defaultCommand.setCommandValue(commandValue, false);
    motorPlan->addCommand(defaultCommand);

    defaultCommandT_.setPriority(0.5);
    defaultCommandT_.setCommandId(MotorCommand::PolarVel);
    NDPose* commandValueT = new NDPose2D();
    defaultCommandT_.setCommandValue(commandValueT, false);
    motorPlanT_->addCommand(defaultCommandT_);
}


DriveNomadControl::~DriveNomadControl()
{
}


MotorCommand DriveNomadControl::writeCommand(MotorCommand::MotorCommands type, double priority, double commandParam1, double commandParam2, double commandParam3)
{
    return DriveControl::writeCommand(type, priority, commandParam1, commandParam2, commandParam3);
}

void DriveNomadControl::connectDrive(bool on)
{
    DriveControl::connectDrive(on);
}

void DriveNomadControl::setMotor(AbstractMotor* motor)
{
    DriveControl::setMotor(motor);
}

void DriveNomadControl::setMotorT(AbstractMotor* motor)
{
    this->motorT_ = motor;
}

void DriveNomadControl::setDefaultCommandT(NDPose* const commandValues)
{
    defaultCommandT_.setCommandValue(commandValues);
}


void DriveNomadControl::getKeyEvent(QKeyEvent* event, bool key_on)
{
    //DriveControl::getKeyEvent(event, key_on);
        
    if(!active) {
        return;
    }   

    double param_lin = 0.1;
    double param_ang = 0.1;
    double param_tur = 0.1;
  
//   if(!messages.getActive()) return;
    SynchronousMotor * motor = dynamic_cast<SynchronousMotor *>(this->motor);

    if(motor == 0 || motorT_ == 0){
        //Con esta llamada a la clase padre enviamos las pulsacioens de teclas a la clase DriverControl.
        return;
    }
    double linearVel  = motor->getVel1();
    double angularVel = motor->getVel2();
    double torretVel = motorT_->getVel1();
    int key_event = event->key();
  
    if(key_on) {
        switch(key_event) {
            case Qt::Key_Up :
                linearVel += param_lin;
                break;
            case Qt::Key_Down :
                linearVel -= param_lin;
                break;
            case Qt::Key_Right :
                angularVel -= param_ang;
                break;
            case Qt::Key_Left :
                angularVel += param_ang;
                break;
            case Qt::Key_PageUp :
            case Qt::Key_A :
                torretVel -= param_tur;
;
                break;
            case Qt::Key_PageDown :
            case Qt::Key_Z :
               torretVel += param_tur;
                break;
            case Qt::Key_Space :
                //stop
                linearVel=0;
                angularVel=0;
                torretVel=0;
                break;
            default:
                ;
        }
    }else{
        switch(key_event) { 
            case Qt::Key_Up :
            case Qt::Key_Down :
                if(!event->isAutoRepeat()) { 
                    linearVel = 0.0;
                }
                break;
            case Qt::Key_Right :
            case Qt::Key_Left :
                if(!event->isAutoRepeat()) { 
                    angularVel = 0.0;
                }
                break;
            case Qt::Key_PageUp :
            case Qt::Key_PageDown :
            case Qt::Key_A :
            case Qt::Key_Z :
                if(!event->isAutoRepeat()) { 
                    torretVel = 0.0;
                }
                break;
            case Qt::Key_Space :
                //stop
                linearVel=0.0;
                angularVel=0.0;
                torretVel=0.0;
                break;
            default:
                ;
        }
    }

    double maxLinear = 1.00;
    double maxAngular = 1.00;
    double maxTorret = 1.00;


    linearVel = linearVel <= maxLinear ? linearVel : maxLinear;
    linearVel = linearVel > -maxLinear ? linearVel : -maxLinear;

    angularVel = angularVel <= maxAngular ? angularVel : maxAngular;
    angularVel = angularVel > -maxAngular ? angularVel : -maxAngular;

    torretVel = torretVel <= maxTorret ? torretVel : maxTorret;
    torretVel = torretVel > -maxTorret ? torretVel : -maxTorret;

    MotorCommand command;
    command.setPriority(0.3);
    command.setCommandId(MotorCommand::PolarVel);
    NDPose* commandValue = new NDPose2D();
    
    commandValue->setCoordinates(linearVel, angularVel, 0);
    command.setCommandValue(commandValue, false);
    motorPlan->addCommand(command);    

    MotorCommand commandT;
    commandT.setPriority(0.3);
    commandT.setCommandId(MotorCommand::PolarVel);
    NDPose* commandValueT = new NDPose2D();
    
    commandValueT->setCoordinates(torretVel, 0, 0);
    commandT.setCommandValue(commandValueT, false);
    motorPlanT_->addCommand(commandT);

/*
    motor->setLinearVel(vel);
    motor->setDeltaOrient(dir);
    motorT_->setVel1(velang);
*/
}

