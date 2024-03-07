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

#ifndef RECMONDEVICE_H
#define RECMONDEVICE_H

#include <QObject>
#include <QPointer>
#include <QDomDocument>
#include <QMap>
#include <QList>

class RecMonConnection;

/**
@author jose manuel
*/

class RecMonDevice: public QObject
{
    Q_OBJECT

public:
    RecMonDevice(unsigned long n_id, bool recmon = false,
                 QObject *parent = 0, const char *name = 0);
    ~RecMonDevice();

    virtual const QString devName(bool isCap);
    virtual int rtti_dev(void) const = 0;
    virtual int connectionOffset(void)const {return 1 ;}
    virtual void emitToMonitor(void) = 0;
    virtual void emitToRecorder(void) = 0;
    virtual QList<RecMonConnection *> getRecMonConnections(void) = 0;
    virtual bool edit(bool mode = true, bool adding = false) = 0;
    virtual void write(QDomDocument& , QDomElement&) = 0;
    virtual void write_constructor(QDomElement&) = 0;
    virtual bool isRecMonable(void) { return is_recmonable ; }
    virtual void setRecMonable(bool recmon) { is_recmonable = recmon; }
    typedef QMap<QString, int> SignalMap;
    virtual const SignalMap& signalNames(void) {return signal_map ; }
    virtual QString signalNumName(int num);
    virtual uint getNumId(void) { return num_id ; }
    virtual void setNumId(uint n_id);

protected:
    virtual void write_subobjects(QDomDocument& , QDomElement&) = 0;

signals:
    void emitMonVal(double, int);
    void emitRecVal(double, int);
    //    void isoChanged(uint);
    void num_idChanged(ulong num_id);

protected:
    unsigned long num_id;
    bool is_recmonable;
    SignalMap signal_map;
    const int signalInput;
    const int signalOutput;

};

#endif
