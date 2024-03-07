//
// C++ Implementation: nomadpacket
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "../nomad/nomadpacket.h"
#include "../nomad/nomadhelper.h"
#include <stdlib.h>


/* to build a long out ouf four bytes */
#define LONG_B(x,y,z,q) ((((x) << 24) & 0xFF000000) | \
                         (((y) << 16) & 0x00FF0000) | \
                         (((z) <<  8) & 0x0000FF00) | \
                         ( (q)        & 0x000000FF) )


namespace Nomad{
Packet::Packet()
{
    special_buffer_ = NULL;
    type_ = 0;
    length_ = 0;
    size_ = 0;
    buffer_ = 0;
    pos_ = 0;
    getReset();
}

Packet::Packet(int length, int type)
{
    special_buffer_ = NULL;
    init(length);

    type_ = type;

    add(type);

}


void Packet::setBuffer(int length, unsigned char *buff){
    init(length);

    type_ = buff[0];

    for(int i = 0; i<=length;i++)
        add(buff[i]);

}

void Packet::init(int length){
    /* aqui hay una errata pero se mantiene por no cambiar el protocolo */
    length_ = length;
    size_ = length + 2 /*begin + length + type + data */;
    size_ += 1 /* checksum */;
    size_ += 1 /* end */;

    buffer_ = new unsigned char[size_];
    for(int i=0;i<size_;i++)
        buffer_[i] =0;
    pos_ = 0;
    getReset();
    add(1);/*el primer valor es un 1*/
    addUnsignedInt(length);

}

Packet::~Packet()
{
    if(buffer_ != 0)
        delete [] buffer_;
    buffer_ = 0;
    size_ = 0;
}

void Packet::addUnsignedInt(int num){
    add((unsigned char)(abs(num) % 256));
    add((unsigned char)(abs(num) / 256));
}


void Packet::addSignedInt(int num){
    int sign_num;
  
    add((unsigned char)(abs(num) % 256));
    if (num < 0) 
        sign_num = 128; 
    else 
        sign_num = 0;
    add((unsigned char)(sign_num + (abs(num) / 256)));
}

void Packet::add(unsigned char num){
    if(pos_>= size_) return;
    buffer_[pos_] = num;
    pos_++;
}

void Packet::getReset(){
    repos_ = 1;
}

unsigned char Packet::get(){
    unsigned char tmp;
    if(repos_>= size_){
        tmp = 0;
    }else{
        tmp =  buffer_[repos_];
    }
    repos_++;
    
    return tmp;
}

unsigned int Packet::getUnsignedInt(){
    unsigned char num1, num2;
    num1 = get();
    num2 = get();
    return two_bytes_to_unsigned_int(num1,num2);
}

int Packet::getSignedInt(){
    unsigned char num1, num2;
    num1 = get();
    num2 = get();
    return two_bytes_to_signed_int(num1,num2);
}


long Packet::getLong(){
    long tmp;
    unsigned char num1, num2, num3, num4;
    num1 = get();
    num2 = get();
    num3 = get();
    num4 = get();
  
    tmp = (long) LONG_B(num4,num3,num2,num1);
  
    if ( tmp & (1L << 31) )
        return ( -(tmp & ~(1L << 31) ) );
    else
        return ( tmp );
}

unsigned long Packet::getUnsignedLong(){
    unsigned char num1, num2, num3, num4;
    num1 = get();
    num2 = get();
    num3 = get();
    num4 = get();

    return (unsigned long) LONG_B(num4,num3,num2,num1);
}

int Packet::calculeCksum(){
    int i, chk_sum;

    chk_sum = 0;

    for ( i=0; i< size_-2; i++ )
        chk_sum = chk_sum + buffer_[i];
    chk_sum = chk_sum % 256;

    return chk_sum;

}

void Packet::pack(){
    buffer_[0] = 1;

    buffer_[size_ - 2] = calculeCksum();
    buffer_[size_ - 1] = 92;
}

void Packet::unpack(){
    length_ = getUnsignedInt();
    type_ = get();
}

bool Packet::check(){
    if(buffer_[0] != 1) return false;
    if(buffer_[size_ - 2] != calculeCksum()) return false;
    if(buffer_[size_ - 1] != 92) return false;
    return true;
}

};
