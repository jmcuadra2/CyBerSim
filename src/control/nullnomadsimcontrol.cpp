//
// C++ Implementation: nullnomadsimcontrol
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "nullnomadsimcontrol.h"
#include "smgnomadinterface.h"
#include "drivenomadcontrol.h"

NullNomadSimControl::NullNomadSimControl()
{
    defaultCommand.setPriority(1);
    defaultCommand.setCommandId(MotorCommand::PolarVel);
    NDPose* commandValue = new NDPose2D();
    defaultCommand.setCommandValue(commandValue, false);
    motorPlan->addCommand(defaultCommand);

    defaultCommandT_.setPriority(1);
    defaultCommandT_.setCommandId(MotorCommand::PolarVel);
    NDPose* commandValueT = new NDPose2D();
    defaultCommandT_.setCommandValue(commandValueT, false);

    smgNomadI_ = 0;
}

NullNomadSimControl::~NullNomadSimControl()
{
}

void NullNomadSimControl::initElements(void)
{
    SMGInterface* smgInterface = getSMGInterface();
    if(!smgInterface) {
        cerr << "Warning: NullNomadSimControl::initElements() null SMGInterface" << endl;
        return;
    }

    if(!smgInterface->isInitialized()) {
        cerr << "Warning: NullNomadSimControl::initElements() SMGInterface not initialized" << endl;
        return;
    }

    double vel1, vel2;
    smgInterface->getOutputMotorPlan()->getInitialCommand(vel1, vel2);  // provisional

    NDPose* commandDefault = defaultCommand.getCommandValue();
    commandDefault->setCoordinates(vel1, vel2, 0);
    defaultCommand.setCommandValue(commandDefault);

    smgNomadI_->getOutputMotorPlanT()->getInitialCommand(vel1, vel2);  // provisional
    NDPose* commandDefaultT = defaultCommandT_.getCommandValue();
    commandDefaultT->setCoordinates(vel1, vel2, 0);
    defaultCommandT_.setCommandValue(commandDefaultT);

    DriveNomadControl* drive = dynamic_cast<DriveNomadControl *>(getSMGInterface()->getDrive());

    if(drive) {
        if(drive->isActive()){
            drive->setDefaultCommand(commandDefault);
            drive->setDefaultCommandT(commandDefaultT);
        }
    }
}



void NullNomadSimControl::readInputs(void)
{
    sampleInputs();
}

void NullNomadSimControl::propagate(void)
{    
}

void NullNomadSimControl::backPropagate(void)
{ 
}

int NullNomadSimControl::getType(void)
{
    return NULL_NOMAD_CONTROL;
}

int NullNomadSimControl::getSuperType(void)
{  
    return CODE_CONTROL ;
}

void NullNomadSimControl::doMonitor(void)
{    
}

void NullNomadSimControl::connectIO(void)
{   
}

QList< QewExtensibleDialog* > NullNomadSimControl::tabsForEdit(QewExtensibleDialog* )
{
    QList<QewExtensibleDialog*> add_dialogs;
    return add_dialogs;
}

void NullNomadSimControl::setParams(const QDomElement& )
{
}   

bool NullNomadSimControl::createSMGInterface(void)
{
    bool ret = false;
    if(!smgNomadI_) {
        smgNomadI_ = new SMGNomadInterface();
        setSMGInterface(smgNomadI_);
        ret = true;
    }
    else
        cerr << "Warning: NullNomadSimControl::creategetSMGInterface()() getSMGInterface() already assigned" << endl;

    return ret;
}

void NullNomadSimControl::sendOutputs(void)
{

    if(smgNomadI_ == 0) {
        cerr << "Warning: NullNomadSimControl::sendOutputs() null SMGInterface" << endl;
        return;
    }

    if(!smgNomadI_->isInitialized()) {
        cerr << "Warning: NullNomadSimControl::sendOutputs() SMGInterface not initialized" << endl;
        return;
    }

    DriveNomadControl* drive = dynamic_cast<DriveNomadControl *>(getSMGInterface()->getDrive());
    //Motor 1
    OutputMotorPlan* outputMotorPlan = smgNomadI_->getOutputMotorPlan();
    if(outputMotorPlan==0) {
        cerr << "Warning: NullNomadSimControl::sendOutputs(), no motor plan" << endl;
        return;
    }

    OutputMotorPlan* outputMotorPlanT = smgNomadI_->getOutputMotorPlanT();
    if(outputMotorPlanT==0) {
        cerr << "Warning: NullNomadSimControl::sendOutputs(), no motor plan T" << endl;
        return;
    }

    if(drive->isActive()) {
        outputMotorPlan->addCommand(drive->getMotorPlan()->getCommands()[0]);
        outputMotorPlanT->addCommand(drive->getMotorPlanT()->getCommands()[0]);
    }
    else{
        outputMotorPlan->addCommand(defaultCommand);
        outputMotorPlanT->addCommand(defaultCommandT_);
    }

    outputMotorPlan->sendCommand();
    outputMotorPlanT->sendCommand();

}
