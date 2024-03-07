//
// C++ Implementation: robotcommandsdialog
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "robotcommandsdialog.h"
#include "../robotcommands.h"
#include "../ndariarobot.h"
// #include <iostream>

RobotCommandsDialog::RobotCommandsDialog(QWidget* parent, const char* name)
    : QewSimpleDialog(parent, name)
{
  selectedCommand = 0;
  idx = -1;
  setupUi(this);
  listCommands = RobotCommands::getListCommands();
  for(unsigned int i = 0; i < listCommands.size(); i++)
    listWidget->addItem(listCommands[i]->getName());
    
  connect(listWidget, SIGNAL(itemClicked(QListWidgetItem* )), this, SLOT(setCommand(QListWidgetItem* )));
  connect(spinBox_1, SIGNAL(valueChanged(int)), this, SLOT(setParams1(int)));
  connect(spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(setParams2(int)));
  connect(sendButton, SIGNAL(clicked()), this, SLOT(send(void)));    
  
}


RobotCommandsDialog::~RobotCommandsDialog()
{
  RobotCommands::clearListCommands();  
}

void RobotCommandsDialog::init(NDAriaRobot* robot)
{
  this->robot = robot;
}

void RobotCommandsDialog::setCommand(QListWidgetItem * item)
{
  if(idx != listWidget->row(item)) {
    idx = listWidget->row(item);
    selectedCommand = listCommands[idx];
    spinBox_1->setValue(0);
    spinBox_2->setValue(0);
    spinBox_1->setEnabled(false);
    spinBox_2->setEnabled(false);
    int num_params = selectedCommand->getNumParams();
    currentParams.resize(num_params);
    currentParams = selectedCommand->getValues();
    if(num_params > 0) {
      spinBox_1->setEnabled(true);
      spinBox_1->setValue(currentParams[0]);
    }
    if(num_params > 1) {
      spinBox_2->setEnabled(true);
      spinBox_2->setValue(currentParams[1]);
    }
  }
  
//   std::cout << idx << endl;
}

void RobotCommandsDialog::setParams1(int val)
{
  if(currentParams[0] != val) {
    currentParams[0] = val;
    spinBox_1->setValue(val);
    selectedCommand->setValues(currentParams);
  }
}

void RobotCommandsDialog::setParams2(int val)
{
  if(currentParams[1] != val) {
    currentParams[1] = val;
    spinBox_2->setValue(val);
    selectedCommand->setValues(currentParams);
  }
}

void RobotCommandsDialog::send(void)
{
  if(selectedCommand) {
    robot->getAriaClient()->requestOnce("MicroControllerCommand", selectedCommand->toPacket());
  }
}
