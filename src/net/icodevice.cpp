/***************************************************************************
 *   Copyright (C) 2004 by jose manuel                                     *
 *   jose@pcjose                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "icodevice.h"

ICODevice::ICODevice(ulong n_i, int sub_lay, double th, int actF, double initext,
                     double l_r, bool recmon, QObject *parent , const char *name) :
                ISODevice(n_i, sub_lay, th, actF, initext, l_r, recmon, parent, name)
{
}

ICODevice::ICODevice(ulong n_i, int lay, bool recmon, QObject *parent , const char *name) :
                ISODevice(n_i, lay, recmon, parent, name )
{
}

ICODevice::ICODevice(ulong n_i, bool recmon, QObject *parent , const char *name) :
                ISODevice(n_i, recmon, parent , name)
{
}

ICODevice::~ICODevice()
{
}

void ICODevice::defaultValues(bool to_base)
{    
    if(to_base)
        ISODevice::defaultValues();
    old_USVal = 0.0;
}  

int ICODevice::rtti_dev(void) const
{   
    return (Neuron::ICO_DEVICE);
}

double ICODevice::calculateDerivative(double const& /*old_value*/)
{
    derivative = USVal - old_USVal;
    old_USVal = USVal;
    return derivative;
}

void ICODevice::initOldOut(void)
{   
    ISODevice::initOldOut();
    old_USVal = 0.0;
    derivative = 0.0;
}

void ICODevice::emitToMonitor(void)
{    
    emit emitMonVal(input_sum, signalInput);
    emit emitMonVal(Output, signalOutput);
    emit emitMonVal(derivative, signalOutDiff);
}

void ICODevice::emitToRecorder(void)
{    
    emit emitRecVal(input_sum, signalInput);
    emit emitRecVal(Output, signalOutput);
    emit emitRecVal(derivative, signalOutDiff);
}
