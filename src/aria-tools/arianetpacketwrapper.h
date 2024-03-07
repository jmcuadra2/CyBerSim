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
#ifndef ARIANETPACKETWRAPPER_H
#define ARIANETPACKETWRAPPER_H

class ArNetPacket;
class ArClientBase;

/**
@author jose manuel
*/
class AriaNetPacketWrapper{

  public:
    AriaNetPacketWrapper();
    virtual ~AriaNetPacketWrapper();

    virtual bool readPacket(ArNetPacket* packet) = 0;
    virtual const char* packetName(void) { return packet_name ; }
    virtual const char* deviceName(void) { return device_name ; }
    virtual void setClient(ArClientBase* client) { this->client = client ; }
    
    bool isWaiting() const;
    void setWaiting(bool value);

protected:
    char const* packet_name;
    char const* device_name;
    enum  packetHeaderLenght {RangeHeaderLenght=64};
    ArClientBase* client;
    bool waiting;

};

#endif
