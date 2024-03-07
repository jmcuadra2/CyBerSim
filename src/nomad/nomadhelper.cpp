//
// C++ Implementation: nomadhelper
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <stdio.h>
#include <stdlib.h>

#include  "nomadhelper.h"

namespace Nomad{

char * convertAddr (const char *name, char *addr )
{
    int addrInt[10];

    sscanf(name, "%d.%d.%d.%d", &(addrInt[0]), &(addrInt[1]), &(addrInt[2]), &(addrInt[3]));
    addr[0] = addrInt[0];
    addr[1] = addrInt[1];
    addr[2] = addrInt[2];
    addr[3] = addrInt[3];
    return ( addr );
}


int low_half(unsigned char num)
{
  return (num % 16);
}

int high_half(unsigned char num)
{
  return (num / 16);
}

void signed_int_to_two_bytes(int n, unsigned char *byte_ptr)
{
  int sign_num;
  
  *byte_ptr = (unsigned char)(abs(n) % 256);
  byte_ptr++;
  if (n < 0) sign_num = 128; else sign_num = 0;
  *byte_ptr = (unsigned char)(sign_num + (abs(n) / 256));
}

void unsigned_int_to_two_bytes(int n, unsigned char *byte_ptr)
{
  *byte_ptr = (unsigned char)(abs(n) % 256);
  byte_ptr++;
  *byte_ptr = (unsigned char)(abs(n) / 256);
}

int two_bytes_to_signed_int(unsigned char low_byte,
                   unsigned char high_byte)
{
  int num;
  
  if (high_byte > 127)
    num = (-256 * (high_byte - 128)) - low_byte;
  else
    num = 256 * high_byte + low_byte;
  return (num);
}

unsigned int two_bytes_to_unsigned_int(unsigned char low_byte,
                          unsigned char high_byte)
{
  unsigned int num;
  
  num = 256 * high_byte + low_byte;
  return (num);
}


long combine_bumper_vector(unsigned char b1,
                  unsigned char b2,
                  unsigned char b3)
{
  long num;
  
  num = b1 + (b2 * 256) + (b3 * 65536);
  return (num);
}


/*
 * bits_to_byte - converts 8 bits into one byte. Helper function for ct()
 */
unsigned char bits_to_byte(char bt0, char bt1, char bt2, char bt3,
                           char bt4, char bt5, char bt6, char bt7)
{
  unsigned char  rbyte;
  
  if (bt0 > 0) bt0 = 1; else bt0 = 0;
  if (bt1 > 0) bt1 = 1; else bt1 = 0;
  if (bt2 > 0) bt2 = 1; else bt2 = 0;
  if (bt3 > 0) bt3 = 1; else bt3 = 0;
  if (bt4 > 0) bt4 = 1; else bt4 = 0;
  if (bt5 > 0) bt5 = 1; else bt5 = 0;
  if (bt6 > 0) bt6 = 1; else bt6 = 0;
  if (bt7 > 0) bt7 = 1; else bt7 = 0;
  
  rbyte = (unsigned char)(bt0 + (2*bt1) + (4*bt2) + (8*bt3) + (16*bt4) +
              (32*bt5) + (64*bt6) + (128*bt7));
  return (rbyte);
}



/* 
 * First some helper functions 
 */
void stuff_length_type(int length, int ptype, unsigned char *byte_ptr)
{
     byte_ptr++; /* skip the first byte of the buffer, which is 
            reserved for begin_pkg character */

     unsigned_int_to_two_bytes(length, byte_ptr);
     byte_ptr++; byte_ptr++;
     *byte_ptr = ptype;
}

void stuff_two_signed_int(int length, int ptype, int num1, int num2,
              unsigned char *byte_ptr)
{
     byte_ptr++; /* skip the first byte of the buffer, which is 
            reserved for begin_pkg character */

     unsigned_int_to_two_bytes(length, byte_ptr);
     byte_ptr++; byte_ptr++;
     *byte_ptr = ptype;
     byte_ptr++;
     signed_int_to_two_bytes(num1, byte_ptr);
     byte_ptr++; byte_ptr++;
     signed_int_to_two_bytes(num2, byte_ptr);
}

void stuff_three_unsigned_int(int length, int ptype, int num1, int num2,
                  int num3, unsigned char *byte_ptr)
{
     byte_ptr++; /* skip the first byte of the buffer, which is 
            reserved for begin_pkg character */

     unsigned_int_to_two_bytes(length, byte_ptr);
     byte_ptr++; byte_ptr++;
     *byte_ptr = ptype;
     byte_ptr++;
     unsigned_int_to_two_bytes(num1, byte_ptr);
     byte_ptr++; byte_ptr++;
     unsigned_int_to_two_bytes(num2, byte_ptr);
     byte_ptr++; byte_ptr++;
     unsigned_int_to_two_bytes(num3, byte_ptr);
}

void stuff_three_signed_int(int length, int ptype, int num1, int num2,
                int num3, unsigned char *byte_ptr)
{
     byte_ptr++; /* skip the first byte of the buffer, which is 
            reserved for begin_pkg character */

     unsigned_int_to_two_bytes(length, byte_ptr);
     byte_ptr++; byte_ptr++;
     *byte_ptr = ptype;
     byte_ptr++;
     signed_int_to_two_bytes(num1, byte_ptr);
     byte_ptr++; byte_ptr++;
     signed_int_to_two_bytes(num2, byte_ptr);
     byte_ptr++; byte_ptr++;
     signed_int_to_two_bytes(num3, byte_ptr);
}



}