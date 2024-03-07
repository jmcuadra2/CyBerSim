//
// C++ Interface: robotcommandsdialog
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ROBOTCOMMANDSDIALOG_H
#define ROBOTCOMMANDSDIALOG_H

#include "ui_robotcommandsdialog.h"
#include <vector>

using namespace std;

class RobotCommands;
class NDAriaRobot;

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class RobotCommandsDialog : public QewSimpleDialog, private Ui::RobotCommandsDialog
{
  Q_OBJECT
  
	public:
		RobotCommandsDialog(QWidget* parent = 0, const char* name = 0);  

		~RobotCommandsDialog();
    
    virtual void init(NDAriaRobot* robot);
    
  protected slots:
    void setCommand(QListWidgetItem * item);
    void setParams1(int val);
    void setParams2(int val);
    void send(void);

  protected:
    RobotCommands* selectedCommand;
    vector< RobotCommands* > listCommands;
    NDAriaRobot* robot;
    vector<int> currentParams;
    int idx;
};

#endif
