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
#ifndef ICODEVICE_H
#define ICODEVICE_H

#include "isodevice.h"

/**
@author jose manuel
*/
class ICODevice : public ISODevice
{

    Q_OBJECT
    friend class EditISODeviceTab;

public:
    ICODevice(ulong n_i, int sub_lay, double th, int actF, double initext,
              double l_r, bool recmon = false,
              QObject *parent = 0, const char *name = 0);
    ICODevice(ulong n_i, int lay, bool recmon = false,
              QObject *parent = 0, const char *name = 0);
    ICODevice(ulong n_i, bool recmon = false,
              QObject *parent = 0, const char *name = 0);
    ~ICODevice();

    int rtti_dev(void) const;
    void initOldOut(void);
    
public slots:
    void emitToMonitor(void);
    void emitToRecorder(void);
    
protected:
    double calculateDerivative(double const& old_value);
    void defaultValues(bool to_base = true);

protected:
    double old_USVal;

};

#endif
