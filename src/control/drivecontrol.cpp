//
// C++ Implementation: drivecontrol
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "drivecontrol.h"
#include "../neuraldis/ndmath.h"

DriveControl::DriveControl(): /*QObject() ,*/ MotorControl()
{
//   messages.setConfidence(0);
//   messages.setActive(false);
  controlName = "DriveControl";

//   defaultCommand.setPriority(0.5);
  defaultCommand.setPriority(0.5);
  defaultCommand.setCommandId(MotorCommand::WheelsVel);
  NDPose* commandValue = new NDPose2D();
  defaultCommand.setCommandValue(commandValue, false);
  motorPlan->addCommand(defaultCommand);
   
  active = false;

  emit sendToRecorder(0, 0);
}


DriveControl::~DriveControl()
{
}

int DriveControl::getType(void)
{
  return DRIVE_CONTROL;
}

void DriveControl::sendOutputs(void)
{
}

void DriveControl::setMotor(AbstractMotor* motor) 
{ 
  MotorControl::setMotor(motor);
}

void DriveControl::connectDrive(bool /*on*/)
{
//   messages.setActive(on);
//   if(on)
//     messages.setConfidence(0.25);
//   else
//     messages.setConfidence(0);
}

// void DriveControl::getKeyEvent(int key_event, bool key_on) 
// {
//   double param_head = 0.05;
//   double param_vel = 0.1;
//   double leftVel = motor->getVel1();
//   double rightVel = motor->getVel2(); 
// 
//   if(key_on) {
//     switch(key_event) {
//       case Qt::Key_Up :
//         leftVel += param_vel;
//         rightVel += param_vel;
//         break;
//       case Qt::Key_Down :
//         leftVel -= param_vel;
//         rightVel -= param_vel;
//         break;
//       case Qt::Key_Right :
//         leftVel += param_head;
//         rightVel -= param_head;
//         break;
//       case Qt::Key_Left :
//         leftVel -= param_head;
//         rightVel += param_head;
//         break;
//       default:
//         int i  = 0;
//         break;      
//     }
//   }
//   else {
//     switch(key_event) { 
//       case Qt::Key_Up :
//       case Qt::Key_Down : 
//         leftVel = 0.0;
//         rightVel = 0.0;      
//         break;       
//       case Qt::Key_Right : 
//       case Qt::Key_Left :       
//         leftVel = (leftVel + rightVel)/2;
//         rightVel = leftVel;                    
//         break;             
//       default:
//         int i  = 0;      
//         break;
//     }
//   }
//   messages.setX(leftVel);
//   messages.setY(rightVel);  
//      
// }

void DriveControl::getKeyEvent(QKeyEvent* event, bool key_on) 
{
  double param_head = 0.05;
  double param_vel = 0.1;
  
//   if(!messages.getActive()) return;
  
  double leftVel = motor->getVel1();
  double rightVel = motor->getVel2();
  int key_event = event->key();
  
  if(key_on) {
    switch(key_event) {
      case Qt::Key_Home :  // piloto automático
        if(!event->isAutoRepeat()) {
          motorPlan->getCommands()[0] = defaultCommand; 
        }
        return;
        break;
      case Qt::Key_Up :
        leftVel += param_vel;
        rightVel += param_vel;
        break;
      case Qt::Key_Down :
        leftVel -= param_vel;
        rightVel -= param_vel;
        break;
      case Qt::Key_Right :
        if(!event->isAutoRepeat()) {
          leftVel += param_head;
          rightVel -= param_head;
        }
        break;
      case Qt::Key_Left :
        if(!event->isAutoRepeat()) {
          leftVel -= param_head;
          rightVel += param_head;
        }
        break;
      default:
//         int i  = 0;
        break;      
    }
  }
  else {
    switch(key_event) { 
      case Qt::Key_Up :
      case Qt::Key_Down :
        if(!event->isAutoRepeat()) { 
         leftVel = 0.0;
         rightVel = 0.0;
        }
        break;       
      case Qt::Key_Right : 
      case Qt::Key_Left : 
        if(!event->isAutoRepeat()) {      
          leftVel = (leftVel + rightVel)/2;
          rightVel = leftVel;
        }
        break;             
      default:
        int i  = 0;      
        break;
    }
  }
//   double max = 1.0;
//   max = fabs(leftVel) > max ? fabs(leftVel) : max;
//   max = fabs(rightVel) > max ? fabs(rightVel) : max;
//   leftVel /= max;
//   rightVel /= max;

  double maxLeft = fabs(defaultCommand.getCommandValue()->getPosition().at(0));
  double maxRight = fabs(defaultCommand.getCommandValue()->getPosition().at(1));
  maxLeft = maxLeft > 1 ? 1 : maxLeft;
  maxRight = maxRight > 1 ? 1 : maxRight;
  maxLeft = maxLeft == 0 ? 1 : maxLeft;
  maxRight = maxRight == 0 ? 1 : maxRight;

  leftVel = leftVel <= maxLeft ? leftVel : maxLeft;
  leftVel = leftVel > -maxLeft ? leftVel : -maxLeft;
  rightVel = rightVel <= maxRight ? rightVel : maxRight;
  rightVel = rightVel > -maxRight ? rightVel : -maxRight;

//   MotorCommand& command = motorPlan->getCommands().at(0);
//   MotorCommand& command = motorPlan->getCommands()[0];
//   command.getCommandValue()->setCoordinates(leftVel, rightVel, 0);

  MotorCommand command = writeCommand(MotorCommand::WheelsVel, .3, leftVel, rightVel, 0);
  motorPlan->addCommand(command);
//   messages.setX(leftVel);
//   messages.setY(rightVel);
//   messages.setConfidence(0.25);  
     
}

MotorCommand DriveControl::writeCommand(MotorCommand::MotorCommands type, double priority, double commandParam1, double commandParam2, double commandParam3)
{
  MotorCommand command;
  command.setPriority(priority);
  command.setCommandId(MotorCommand::WheelsVel);
  NDPose* commandValue = new NDPose2D();

  commandValue->setCoordinates(commandParam1, commandParam2, commandParam3);
  command.setCommandValue(commandValue, false);
  return command;
}

void DriveControl::activate(bool on)
{
  if(active != on) {
    active = on;
    if(!active) {
      MotorCommand command = writeCommand(MotorCommand::WheelsVel, .3, 0, 0, 0);
      motorPlan->addCommand(command);
    }
    
  }
}

// void DriveControl::setDefaultCommand(MotorCommand const& motorCommand)
// {
//   defaultCommand = motorCommand;
// }

// void DriveControl::getKeyEvent(int key_event, bool key_on) 
// {
//   double leftVel;
//   double rightVel;
// 
//   if(key_on) {
//     switch(key_event) {
//       case Qt::Key_Up :
//         vel_increment = 1;
//         break;
//       case Qt::Key_Down :
//         vel_increment = -1;
//         break;
//       case Qt::Key_Right :
//         head_increment = 1;
//         break;
//       case Qt::Key_Left :
//         head_increment = -1;
//         break;
//       default:
//         break;      
//     }
//   }
//   else {
//     switch(key_event) {    
//       case Qt::Key_Up :
//         vel_increment = 0;
//         break;
//       case Qt::Key_Down :
//         vel_increment = 0;
//         break;
//       case Qt::Key_Right : 
//         leftVel = messages.getX();
//         rightVel = messages.getY();             
//         leftVel = (leftVel + rightVel)/2;
//         rightVel = leftVel;          
//         head_increment = 0;
//         messages.setX(leftVel);
//         messages.setY(rightVel);         
//         break;
//       case Qt::Key_Left :
//         leftVel = messages.getX();
//         rightVel = messages.getY();        
//         leftVel = (leftVel + rightVel)/2;
//         rightVel = leftVel;             
//         head_increment = 0;
//         messages.setX(leftVel);
//         messages.setY(rightVel);        
//         break;
//       default:
//         break;
//     }
//   }   
// }


// void DriveControl::getKeyEvent(int key_event, bool key_on) 
// {
//   double param_head = 0.05;
//   double param_vel = 0.1;
//   double leftVel = motor->getVel1();
//   double rightVel = motor->getVel2(); 
// 
//   if(key_on) {
//     switch(key_event) {
//       case Qt::Key_Up :
//         leftVel += param_vel;
//         rightVel += param_vel;
//         break;
//       case Qt::Key_Down :
//         leftVel -= param_vel;
//         rightVel -= param_vel;
//         break;
//       case Qt::Key_Right :
//         leftVel += param_head;
//         rightVel -= param_head;
//         break;
//       case Qt::Key_Left :
//         leftVel -= param_head;
//         rightVel += param_head;
//         break;
//       default:
//         int i  = 0;
//         break;      
//     }
//   }
//   else {
//     switch(key_event) { 
//       case Qt::Key_Up :
//       case Qt::Key_Down : 
//         leftVel = 0.0;
//         rightVel = 0.0;      
//         break;       
//       case Qt::Key_Right : 
//       case Qt::Key_Left :       
//         leftVel = (leftVel + rightVel)/2;
//         rightVel = leftVel;                    
//         break;             
//       default:
//         int i  = 0;      
//         break;
//     }
//   }
//   messages.setX(leftVel);
//   messages.setY(rightVel);  
//      
// }

// void DriveControl::drive(void)
// {
//   double param_head = 0.02;
//   double param_vel = 0.1;
//   double leftVel = messages.getX();
//   double rightVel = messages.getY();
//   
//   if(head_increment > 0) { // izquierda
//     if(vel_increment >= 0) {
//       leftVel += param_head;
//       rightVel -= param_head;
//     } 
//     else if(vel_increment < 0) {
//       leftVel -= param_head;
//       rightVel += param_head;    
//     }
//     else {
//       leftVel += param_head;
//       rightVel -= param_head;
//     }      
//   }
//   else if(head_increment < 0){ // derecha
//     if(vel_increment > 0) {
//       rightVel += param_head*rightVel;
//     } 
//     else if(vel_increment < 0) {
//       rightVel -= param_head*rightVel;    
//     }
//     else {
//       leftVel -= param_head;
//       rightVel += param_head;    
//     }   
//   } 
//   else {  
//     if(vel_increment > 0) { // centro
//       leftVel += param_vel;
//       rightVel += param_vel;    
//     } 
//     else if(vel_increment < 0) {
//       leftVel -= param_vel;
//       rightVel -= param_vel;        
//     }
//     else {
//       leftVel = 0.0;
//       rightVel = 0.0;    
//     }  
// 
//   }
//   
//   double max = 1.0;
//   max = leftVel > max ? leftVel : max;
//   max = rightVel > max ? rightVel : max;
//   leftVel /= max;
//   rightVel /= max;
//   messages.setX(leftVel);
//   messages.setY(rightVel);
//   motor->setVel1(leftVel);
//   motor->setVel2(rightVel);
// 
// }
