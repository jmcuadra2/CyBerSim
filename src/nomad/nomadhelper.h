//
// C++ Interface: nomadhelper
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
/*******************************************************
 *                                                     *
 * Helper functions for manipulating bytes and numbers *
 *                                                     *
 *******************************************************/

namespace Nomad{

char * convertAddr (const char *name, char *addr );
int low_half(unsigned char num);

int high_half(unsigned char num);

void signed_int_to_two_bytes(int n, unsigned char *byte_ptr);

void unsigned_int_to_two_bytes(int n, unsigned char *byte_ptr);

int two_bytes_to_signed_int(unsigned char low_byte,
                   unsigned char high_byte);

unsigned int two_bytes_to_unsigned_int(unsigned char low_byte, unsigned char high_byte);

long combine_bumper_vector(unsigned char b1,unsigned char b2, unsigned char b3);

unsigned char bits_to_byte(char bt0, char bt1, char bt2, char bt3,
                  char bt4, char bt5, char bt6, char bt7);

/* 
 * First some helper functions 
 */
void stuff_length_type(int length, int ptype, unsigned char *byte_ptr);

void stuff_two_signed_int(int length, int ptype, int num1, int num2,
              unsigned char *byte_ptr);

void stuff_three_unsigned_int(int length, int ptype, int num1, int num2,
                  int num3, unsigned char *byte_ptr);

void stuff_three_signed_int(int length, int ptype, int num1, int num2,
                int num3, unsigned char *byte_ptr);



};
