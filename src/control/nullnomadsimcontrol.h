//
// C++ Interface: nullnomadsimcontrol
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NULLNOMADSIMCONTROL_H
#define NULLNOMADSIMCONTROL_H

#include "nullsimcontrol.h"

class QewExtensibleDialog;
class SMGNomadInterface;

/**
    @author Javier Garcia Misis
*/

class NullNomadSimControl : public NullSimControl
{
public:
    NullNomadSimControl();

    ~NullNomadSimControl();

    virtual bool createSMGInterface();
    virtual int getSuperType();
    virtual int getType();
    virtual QList< QewExtensibleDialog* > tabsForEdit(QewExtensibleDialog* arg1);
    virtual void backPropagate();
    virtual void connectIO();
    virtual void doMonitor();
    virtual void initElements();
    virtual void sendOutputs();
    virtual void setParams(const QDomElement& e);

protected:
    virtual void propagate();
    virtual void readInputs();

private:
    SMGNomadInterface * smgNomadI_;
    MotorCommand defaultCommandT_;
};

#endif
