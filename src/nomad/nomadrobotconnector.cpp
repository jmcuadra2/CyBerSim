//
// C++ Implementation: nomadrobotconnector
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <arpa/inet.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <memory.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <sys/time.h>           

#include "../nomad/nomadrobotconnector.h"
#include "../nomad/nomadhelper.h"

#include <sstream>

/* error types */
#define SERIAL_OPEN_ERROR 1
#define SERIAL_WRITE_ERROR 2
#define SERIAL_READ_ERROR 3
#define SERIAL_PKG_ERROR 4
#define SERIAL_TIMEOUT_ERROR 5

/* serial setting */
#define RE_XMIT 0            /* not re-transmit when failed */
#define NORMAL_TIMEOUT     1 /* 1 second */
#define CONNECT_TIMEOUT   10 /* 10 seconds */
#define SPECIAL_TIMEOUT   30 /* used for user define package */

/* 
 * these macros enable the user to determine if the pos-attachment
 * is desired for a specific sensor, the argument is Smask[ SMASK_POS_DATA ]
 * to avoid overlap with Nclient.h suffix I for internal
 */
#define POS_INFRARED_PI(x)  ( ( (x) & POS_INFRARED ) ? 1 : 0 )
#define POS_SONAR_PI(x)     ( ( (x) & POS_SONAR    ) ? 1 : 0 )
#define POS_BUMPER_PI(x)    ( ( (x) & POS_BUMPER   ) ? 1 : 0 )
#define POS_LASER_PI(x)     ( ( (x) & POS_LASER    ) ? 1 : 0 )
#define POS_COMPASS_PI(x)   ( ( (x) & POS_COMPASS  ) ? 1 : 0 )

namespace Nomad{

int serial_ready (int fd, int wait)
{
    fd_set          lfdvar;
    int             ready;
    struct timeval  timeout;

    FD_ZERO(&lfdvar);
    FD_SET(fd, &lfdvar);

    timeout.tv_sec = wait;
    timeout.tv_usec = 0;

    ready = select(fd+1, &lfdvar, NULL, NULL, &timeout);
    return(ready);
}

RobotConnector::RobotConnector():Connector(1)
{
    host_ = "";
    port_ = 0;
    serial_ = "";
    bautrate_ = 0;
    Fd_ = -1;
}

RobotConnector::~RobotConnector()
{
}

bool RobotConnector::disconnect_robot(){
    if ( Fd_ != -1 )
        close ( Fd_ );

    Fd_ = -1;
    return true;
}

bool RobotConnector::ac(int t_ac, int s_ac, int r_ac){
    Packet packet(8, Packet::AC);
    
    packet.addUnsignedInt(t_ac);
    packet.addUnsignedInt(s_ac);
    packet.addUnsignedInt(r_ac);

    //    stuff_three_unsigned_int(8, AC, t_ac, s_ac, r_ac, outbuf);
    return Comm_Robot(packet);
}

bool RobotConnector::sp(int t_sp, int s_sp, int r_sp){
    Packet packet(8, Packet::SP);

    packet.addUnsignedInt(t_sp);
    packet.addUnsignedInt(s_sp);
    packet.addUnsignedInt(r_sp);

    //    stuff_three_unsigned_int(8, SP, t_sp, s_sp, r_sp, outbuf);
    return(Comm_Robot(packet));
}

bool RobotConnector::pr(int t_pr, int s_pr, int r_pr){
    Packet packet(8, Packet::PR);

    packet.addSignedInt(t_pr);
    packet.addSignedInt(s_pr);
    packet.addSignedInt(r_pr);

    //    stuff_three_signed_int(8, PR, t_pr, s_pr, r_pr, outbuf);
    return(Comm_Robot(packet));
}

bool RobotConnector::pa(int t_pa, int s_pa, int r_pa){
    Packet packet(8, Packet::PA);

    packet.addSignedInt(t_pa);
    packet.addSignedInt(s_pa);
    packet.addSignedInt(r_pa);

    //    stuff_three_signed_int(8, PA, t_pa, s_pa, r_pa, outbuf);
    return(Comm_Robot(packet));
}





bool RobotConnector::vm(int t_vm, int s_vm, int r_vm){
    Packet packet(8, Packet::VM);

    packet.addSignedInt(t_vm);
    packet.addSignedInt(s_vm);
    packet.addSignedInt(r_vm);
    
    //    stuff_three_signed_int(8, VM, t_vm, s_vm, r_vm, outbuf);
    return(Comm_Robot(packet));
}

bool RobotConnector::mv(int t_mode, int t_mv, int s_mode, int s_mv, int r_mode, int r_mv){
    Packet packet(6 * 2 + 2, Packet::MV);

    packet.addUnsignedInt(t_mode);
    packet.addSignedInt(t_mv);

    packet.addUnsignedInt(s_mode);
    packet.addSignedInt(s_mv);

    packet.addUnsignedInt(r_mode);
    packet.addSignedInt(r_mv);

    return ( Comm_Robot(packet) );
}

bool RobotConnector::ct ( void ){
    unsigned char b0, b1, b2, b3, b4, b5, b6;


    /*
      for ( i = 0; i < NUM_MASK; i++ )
        usedSmask[i] = Smask[i];
    */

    /* first encode Mask */
    b0 = bits_to_byte ( status_.getSmaskVector() [1],
            status_.getSmaskVector() [2],
            status_.getSmaskVector() [3],
            status_.getSmaskVector() [4],
            status_.getSmaskVector() [5],
            status_.getSmaskVector() [6],
            status_.getSmaskVector() [7],
            status_.getSmaskVector() [8] );

    b1 = bits_to_byte ( status_.getSmaskVector() [9],
            status_.getSmaskVector() [10],
            status_.getSmaskVector() [11],
            status_.getSmaskVector() [12],
            status_.getSmaskVector() [13],
            status_.getSmaskVector() [14],
            status_.getSmaskVector() [15],
            status_.getSmaskVector() [16] );

    b2 = bits_to_byte ( status_.getSmaskVector() [17],
            status_.getSmaskVector() [18],
            status_.getSmaskVector() [19],
            status_.getSmaskVector() [20],
            status_.getSmaskVector() [21],
            status_.getSmaskVector() [22],
            status_.getSmaskVector() [23],
            status_.getSmaskVector() [24] );

    b3 = bits_to_byte ( status_.getSmaskVector() [25],
            status_.getSmaskVector() [26],
            status_.getSmaskVector() [27],
            status_.getSmaskVector() [28],
            status_.getSmaskVector() [29],
            status_.getSmaskVector() [30],
            status_.getSmaskVector() [31],
            status_.getSmaskVector() [32] );

    b4 = bits_to_byte ( status_.getSmaskVector() [33],
            status_.getSmaskVector() [34],
            status_.getSmaskVector() [35],
            status_.getSmaskVector() [36],
            status_.getSmaskVector() [37],
            status_.getSmaskVector() [38],
            status_.getSmaskVector() [39],
            status_.getSmaskVector() [40] );

    b5 = bits_to_byte ( status_.getSmaskVector() [42], 0, 0, 0, 0, 0, 0, 0 );
    /* we pack the pos mask into b6 */
    b6 = bits_to_byte ( status_.getSmaskVector() [43],
            POS_INFRARED_PI ( status_.getSmaskVector() [ SMASK_POS_DATA ] ),
            POS_SONAR_PI ( status_.getSmaskVector() [ SMASK_POS_DATA ] ),
            POS_BUMPER_PI ( status_.getSmaskVector() [ SMASK_POS_DATA ] ),
            POS_LASER_PI ( status_.getSmaskVector() [ SMASK_POS_DATA ] ),
            POS_COMPASS_PI ( status_.getSmaskVector() [ SMASK_POS_DATA ] ),
            0,0 );

    Packet packet(9, Packet::CT);
    packet.add(b0);
    packet.add(b1);
    packet.add(b2);
    packet.add(b3);
    packet.add(b4);
    packet.add(b5);
    packet.add(b6);

    return ( Comm_Robot(packet) );
}

bool RobotConnector::gs ( void )
{
    Packet packet(2, Packet::GS);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::st ( void )
{
    Packet packet( 2, Packet::ST);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::lp ( void )
{
    Packet packet( 2, Packet::LP);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::tk ( char *talk_string )
{
    int i, length;

    length = 3 + strlen ( talk_string );
    Packet packet( length, Packet::TK);
    for ( i=3; i<length; i++ )
    {
        packet.add(talk_string[i-3]);
    }
    packet.add(0); /* null terminate the string */
    return ( Comm_Robot(packet) );
}

bool RobotConnector::dp(int x, int y){
    Packet packet(6, Packet::DP);
    packet.addSignedInt(x);
    packet.addSignedInt(y);

    return(Comm_Robot(packet));
}

bool RobotConnector::zr(){
    bool temp;
    
    wait_time_ =  120;   /* maximum of 2 minutes */
    /* zr() takes maximum of 120 seconds */
    Packet packet(2, Packet::ZR);
    temp = Comm_Robot( packet );
    wait_time_ = NORMAL_TIMEOUT;
    return(temp);
}

bool RobotConnector::conf_ir ( int history, int order[16] )
{
    int i;

    Packet packet( 19, Packet::CONF_IR);

    if ( history > 10 )
        history = 10;
    packet.add(( unsigned char ) history);

    for ( i=0; i<16; i++ )
    {
        packet.add(( unsigned char ) order[i]);
    }
    return ( Comm_Robot(packet) );
}

bool RobotConnector::conf_sn ( int rate, int order[16] )
{
    int i;

    Packet packet( 19, Packet::CONF_SN);

    packet.add(( unsigned char ) rate);

    for ( i=0; i<16; i++ )
    {
        packet.add(( unsigned char ) order[i]);
    }

    return ( Comm_Robot(packet) );
}

bool RobotConnector::conf_cp ( int mode )
{
    Packet packet( 3, Packet::CONF_CP);

    packet.add(( unsigned char ) mode);

    return ( Comm_Robot(packet) );
}

bool RobotConnector::conf_ls ( unsigned int mode,
                               unsigned int threshold,
                               unsigned int width,
                               unsigned int num_data,
                               unsigned int processing )
{

    status_.setLaserMode(mode);
    Packet packet( 8, Packet::CONF_LS);

    if ( mode == 51 )
        packet.add(35); /* special case */
    else
        packet.add(( unsigned char ) mode);


    packet.add(( unsigned char ) threshold);
    packet.add(( unsigned char ) width);
    packet.addUnsignedInt(num_data);
    packet.add(( unsigned char ) processing);

    return ( Comm_Robot(packet) );
}

bool RobotConnector::conf_tm ( unsigned char timeout )
{

    Packet packet( 3, Packet::CONF_TM);
    packet.add(( unsigned char ) timeout);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::conf_ser ( unsigned char port, unsigned short baud )
{
    Packet packet( 5, Packet::CONF_SER);

    packet.add(port);
    packet.addUnsignedInt(baud);

    return ( Comm_Robot ( packet ) );
}


bool RobotConnector::get_ir ( void )
{
    Packet packet( 2, Packet::GET_IR);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::get_sn ( void )
{
    Packet packet( 2, Packet::GET_SN);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::get_rc ( void )
{
    Packet packet( 2, Packet::GET_RC);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::get_rv ( void )
{
    Packet packet( 2, Packet::GET_RV);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::get_ra ( void )
{
    Packet packet( 2, Packet::GET_RA);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::get_cp ( void )
{
    Packet packet( 2, Packet::GET_CP);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::get_ls ( void )
{
    Packet packet( 2, Packet::GET_LS);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::get_bp ( void )
{
    Packet packet(2, Packet::GET_BP);
    return ( Comm_Robot ( packet ) );
}

bool RobotConnector::conf_sg ( unsigned int threshold,
                               unsigned int min_points,
                               unsigned int gap )
{

    Packet packet( 5, Packet::CONF_SG);

    packet.add(threshold);
    packet.add(min_points);
    packet.add(gap);

    return ( Comm_Robot(packet) );
}

bool RobotConnector::get_sg ( void )
{
    Packet packet( 2, Packet::GET_SG);
    return ( Comm_Robot(packet) );
}

bool RobotConnector::da ( int th, int tu )
{
    Packet packet( 6, Packet::DA);

    packet.addSignedInt(th);
    packet.addSignedInt(tu);

    return ( Comm_Robot(packet) );
}

bool RobotConnector::ws ( unsigned char t_ws, unsigned char s_ws,
                          unsigned char r_ws, unsigned char timeout )
{
    bool ret;

    Packet packet( 6, Packet::WS);

    if ( t_ws > 1 ) t_ws = 1;

    packet.add(t_ws);

    if ( s_ws > 1 ) s_ws = 1;

    packet.add(s_ws);

    if ( r_ws > 1 ) r_ws = 1;

    packet.add(r_ws);

    packet.add(timeout);

    wait_time_ =  timeout + NORMAL_TIMEOUT;
    ret = Comm_Robot(packet);
    wait_time_ = NORMAL_TIMEOUT;

    return ret;
}

bool RobotConnector::get_rpx ( long *robot_pos )
{
    *robot_pos = -1;

    return false;
}



/*****************************
*                           *
* World Interface Functions *
*                           *
*****************************/

bool RobotConnector::add_obstacle(long obs[2*MAX_VERTICES+1]){
    obs[0] = obs[0];
    return true;
}

bool RobotConnector::delete_obstacle(long obs[2*MAX_VERTICES+1]){
    obs[0] = obs[0];
    return true;
}

bool RobotConnector::move_obstacle(long obs[2*MAX_VERTICES+1], long dx, long dy){
    obs[0] = obs[0];
    dx = dx;
    dy = dy;
    return true;
}

bool RobotConnector::new_world(void){
    return true;
}

/****************************
*                          *
* Graphics refresh control *
*                          *
****************************/
bool RobotConnector::refresh_all(void){
    return true;
}
bool RobotConnector::refresh_all_traces(void){
    return true;
}
bool RobotConnector::refresh_actual_trace(void){
    return true;
}
bool RobotConnector::refresh_encoder_trace(void){
    return true;
}
bool RobotConnector::refresh_all_sensors(void){
    return true;
}
bool RobotConnector::refresh_bumper_sensor(void){
    return true;
}
bool RobotConnector::refresh_infrared_sensor(void){
    return true;
}
bool RobotConnector::refresh_sonar_sensor(void){
    return true;
}
bool RobotConnector::refresh_laser_sensor(void){
    return true;
}
bool RobotConnector::refresh_client_graphics(void){
    return true;
}


/*******************************
*                             *
* Miscellaneous robot control *
*                             *
*******************************/

bool RobotConnector::place_robot(int x, int y, int th, int tu){
    if (!dp(x, y) || !da(th, tu))
        return false;

    return true;
}

bool RobotConnector::special_request ( unsigned char *user_send_buffer,
                                       unsigned char *user_receive_buffer )
{
    int i, length;
    bool temp;

    length = 2 + user_send_buffer[0] + 256 * user_send_buffer[1];
    if ( length>USER_BUFFER_LENGTH-5 )
    {
        printf ( "Data + protocol bytes exceeding %d, truncating\n",
                 USER_BUFFER_LENGTH );
        /* num_data already includes the 4 bytes of user packets protocol */
        length  = USER_BUFFER_LENGTH-5;
    }
    Packet packet( length, Packet::SPECIAL);

    for ( i=0; i<length; i++ )
    {
        packet.add(user_send_buffer[i]);
    }

    /*
     * Comm_Robot will process the returned package and write the
     * data into special_buffer that we assign to be the buffer
     * that the caller provided.
     */

    special_buffer_ = user_receive_buffer;
    wait_time_ = SPECIAL_TIMEOUT;
    temp = Comm_Robot(packet);
    wait_time_ = NORMAL_TIMEOUT;
    special_buffer_ = NULL;
    return ( temp );
}

/*******************************
*                             *
* Graphic Interface Functions *
*                             *
*******************************/

bool RobotConnector::draw_robot ( long x, long y, int th, int tu, int mode )
{
    x = x;
    y = y;
    th = th;
    tu = tu;
    mode = mode;
    return true;
}
bool RobotConnector::draw_line ( long x_1, long y_1, long x_2, long y_2, int mode )
{
    x_1 = x_1;
    y_1 = y_1;
    x_2 = x_2;
    y_2 = y_2;
    mode = mode;
    return true;
}
bool RobotConnector::draw_arc ( long x_0, long y_0, long w, long h, int th1, int th2, int mode )
{
    x_0 = x_0;
    y_0 = y_0;
    w   = w;
    h   = h;
    th1 = th1;
    th2 = th2;
    mode = mode;
    return true;
}

/*************************************
*                                   *
* Miscellaneous Interface Functions *
*                                   *
*************************************/

bool RobotConnector::server_is_running(void){
    return(connect_robot());
}
bool RobotConnector::quit_server(void){
    return true;
}
bool RobotConnector::real_robot(void){
    return true;
}
bool RobotConnector::simulated_robot(void){
    return true;
}
bool RobotConnector::predict_sensors(int x, int y, int th, int tu, long *state, int *laser){
    x   = x;
    y   = y;
    th  = th;
    tu  = tu;
    state[0] = state[0];
    laser[0] = laser[0];
    return true;
}
bool RobotConnector::motion_check(long type, double a1, double a2, double a3, double a4, 
                                  double a5, double a6, double a7, double collide[3]){
    type = type;
    a1   = a1;
    a2   = a2;
    a3   = a3;
    a4   = a4;
    a4   = a5;
    a4   = a6;
    a4   = a7;
    collide[0] = collide[0];
    return false;
}
bool RobotConnector::get_robot_conf(long *conf){
    conf[0] = conf[0];
    return true;
}

/* Otras Funciones  */


/* 
 * read response from the robot
 */


unsigned char RobotConnector::GETC()
{
    if ( bufp_<bufe_ )
        return ( *bufp_++ );//TODO: cambiar
    return ( buf_fill() );
}


bool RobotConnector::comm_check (){
    int i;

    if ( Fd_ == -1 )
    {
        fprintf ( stderr,
                  "Trying again to reestablish connection with the robot...\n"
                  "                          " );
        fflush ( stderr );
        for ( i = 0; i < 3; i++ ){
            if (connect_robot ())
                break;
            if(i<2){
                sleep ( 5 );
                fprintf ( stderr, "Trying again...           " );
                fflush ( stderr );
            }
        }
        if (i == 3)
        {
            fprintf ( stderr, "Failed to reestablish connection.  Command aborted.\n" );
            return false;
        }
        fprintf ( stderr, "Successful!  Continuing with command.\n" );
    }
    return true;
}

bool RobotConnector::comm_flush (){
    int i;
    unsigned char response[BUFSIZE];
    bool respondedp;
    fd_set          lfdvar;
    struct timeval  timeout;

    FD_ZERO ( &lfdvar );
    FD_SET ( Fd_, &lfdvar );

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    while ( select ( Fd_+1, &lfdvar, NULL, NULL, &timeout ) != 0 )
    {
        /* Flush buffer */
        respondedp = read ( Fd_, response, BUFSIZE );
        /* Check for errors, such as lost connection. */
        if ( respondedp <= 0 && errno != EWOULDBLOCK ) {
            close ( Fd_ );
            Fd_ = -1;
            fprintf ( stderr,
                      "Lost communication with robot.\nAttempting to reconnect..." );
            fflush ( stderr );
            for ( i = 0; i < 3; i++ ){
                if (connect_robot ())
                    break;
                if(i<2){
                    sleep ( 5 );
                    fprintf ( stderr, "Trying again...           " );
                    fflush ( stderr );
                }
            }
            if (i == 3)            {
                fprintf ( stderr, "Failed to reestablish connection.  Command aborted.\n" );
                return false;
            }else{
                fprintf ( stderr, "Successful!  Continuing with command.\n" );
            }
        }
    }
    return true;
}


bool RobotConnector::Comm_Robot (Packet & packet)
{
    Packet response;
    //    bool respondedp;
    int i;

    if (!comm_check ()) return false;
    if (!comm_flush ()) return false;

    Write_Pkg ( packet );
    /*
    while ( ! ( respondedp = Read_Pkg ( response ) ) && ( RE_XMIT ) )
    {
        Write_Pkg ( packet );
    }
*/
    if ( !Read_Pkg ( response ) ){
        printf ( "Last command packet transmitted:\n" );
        for ( i = 0; i < packet.getSize(); i++ )
            printf ( "%2.2x ", packet.getBuffer()[i] );
        printf ( "\n" );

        return false;
    } else {
        response.setSpecialBuffer(special_buffer_);
        if ( status_.Process_Robot_Resp ( response ) ) {
            return true;
        } else {
            printf ( "error in robot response\n" );
            return false;
        }
    }
}

/*
 * creates a package by adding initial byte, checksum, and end byte
 * and send the package to robot
 */
bool RobotConnector::Write_Pkg (Packet & packet)
{
    int nleft, nwritten;
    unsigned char * outbuf;

    /* create a package */
    packet.pack();
    outbuf = packet.getBuffer();

    /* send package */
    for ( nleft = packet.getSize(); nleft > 0; )
    {
        nwritten = write ( Fd_ , outbuf, nleft );
        if ( nwritten <= 0 )
        {
            status_.setError(SERIAL_WRITE_ERROR);
            return false;
        }
        nleft = nleft - nwritten;
        outbuf = outbuf + nwritten;
    }
    return true;
}

/*
 * getting package from robot and
 * check for package error: initial byte = 1, end byte = 92, check sum
 */
bool RobotConnector::Read_Pkg (Packet & packet)
{
    unsigned char * inbuf = 0;
    int i, length;
    unsigned char ichar, ichar2;

    if ( ! ( serial_ready ( Fd_, wait_time_ ) ) )
    {
        status_.setError(SERIAL_TIMEOUT_ERROR);
        return false;
    }

    status_.setError(0);

    /* read the begin packet character, it should be 1 */
    ichar = ( unsigned char ) GETC ();
    if ( status_.getError()==0 ){
        if ( ichar != 1 )
        {
            printf ( "start byte error: %u\n", ichar );
            status_.setError(SERIAL_PKG_ERROR);
        }
    }

    if (status_.getError()==0 ) {
        /* read length, it should be >0 */
        ichar = GETC ();
        ichar2 = GETC ();
        if ( status_.getError()==0 )
        {
            length = two_bytes_to_unsigned_int ( ichar, ichar2 );
            if ( length < 1 )
            {
                printf ( "length byte error\n" );
                status_.setError(SERIAL_PKG_ERROR);
            }
        }
    }

    if ( status_.getError()==0 )
    {
        inbuf = new unsigned char[length+1];
    }

    /* get the data portion of the message */
    i = 0;
    while ( ( status_.getError()==0 ) && ( i<=length ) )
    {
        ichar = GETC ();
        if ( status_.getError()==0 )
        {
            inbuf[i] = ichar;
        }
        i++;
    }

    if ( status_.getError()==0 )
    {
        packet.setBuffer(length,inbuf);
        packet.unpack();

        /* check chk_sum and end_pkg */
        if (!packet.check())
        {
            printf ( "check sum error\n" );
            status_.setError(SERIAL_PKG_ERROR);
        }
    }

    if(inbuf != 0)
        delete [] inbuf;

    if ( ( status_.getError()!=0 ) && ( status_.getError() != SERIAL_TIMEOUT_ERROR ) )
    {
        printf ( "emptying buffer\n" );
        buf_fill ();  /* read everything else in the serial line into buffer */
        bufp_ = bufe_; /* and flush it */
    }

    if ( status_.getError()!=0 )
        return false;
    else
        return true;
}

int RobotTCPConnector::DEFAULT_ROBOT_TCP_PORT = 65001;

RobotTCPConnector::RobotTCPConnector(std::string host,int port):RobotConnector()
{
    host_ = host;
    port_ = port;
}

RobotTCPConnector::~RobotTCPConnector(){

}

bool RobotTCPConnector::connect_robot(){
    struct hostent *hp;
    struct sockaddr_in serv_addr;
    char addr[10];
    int ret, i;
    socklen_t retlen;
    unsigned char ir_mask[16],sn_mask[16],cf_mask[4],vl_mask[3];
    unsigned char cp_mask,bp_mask,ls_mask,pos_mask, byte;

    if ( ( ( hp = gethostbyname ( host_.c_str() ) ) == NULL ) )
    {
        convertAddr ( host_.c_str(), addr );
        if ( addr[0] != 0 || addr[1] != 0 || addr[2] != 0 || addr[3] != 0 )
        {
            memset ( ( char * ) &serv_addr, 0, sizeof ( serv_addr ) );
            memcpy ( ( char * ) & ( serv_addr.sin_addr ), addr, 4 );
        }
        else
        {
            //fprintf ( stderr, "Machine %s not valid.\n", device );
            return false;
        }
    }
    else
    {
        memset ( ( char * ) &serv_addr, 0, sizeof ( serv_addr ) );
        memcpy ( ( char * ) & ( serv_addr.sin_addr ), hp->h_addr_list[0], hp->h_length );
    }

    serv_addr.sin_family = AF_INET;  /* address family */

    /* TCP port number */
    if ( port_ == 0 )
    {
        port_ = DEFAULT_ROBOT_TCP_PORT;
    }
    serv_addr.sin_port = htons ( port_ );

    if ( ( Fd_ = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        //fprintf ( stderr, "Error: in open_socket_to_send_data, socket failed.\n" );
        return false;
    }
    fcntl ( Fd_, F_SETFL, O_NDELAY );
    if ( connect ( Fd_, ( struct sockaddr * ) &serv_addr, sizeof ( serv_addr ) ) < 0 )
    {
        if ( errno == EINPROGRESS )
        {
            fd_set          lfdvar;
            struct timeval  timeout;

            FD_ZERO ( &lfdvar );
            FD_SET ( Fd_, &lfdvar );

            timeout.tv_sec = ( long ) CONNECT_TIMEOUT;
            timeout.tv_usec = ( long ) 0;

            if ( select ( Fd_+1, NULL, &lfdvar, NULL, &timeout ) == 0 )
            {
                //fprintf ( stderr, "Error: connect timed out.\n" );
                close ( Fd_ );
                Fd_ = -1;
                return false;
            }
            else
            {
                errno = 0;
                retlen = 4;
                if ( getsockopt ( Fd_, SOL_SOCKET, SO_ERROR, ( char * ) &ret, &retlen ) == 0 )
                {
                    if ( ret != 0 )
                        errno = ret;
                    if ( errno != 0 )
                    {
                        //perror ( "Error: connect failed" );
                        close ( Fd_ );
                        Fd_ = -1;
                        return false;
                    }
                }
            }
        }
        else
        {
            //perror ( "Error: connect failed" );
            close ( Fd_ );
            Fd_ = -1;
            return false;
        }
    }

    wait_time_ = NORMAL_TIMEOUT;

    printf ( "Robot <-> Host TCP/IP communication setup\n" );
    printf ( "(machine %s on port %d)\n", host_.c_str(), port_ );

    /* Read configuration data */
    byte = GETC ();
    ir_mask[ 0] = byte & ( 1 << 0 ) ? 1 : 0;
    ir_mask[ 1] = byte & ( 1 << 1 ) ? 1 : 0;
    ir_mask[ 2] = byte & ( 1 << 2 ) ? 1 : 0;
    ir_mask[ 3] = byte & ( 1 << 3 ) ? 1 : 0;
    ir_mask[ 4] = byte & ( 1 << 4 ) ? 1 : 0;
    ir_mask[ 5] = byte & ( 1 << 5 ) ? 1 : 0;
    ir_mask[ 6] = byte & ( 1 << 6 ) ? 1 : 0;
    ir_mask[ 7] = byte & ( 1 << 7 ) ? 1 : 0;
    byte = GETC ();
    ir_mask[ 8] = byte & ( 1 << 0 ) ? 1 : 0;
    ir_mask[ 9] = byte & ( 1 << 1 ) ? 1 : 0;
    ir_mask[10] = byte & ( 1 << 2 ) ? 1 : 0;
    ir_mask[11] = byte & ( 1 << 3 ) ? 1 : 0;
    ir_mask[12] = byte & ( 1 << 4 ) ? 1 : 0;
    ir_mask[13] = byte & ( 1 << 5 ) ? 1 : 0;
    ir_mask[14] = byte & ( 1 << 6 ) ? 1 : 0;
    ir_mask[15] = byte & ( 1 << 7 ) ? 1 : 0;
    byte = GETC ();
    sn_mask[ 0] = byte & ( 1 << 0 ) ? 1 : 0;
    sn_mask[ 1] = byte & ( 1 << 1 ) ? 1 : 0;
    sn_mask[ 2] = byte & ( 1 << 2 ) ? 1 : 0;
    sn_mask[ 3] = byte & ( 1 << 3 ) ? 1 : 0;
    sn_mask[ 4] = byte & ( 1 << 4 ) ? 1 : 0;
    sn_mask[ 5] = byte & ( 1 << 5 ) ? 1 : 0;
    sn_mask[ 6] = byte & ( 1 << 6 ) ? 1 : 0;
    sn_mask[ 7] = byte & ( 1 << 7 ) ? 1 : 0;
    byte = GETC ();
    sn_mask[ 8] = byte & ( 1 << 0 ) ? 1 : 0;
    sn_mask[ 9] = byte & ( 1 << 1 ) ? 1 : 0;
    sn_mask[10] = byte & ( 1 << 2 ) ? 1 : 0;
    sn_mask[11] = byte & ( 1 << 3 ) ? 1 : 0;
    sn_mask[12] = byte & ( 1 << 4 ) ? 1 : 0;
    sn_mask[13] = byte & ( 1 << 5 ) ? 1 : 0;
    sn_mask[14] = byte & ( 1 << 6 ) ? 1 : 0;
    sn_mask[15] = byte & ( 1 << 7 ) ? 1 : 0;
    byte = GETC ();
    bp_mask    = byte & ( 1 << 0 ) ? 1 : 0;
    cf_mask[0] = byte & ( 1 << 1 ) ? 1 : 0;
    cf_mask[1] = byte & ( 1 << 2 ) ? 1 : 0;
    cf_mask[2] = byte & ( 1 << 3 ) ? 1 : 0;
    cf_mask[3] = byte & ( 1 << 4 ) ? 1 : 0;
    vl_mask[0] = byte & ( 1 << 5 ) ? 1 : 0;
    vl_mask[1] = byte & ( 1 << 6 ) ? 1 : 0;
    vl_mask[2] = byte & ( 1 << 7 ) ? 1 : 0;
    byte = GETC ();
    cp_mask = byte & 1;
    byte = GETC ();
    ls_mask = byte & 1;
    pos_mask = byte >> 1;

    status_.getSmaskVector()[0] = pos_mask;
    for ( i = 0; i < 16; i++ )
        status_.getSmaskVector()[i+1] = ir_mask[i];

    for ( i = 0; i < 16; i++ )
        status_.getSmaskVector()[i+17] = sn_mask[i];

    status_.getSmaskVector()[33] = bp_mask;

    for ( i = 0; i < 4; i++ )
        status_.getSmaskVector()[i+34] = cf_mask[i];

    for ( i = 0; i < 3; i++ )
        status_.getSmaskVector()[i+38] = vl_mask[i];
    status_.getSmaskVector()[42] = ls_mask;
    status_.getSmaskVector()[43] = cp_mask;

    /* get laser mode, num_points, processing */
    byte = GETC ();
    status_.setLaserMode(byte);
    byte = GETC ();
    byte = GETC ();
    byte = GETC ();

    return true;
}

unsigned char RobotTCPConnector::buf_fill()
{
    int n;

    if ( serial_ready ( Fd_, 100) )//antes 100
    {
        n = read ( Fd_, buf_, BUFSIZE );
        if ( n < 0 ) {
            //printf ( "TCP/IP communication broken.\n" );
            status_.setError(SERIAL_READ_ERROR);
            return 0;
        } else if ( n == 0 ) {
            //printf ( "TCP/IP select/read error\n" );
            status_.setError(SERIAL_READ_ERROR);
            return ( 0 );
        }
    } else {
        //printf ( "TCP/IP read timeout error.\n" );
        status_.setError(SERIAL_TIMEOUT_ERROR);
        return 0;
    }

    bufp_ = &buf_[1];
    bufe_ = buf_ + n;
    return ( buf_[0] );
}








int RobotSERIALConnector::DEFAULT_SERIAL_BAUD = 38400;

RobotSERIALConnector::RobotSERIALConnector(int serial,int baudios):RobotConnector()
{
    std::stringstream sstmp;

    sstmp << "/dev/ttyS" << serial;

    serial_ = sstmp.str();
    bautrate_ = baudios;
    Fd_ = -1;
}

RobotSERIALConnector::~RobotSERIALConnector(){

}
bool RobotSERIALConnector::connect_robot()
{
    struct termios info;

    if ( Fd_ != -1 )
        close ( Fd_ );

    if ( ( Fd_=open ( serial_.c_str(), O_RDWR|O_NONBLOCK ) ) < 0 )
    {
        //perror ( "Error opening serial port" );
        return false;
    }

    if ( tcgetattr ( Fd_, &info ) < 0 )
    {
        //perror ( "Error using TCGETS in ioctl." );
        close ( Fd_ );
        Fd_ = -1;
        return false;
    }

    /* restore old values to unhang the bastard, if hung */
    info.c_iflag=1280;
    info.c_oflag=5;
    info.c_cflag=3261;
    info.c_lflag=35387;

    if ( tcsetattr ( Fd_, TCSANOW, &info ) < 0 )
    {
        //perror ( "Error using TCSETS in ioctl." );
        close ( Fd_ );
        Fd_ = -1;
        return false;
    }
    close ( Fd_ );

    if ( ( Fd_ = open ( serial_.c_str(), O_RDWR ) ) == -1 )
    {
        //perror ( "Error opening serial port" );
        status_.setError(SERIAL_OPEN_ERROR);
        return false;
    }

    if ( tcgetattr ( Fd_, &info ) < 0 )
    {
        //perror ( "Error using TCGETS in ioctl." );
        status_.setError(SERIAL_OPEN_ERROR);
        close ( Fd_ );
        Fd_ = -1;
        return false;
    }

    if ( bautrate_ != 4800 && bautrate_ != 9600 && bautrate_ != 19200 && bautrate_ != 38400 )
    {
        if ( bautrate_ != 0 )
        {
            fprintf ( stderr, "Invalid baud rate %d, using %d\n", bautrate_,
                      DEFAULT_SERIAL_BAUD );
        }
        bautrate_ = DEFAULT_SERIAL_BAUD;
    }

    info.c_iflag = 0;
    info.c_oflag = 0;
    info.c_lflag = 0;
    switch ( bautrate_ ) /* serial port rate */
    {
    case 4800:
        info.c_cflag = B4800 | CS8 | CREAD | CLOCAL;
        break;
    case 9600:
        info.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
        break;
    case 19200:
        info.c_cflag = B19200 | CS8 | CREAD | CLOCAL;
        break;
    case 38400:
        info.c_cflag = B38400 | CS8 | CREAD | CLOCAL;
        break;
    default:
        break;
    }
    /* set time out on serial read */
#if 1
    info.c_cc[VMIN] = 0;
    info.c_cc[VTIME] = 10;
#endif
    wait_time_ = NORMAL_TIMEOUT;

    if ( tcsetattr ( Fd_,TCSANOW,&info ) < 0 )
    {
        //perror ( "Error using TCSETS in ioctl." );
        status_.setError(SERIAL_OPEN_ERROR);
        close ( Fd_ );
        Fd_ = -1;
        return false;
    }

    printf ( "Robot <-> Host serial communication setup\n" );
    printf ( "(%d baud using %s)\n", bautrate_, serial_.c_str() );

    /* Send init_sensors to make sure that server and robot are synchronized */
    init_sensors();
    
    return true;
}

unsigned char RobotSERIALConnector::buf_fill()
{
    int n;

    n = read ( Fd_, buf_, BUFSIZE );
    if ( n < 0 ){
        //printf ( "error reading serial port\n" );
        status_.setError(SERIAL_READ_ERROR);
        return ( 0 );
    }else if ( n == 0 ){
        //printf ( "serial port read timeout error\n" );
        status_.setError(SERIAL_TIMEOUT_ERROR);
        return ( 0 );
    }

    bufp_ = &buf_[1];
    bufe_ = buf_ + n;
    return ( buf_[0] );
}

}
