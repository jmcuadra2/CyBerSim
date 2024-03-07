//
// C++ Interface: robotcommands
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ROBOTCOMMANDS_H
#define ROBOTCOMMANDS_H

#include <QString>
#include <Aria.h>
#include <ArNetworking.h>
#include <vector>
using namespace std;

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

class RobotCommands
{
  protected:
		RobotCommands(QString const& name, ArCommands::Commands ident, size_t numParams);
    
  public:  
    static RobotCommands* createCommand(ArCommands::Commands ident);
    static vector<RobotCommands*> const getListCommands(void);
    static void clearListCommands(void);
    
		~RobotCommands();
    
    ArNetPacket* toPacket(void);
    QString const& getName(void)  { return name ; };
    bool setValues(vector<int> params);
    vector<int> getValues(void) const {return params ; };
    size_t getNumParams() const { return numParams; }
  
    
  protected:
    QString name;
    ArCommands::Commands ident;
    size_t numParams;
    vector<int> params;
    ArNetPacket packet;
    static vector<RobotCommands*> listCommands;   

};

#endif
