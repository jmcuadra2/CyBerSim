/***************************************************************************
                          nullsimcontrol.cpp  -  description
                             -------------------
    begin                : Sat Apr 30 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "nullsimcontrol.h"
#include "smginterface.h"

NullSimControl::NullSimControl()
{
    defaultCommand.setPriority(1);
    defaultCommand.setCommandId(MotorCommand::WheelsVel);
    NDPose* commandValue = new NDPose2D();
    defaultCommand.setCommandValue(commandValue, false);
    motorPlan->addCommand(defaultCommand);
}

NullSimControl::~NullSimControl()
{
}

void NullSimControl::initElements(void)
{
    SMGInterface* smgInterface = getSMGInterface();
    if(!smgInterface) {
        cerr << "Warning: NullSimControl::initElements() null SMGInterface" << endl;
        return;
    }

    if(!smgInterface->isInitialized()) {
        cerr << "Warning: NullSimControl::initElements() SMGInterface not initialized" << endl;
        return;
    }
    double vel1, vel2;
    smgInterface->getOutputMotorPlan()->getInitialCommand(vel1, vel2);  // provisional
    DriveControl* drive = getSMGInterface()->getDrive();

    NDPose* commandDefault = defaultCommand.getCommandValue();
    commandDefault->setCoordinates(vel1, vel2, 0);
    defaultCommand.setCommandValue(commandDefault);

    if(drive) {
        if(drive->isActive())
            drive->setDefaultCommand(commandDefault); // cuidado matar puntero ?
    }
}

void NullSimControl::readInputs(void)
{
    sampleInputs();
}

void NullSimControl::propagate(void)
{

}

void NullSimControl::backPropagate(void)
{    
}

int NullSimControl::getType(void)
{ 
    return NULL_CONTROL ;
}

int NullSimControl::getSuperType(void)
{ 
    return CODE_CONTROL ;
}

void NullSimControl::doMonitor(void)
{  
}

void NullSimControl::connectIO(void)
{  
}

QList<QewExtensibleDialog*> NullSimControl::tabsForEdit(QewExtensibleDialog* )
{
    QList<QewExtensibleDialog*> add_dialogs;
    return add_dialogs;
}

void NullSimControl::setParams(const QDomElement& )
{
}   

bool NullSimControl::createSMGInterface(void)
{
    bool ret = false;
    if(!getSMGInterface()) {
        setSMGInterface(new SMGInterface());
        ret = true;
    }
    else
        cerr << "Warning: NullSimControl::creategetSMGInterface()() getSMGInterface() already assigned" << endl;
    return ret;

}

void NullSimControl::sendOutputs(void)
{
    DriveControl* drive;

    OutputMotorPlan* outputMotorPlan = getSMGInterface()->getOutputMotorPlan();
    if(outputMotorPlan) {
        drive = getSMGInterface()->getDrive();
        if(drive->isActive()) {
            QList<MotorCommand> driveCommands = drive->getMotorPlan()->getCommands();
            outputMotorPlan->addCommand(driveCommands[0]);
        }
        else
            outputMotorPlan->addCommand(defaultCommand);

        outputMotorPlan->sendCommand();
    }
    else
        cerr << "Warning: NullSimControl::sendOutputs(), no motor plan" << endl;
}
