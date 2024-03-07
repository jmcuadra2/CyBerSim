//
// C++ Implementation: sickinfowrapper
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sickinfowrapper.h"

SickInfoWrapper::SickInfoWrapper(): NDRangeDeviceInfoWrapper()
{
    packet_name = "laserRanges";
    device_name = "laser";
}

SickInfoWrapper::~SickInfoWrapper()
{
}


