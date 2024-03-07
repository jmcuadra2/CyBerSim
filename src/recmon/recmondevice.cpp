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
#include "recmondevice.h"
#include "recmonconnection.h"

RecMonDevice::RecMonDevice(unsigned long n_id, bool recmon,
                           QObject *parent, const char *name)
    : QObject(parent), signalInput(-1),
      signalOutput(-2)
{

    num_id = n_id;
    is_recmonable = recmon;
    signal_map[tr("Input")] = signalInput;
    signal_map[tr("Output")] = signalOutput;

}


RecMonDevice::~RecMonDevice()
{
}

void RecMonDevice::setNumId(uint n_id)
{

    num_id = n_id;

}

const QString RecMonDevice::devName(bool isCap)
{

    return isCap ? tr("Device") : tr("device");

}

QString RecMonDevice::signalNumName(int num)
{

    SignalMap::Iterator it;
    QString signal_name = "";
    for (it = signal_map.begin(); it != signal_map.end(); ++it) {
        if(it.value() == num) {
            signal_name = it.key();
            break;
        }
    }
    return signal_name;

}
