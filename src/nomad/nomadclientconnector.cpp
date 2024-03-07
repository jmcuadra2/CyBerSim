//
// C++ Implementation: nomadclientconnector
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "../nomad/nomadclientconnector.h"

#define SMASK_POS_DATA             0

namespace Nomad{

Connector::Connector(long robot_id)
    :robot_id_(robot_id)
{
}


Connector::~Connector()
{
}

void Connector::init_mask(void)
{
    status_.init_mask();
}

/*
 * init_sensors - initialize the sensor mask, Smask, and send it to the
 *                robot. It has no effect on the sensors
 */
bool Connector::init_sensors ( void )
{
    init_mask();
    return ct();
}



}
