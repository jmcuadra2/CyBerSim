//
// C++ Implementation: nomadcommand
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "nomadcommand.h"

namespace Nomad{

void Command::setMessage(long * msg,unsigned short size){
    setSize(size);
    for(int i=0;i<size_;i++){
        mesg_[i]    = msg[i];
        mesgNet_[i] = htonl(msg[i]);
    }

}
void Command::setMessageNet(long * msg,unsigned short size){
    setSizeNet(size);
    for(int i=0;i<size_;i++){
        mesg_[i]    = ntohl(msg[i]);
        mesgNet_[i] = msg[i];
    }
}

void Command::netToHost(){
    for(int i=0;i<size_;i++){
        mesg_[i]    = ntohl(mesgNet_[i]);
    }
}
void Command::hostToNet(){
    for(int i=0;i<size_;i++){
        mesgNet_[i] = htonl(mesg_[i]);
    }
}

void Command::setSize(unsigned short size){
    size_=size;
//    if(mesg_<=0){
    if(mesg_==nullptr){
        delete [] mesg_;
        delete [] mesgNet_;
        mesg_ = 0;
        mesgNet_ = 0;
    }
    
    if(size_>0){
        mesg_     = new long[size_];
        mesgNet_ = new long[size_];
    }
}

void Command::setSizeNet(unsigned short size){
    setSize(ntohs(size));
}

}

