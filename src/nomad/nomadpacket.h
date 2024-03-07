//
// C++ Interface: nomadpacket
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADPACKET_H
#define NOMADPACKET_H

/* from command type.h  */

#define NUM_COMMANDS 36


#define MAX_USER_BUF     0xFFFF

/**
	@author Javier Garcia Misis
*/
namespace Nomad{

class Packet{
public:
    Packet();
    Packet(int length, int type);

    ~Packet();

    int getLength(){return length_;}
    int getType(){return type_;}
    int getSize(){return size_;}

    void addUnsignedInt(int num);
    void addSignedInt(int num);
    void add(unsigned char num);

    unsigned char get();
    unsigned int  getUnsignedInt();
    int           getSignedInt();
    long          getLong();
    unsigned long getUnsignedLong();
    void          getReset();

    unsigned char * getBuffer(){return buffer_;}
    void setBuffer(int length, unsigned char *buff);

    unsigned char * getSpecialBuffer(){return special_buffer_;}
    void setSpecialBuffer(unsigned char *buff){special_buffer_ = buff;}


    void pack();
    void unpack();
    bool check();

//     typedef enum type{
     enum type{
        AC=1,
        SP=2,
        PR=3,
        PA=4,
        VM=5,
        MV=43,
        CT=6,
        GS=7,
        NAK=8,
        ST=9,
        LP=10,
        TK=11,
        DP=12,
        ZR=13,
        CONF_IR=14,
        CONF_SN=15,
        CONF_CP=16,
        CONF_LS=17,
        CONF_TM=18,
        GET_IR=19,
        GET_SN=20,
        GET_RC=21,
        GET_RV=22,
        GET_RA=23,
        GET_CP=24,
        GET_LS=25,
        SETUP_LS=26,
        GET_BP=27,
        
        CONF_SG=28,
        GET_SG=29,
        
        DA=30,
        WS=31,
        
        ADD_OBS=32,
        DELETE_OBS=33,
        MOVE_OBS=34,
        
        CONF_SER= 35,
        PLACE_ROBOT=36,
        
        SPECIAL=128,
    };


private:
    int calculeCksum();
    void init(int length);

private:
    unsigned char * buffer_;
    int pos_;
    int repos_;
    int length_;
    int type_;
    int size_;

    unsigned char * special_buffer_;
    
};

}
#endif
