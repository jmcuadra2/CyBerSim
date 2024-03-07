//
// C++ Implementation: robotcommands
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "robotcommands.h"
#include <QObject>

#define NUM_COMMANDS 12


vector<RobotCommands*> RobotCommands::listCommands;

RobotCommands* RobotCommands::createCommand(ArCommands::Commands ident)
{
  RobotCommands* command = 0;
  switch(ident) {
    case ArCommands::PULSE :
      command = new RobotCommands(QObject::tr("Pulse"), ident, 0);
      break;
    case ArCommands::SETV  :
      command = new RobotCommands(QObject::tr("Maximum velocity (mm/sec)"), ident, 1);
      break;
    case ArCommands::MOVE :
      command = new RobotCommands(QObject::tr("Translational move (mm)"), ident, 1);
      break;
    case ArCommands::ROTATE :
      command = new RobotCommands(QObject::tr("Rotational velocity (deg/sec)"), ident, 1);
      break;
    case ArCommands::SETRV :
      command = new RobotCommands(QObject::tr("Maximum rotational velocity (deg/sec)"), ident, 1);
      break;
    case ArCommands::VEL :
      command = new RobotCommands(QObject::tr("Translational velocity (mm/sec)"), ident, 1);
      break;
    case ArCommands::HEAD :
      command = new RobotCommands(QObject::tr("Turn to absolute heading 0-359 (degrees)"), ident, 1);
      break;
    case ArCommands::DHEAD :
      command = new RobotCommands(QObject::tr("Turn relative to current heading (degrees) "), ident, 1);
      break;
    case ArCommands::VEL2 :
      command = new RobotCommands(QObject::tr("Independent wheel velocities (right, left)"), ident, 2);
      break;   
    case ArCommands::SONAR :
      command = new RobotCommands(QObject::tr("Enable (1) or disable (0) sonar"), ident, 1);
      break; 
    case ArCommands::STOP :
      command = new RobotCommands(QObject::tr("Stop robot"), ident, 1);
      break;                
    case ArCommands::GYRO :
      command = new RobotCommands(QObject::tr("1 to enable gyro packets, 0 to disable"), ident, 1);
      break;
    default:
      break;                                                 
  }
  return command;
}

vector< RobotCommands* > const RobotCommands::getListCommands(void)
{
  ArCommands::Commands commands[] = {ArCommands::PULSE, ArCommands::SETV, ArCommands::MOVE, ArCommands::ROTATE, ArCommands::SETRV, ArCommands::VEL, ArCommands::HEAD, ArCommands::DHEAD, ArCommands::VEL2, ArCommands::SONAR, ArCommands::STOP, ArCommands::GYRO};
  if(listCommands.empty()) {
    for(int i = 0; i <  NUM_COMMANDS; i++)
      listCommands.push_back(createCommand(commands[i]));
  }
  return listCommands;
}

void RobotCommands::clearListCommands(void)
{
  for(int i = 0; i < NUM_COMMANDS; i++)
    delete listCommands[i];
  listCommands.clear();  
}

RobotCommands::RobotCommands(QString const& name, ArCommands::Commands ident, size_t numParams) : name(name), ident(ident), numParams(numParams) 
{
  params.reserve(numParams);
  params.resize(numParams);
}


RobotCommands::~RobotCommands()
{

}

bool RobotCommands::setValues(vector<int> params)
{
  bool ret = false;
  if(numParams == params.size()) {
    this->params = params;
    ret = true;
  }
  else
    qDebug("RobotCommands invalid number of arguments");
  return ret;
}

ArNetPacket* RobotCommands::toPacket(void)
{
  QString s;
  s = QString::number(ident);
  for(unsigned int i = 0; i < params.size(); i++)
    s += " " + QString::number(params[i]);
  packet.empty();
  packet.strToBuf(s.toLatin1().data());
  return &packet;
//   client->requestOnce("MicroControllerCommand", &packet);
}

