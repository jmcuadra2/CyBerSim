//
// C++ Interface: nomadcommand
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef NOMADCOMMAND_H
#define NOMADCOMMAND_H
/**
    @author Javier Garcia Misis
*/

#define USER_BUFFER_LENGTH  0xFFFF
//#define USER_BUFFER_LENGTH  1000
#include <arpa/inet.h>

namespace Nomad{

class Command{
public:
    Command(short type):type_(type),size_(0),mesg_(0),mesgNet_(0){;}
    Command():type_(0),size_(0),mesg_(0),mesgNet_(0){;}
    ~Command(){;}
    void setType(short type){type_=type;}
    short getType()        {return type_;}

    void setTypeNet(short type){type_=ntohs(type);}
    short getTypeNet()        {return htons(type_);}

    void setSize(unsigned short size);
    unsigned short getSize()        {return size_;}

    void setSizeNet(unsigned short size);
    unsigned short getSizeNet()        {return htons(size_);}

    void setMessage(long * msg,unsigned short size);
    long * getMessage(){return mesg_;}
    void setMessageNet(long * msg,unsigned short size);
    long * getMessageNet(){return mesgNet_;}
    void netToHost();
    void hostToNet();

/* generic message types */
//     typedef enum generic_message{
    enum generic_message{
        VOID_MSG=0
    };

/* request message types */
//     typedef enum request_message{
      enum request_message{
        AC=10,
        SP=11,
        VM=12,
        PR=13,
        MV=43,
        PA=14,
        GS=15,
        CT=16,
        ST=17,
        LP=18,
        TK=19,
        ZR=20,
        DP=21,
        CONF_IR=22,
        CONF_SN=23,
        CONF_CP=24,
        CONF_LS=25,
        CONF_TM=26,
        GET_IR=27,
        GET_SN=28,
        GET_RC=29,
        GET_RV=30,
        GET_RA=31,
        GET_CP=32,
        GET_LS=33,
        SETUP_LS=34,
        GET_BP=35,
        CONF_SG=36,
        GET_SG=37,
        GET_RPX=75,
        RPX=76,
        DA=38,
        WS=39,

        RMOVE=40,
        RPLACE=41,
        PREDICTSENSOR=42,

        ADDOBS=50,
        DELETEOBS=51,
        MOVEOBS=52,
        NEWWORLD=53,

        DRAWROBOT=60,
        DRAWLINE=61,
        DRAWARC=62,

        QUIT=70,
        REALROBOT=71,
        SEND_MESSAGE=72,
        RECEIVE_MESSAGE=73,
        SIMULATEDROBOT=74,

        REFRESHALL=150,
        REFRESHALLTRACES=151,
        REFRESHACTTRACE=152,
        REFRESHENCTRACE=153,
        REFRESHALLSENSORS=154,
        REFRESHBPSENSOR=155,
        REFRESHIRSENSOR=156,
        REFRESHSNSENSOR=157,
        REFRESHLSSENSOR=158,
        REFRESHGRAPHICS=159
    };

/* reply message types */
//     typedef enum reply_message{
      enum reply_message{
        STATE=80,
        MOVED=81,
        REPLY=82,
        INFRARED=83,
        SONAR=84,
        LASER=85,
        COMPASS=86,
        BUMPER=87,
        CONFIGURATION=88,
        VELOCITY=89,
        ACCELERATION=90,
        ERROR=91,

        CREATE_ROBOT=101,
        CONNECT_ROBOT=102,
        DISCONNECT=103,

        GET_CONF=200,

        SPECIAL=300,

        MCHECK=400,
        INTERSECT=401
};


private:
    short type_;
    unsigned short size_;
    long  * mesg_    ;//[USER_BUFFER_LENGTH];
    long  * mesgNet_ ;//[USER_BUFFER_LENGTH];
};
};
#endif
