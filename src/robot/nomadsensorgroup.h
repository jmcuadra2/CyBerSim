//
// C++ Interface: nomadsensorgroup
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADSENSORGROUP_H
#define NOMADSENSORGROUP_H

#include "sensorgroup.h"


/**
    @author Javier Garcia Misis
*/
class NomadSensorGroup : public SensorGroup
{
public:
    NomadSensorGroup(const QDomElement& e, void * client = 0);
    
    NomadSensorGroup( int sens_type, int sup_type, void * client = 0);

    ~NomadSensorGroup();

    virtual bool createSensors();
    virtual void scaleSensorField(const double& scale);

    void* getClient(void) { return client_; }

protected:
    virtual void write_special(QDomDocument& doc, QDomElement& e);

private:
    void * client_;

};

#endif
