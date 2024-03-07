//
// C++ Implementation: nomadserverconnector
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "../nomad/nomadserverconnector.h"
#include "../nomad/nomadhelper.h"

#include <sys/socket.h>         /* for socket calls */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <cstdio>
#include <string.h>
#include <iostream>
#include <unistd.h>


#define MAX_MESSAGE  100

#define SERIAL_ERROR 11
#define IPC_ERROR    111


/*
 * The following defines allow you to access the State vector in a 
 * more readable way.
 */

#define STATE_SIM_SPEED            0

#define STATE_IR_0                 1
#define STATE_IR_1                 2
#define STATE_IR_2                 3
#define STATE_IR_3                 4
#define STATE_IR_4                 5
#define STATE_IR_5                 6
#define STATE_IR_6                 7
#define STATE_IR_7                 8
#define STATE_IR_8                 9
#define STATE_IR_9                 10
#define STATE_IR_10                11
#define STATE_IR_11                12
#define STATE_IR_12                13
#define STATE_IR_13                14
#define STATE_IR_14                15
#define STATE_IR_15                16

#define STATE_SONAR_0              17
#define STATE_SONAR_1              18
#define STATE_SONAR_2              19
#define STATE_SONAR_3              20
#define STATE_SONAR_4              21
#define STATE_SONAR_5              22
#define STATE_SONAR_6              23
#define STATE_SONAR_7              24
#define STATE_SONAR_8              25
#define STATE_SONAR_9              26
#define STATE_SONAR_10             27
#define STATE_SONAR_11             28
#define STATE_SONAR_12             29
#define STATE_SONAR_13             30
#define STATE_SONAR_14             31
#define STATE_SONAR_15             32
 
#define STATE_BUMPER               33
#define STATE_CONF_X               34
#define STATE_CONF_Y               35
#define STATE_CONF_STEER           36
#define STATE_CONF_TURRET          37
#define STATE_VEL_TRANS            38
#define STATE_VEL_RIGHT            38   /* for scout */
#define STATE_VEL_STEER            39
#define STATE_VEL_LEFT             39   /* for scout */
#define STATE_VEL_TURRET           40
#define STATE_MOTOR_STATUS         41
#define STATE_LASER                42
#define STATE_COMPASS              43
#define STATE_ERROR                44

/*
 * The following defines allow you to access the Smask vector in a 
 * more readable way.
 */

#define SMASK_POS_DATA             0

#define SMASK_IR_1                 1
#define SMASK_IR_2                 2
#define SMASK_IR_3                 3
#define SMASK_IR_4                 4
#define SMASK_IR_5                 5
#define SMASK_IR_6                 6
#define SMASK_IR_7                 7
#define SMASK_IR_8                 8
#define SMASK_IR_9                 9
#define SMASK_IR_10                10
#define SMASK_IR_11                11
#define SMASK_IR_12                12
#define SMASK_IR_13                13
#define SMASK_IR_14                14
#define SMASK_IR_15                15
#define SMASK_IR_16                16

#define SMASK_SONAR_1              17
#define SMASK_SONAR_2              18
#define SMASK_SONAR_3              19
#define SMASK_SONAR_4              20
#define SMASK_SONAR_5              21
#define SMASK_SONAR_6              22
#define SMASK_SONAR_7              23
#define SMASK_SONAR_8              24
#define SMASK_SONAR_9              25
#define SMASK_SONAR_10             26
#define SMASK_SONAR_11             27
#define SMASK_SONAR_12             28
#define SMASK_SONAR_13             29
#define SMASK_SONAR_14             30
#define SMASK_SONAR_15             31
#define SMASK_SONAR_16             32
 
#define SMASK_BUMPER               33
#define SMASK_CONF_X               34
#define SMASK_CONF_Y               35
#define SMASK_CONF_STEER           36
#define SMASK_CONF_TURRET          37
#define SMASK_VEL_TRANS            38
#define SMASK_VEL_STEER            39
#define SMASK_VEL_TURRET           40
#define SMASK_RESERVED             41
#define SMASK_LASER                42
#define SMASK_COMPASS              43

/*
 * These defines are used for specifying the control modes in the
 * robot motion command 'mv'. If MV_IGNORE is specified for an axis
 * the current motion command for it will remain active. Specifying
 * MV_VM or MV_PR will select velocity and position control as in 
 * the vm and pr robot motion commands 
 */

#define MV_IGNORE 0
#define MV_VM     1 /* velocity mode */
#define MV_PR     2 /* position relative mode */
#define MV_LP     3 /* limp mode */
#define MV_AC     4 /* set acceleration for vm, pr, pa modes*/
#define MV_SP     5 /* set velocity for pr, pa modes */
#define MV_PA     6 /* position absolute mode */
#define MV_TQ     7 /* torque mode */
#define MV_MT     8 /* set maximum torque for vm, pr, pa, tq modes */

/*
 * zeroing modes for arm
 */

#define ZR_CHECK         1
#define ZR_ORIENT        2
#define ZR_NO_N_GRIPPER  4

using namespace std;

namespace Nomad{
/* BEGIN Funciones genericas */

int readn(/*register*/ int fd, /*register*/ char *ptr, /*register*/ int nbytes)
{
    int nleft, nread;
  
    for (nleft = nbytes; nleft > 0; ) {
        nread = read(fd, ptr, nleft);
        if (nread <= 0) {
            if (nread < 0 && (errno == EINTR || errno == EAGAIN)) {
                nread = 0;
            } else {
                return(nread);
            }
        }
        nleft -= nread;
        ptr += nread;
    }
    return(nbytes - nleft);
}

int writen(/*register*/ int fd, /*register*/ char *ptr, /*register*/ int nbytes)
{
    int nleft, nwritten;
    for (nleft = nbytes; nleft > 0; ) {
        nwritten = write(fd, ptr, nleft);
        if (nwritten <= 0) {
            if (nwritten < 0 && (errno == EINTR || errno == EAGAIN)) {
                nwritten = 0;
            } else {
                return(nwritten);
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return(nbytes - nleft);
}

/* END Funciones genericas */


ServerConnector::ServerConnector(long robot_id,std::string host,int port)
    :Connector(robot_id),
     host_(host),
     port_(port),
     socket_(0),
     connectedp_(0)
{

}


ServerConnector::~ServerConnector()
{
}


/************************************************
*                                               *
* Robot Interface Functions                     *
*                                               *
************************************************/

bool ServerConnector::create_robot(){

  
    Command the_request ( Command::CREATE_ROBOT ),the_reply;

    the_request.setSize(2);
    the_request.getMessage()[0] = robot_id_;
    the_request.getMessage()[1] = getpid();

    connectedp_ = 1;
    if (ipc_comm(the_request, the_reply)){
        return(status_.process_socket_reply(the_reply)!=0);
    } else {
        status_.setError(IPC_ERROR);  /* indicate IPC_ERROR */
        connectedp_ = 0;
        return(false);
    }

    return true;
}

bool ServerConnector::connect_robot(){
    pid_t process_id;
  
    process_id = getpid();
    Command the_request ( Command::CONNECT_ROBOT ),the_reply;

    the_request.setSize(2);
    the_request.getMessage()[0] = robot_id_;
    the_request.getMessage()[1] = process_id;
  
  
    connectedp_ = 1;

    if (ipc_comm(the_request, the_reply))  {

        /* if there was an error, we must not initialize the sensors,
         * but return an error immediately instead */
        if (status_.process_socket_reply(the_reply)==0)
            return false;
    
        /* initialize clients Smask to match the one on the server side */
        init_sensors();

        return ( true );
    }else{
        printf("failed to connect to server on machine %s via tcp port #%d \n", 
               host_.c_str(), port_);
        connectedp_ = 0;
        status_.setError(IPC_ERROR);  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::disconnect_robot(){
    pid_t process_id;
  
    process_id = getpid();

    Command the_request ( Command::DISCONNECT ),the_reply(Command::DISCONNECT);

    the_request.setSize(2);
    the_request.getMessage()[0] = robot_id_;
    the_request.getMessage()[1] = process_id;

    if (ipc_comm(the_request, the_reply)){
        return(true);
    }else{
        status_.setError(IPC_ERROR);  /* indicate IPC_ERROR */
        connectedp_ = 0;
        return(false);
    }
}

bool ServerConnector::ac ( int t_ac, int s_ac, int r_ac )
{
    Command the_request ( Command::AC ),the_reply;
    the_request.setSize(3);
    the_request.getMessage()[0] = t_ac;
    the_request.getMessage()[1] = s_ac;
    the_request.getMessage()[2] = r_ac;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError(IPC_ERROR);  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::sp ( int t_sp, int s_sp, int r_sp )
{
    Command the_request ( Command::SP),the_reply;
    the_request.setSize ( 3 );
    the_request.getMessage() [0] = t_sp;
    the_request.getMessage() [1] = s_sp;
    the_request.getMessage() [2] = r_sp;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::pr ( int t_pr, int s_pr, int r_pr )
{
    Command the_request ( Command::PR),the_reply;
    the_request.setSize ( 3 );
    the_request.getMessage() [0] = t_pr;
    the_request.getMessage() [1] = s_pr;
    the_request.getMessage() [2] = r_pr;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::pa(int t_pa, int s_pa, int r_pa){
    t_pa = t_pa;
    s_pa = s_pa;
    r_pa = r_pa;

    return false;


/*    
    Command the_request ( PA_MSG ),the_reply;

    the_request.setSize ( 3 );
    the_request.getMessage() [0] = t_pa;
    the_request.getMessage() [1] = s_pa;
    the_request.getMessage() [2] = r_pa;
    
    if (ipc_comm(the_request, the_reply))
    {
        return(status_.process_state_reply ( the_reply ));
    }
    else
    {
        status_.setError ( IPC_ERROR );
        return false;
    }
*/
}

bool ServerConnector::vm ( int t_vm, int s_vm, int r_vm )
{
    Command the_request ( Command::VM),the_reply;
    the_request.setSize ( 3 );
    the_request.getMessage() [0] = t_vm;
    the_request.getMessage() [1] = s_vm;
    the_request.getMessage() [2] = r_vm;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::mv ( int t_mode, int t_mv, int s_mode, int s_mv, int r_mode, int r_mv )
{
    /* check if the modes are correct */
    if ( ( ( t_mode != MV_IGNORE ) && ( t_mode != MV_AC ) && ( t_mode != MV_SP ) &&
            ( t_mode != MV_LP ) && ( t_mode != MV_VM ) && ( t_mode != MV_PR ) ) ||
            ( ( s_mode != MV_IGNORE ) && ( s_mode != MV_AC ) && ( s_mode != MV_SP ) &&
              ( s_mode != MV_LP ) && ( s_mode != MV_VM ) && ( s_mode != MV_PR ) ) ||
            ( ( r_mode != MV_IGNORE ) && ( r_mode != MV_AC ) && ( r_mode != MV_SP ) &&
              ( r_mode != MV_LP ) && ( r_mode != MV_VM ) && ( r_mode != MV_PR ) ) )
        return false;

    /* build the request packet */
    Command the_request ( Command::MV),the_reply;
    the_request.setSize ( 6 );
    the_request.getMessage() [0] = t_mode;
    the_request.getMessage() [1] = t_mv;
    the_request.getMessage() [2] = s_mode;
    the_request.getMessage() [3] = s_mv;
    the_request.getMessage() [4] = r_mode;
    the_request.getMessage() [5] = r_mv;

    /* communicate with robot */
    if ( ipc_comm ( the_request, the_reply ) )
    {
        /* process the reply packet that contains the state info */
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        /* indicate IPC_ERROR */
        status_.setError ( IPC_ERROR );
        return false;
    }
}

bool ServerConnector::ct ( void )
{

    Command the_request ( Command::CT),the_reply;

    the_request.setSize ( 7 );
    the_request.getMessage() [0] = status_.byteSmask ( 0 );
    the_request.getMessage() [1] = status_.byteSmask ( 1 );
    the_request.getMessage() [2] = status_.byteSmask ( 2 );
    the_request.getMessage() [3] = status_.byteSmask ( 3 );
    the_request.getMessage() [4] = status_.byteSmask ( 4 );
    the_request.getMessage() [5] = status_.byteSmask ( 5 );
    the_request.getMessage() [6] = status_.byteSmask ( 6 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
        return true;
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return ( false );
    }
}

bool ServerConnector::gs ( void )
{
    Command the_request ( Command::GS),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::st ( void )
{
    Command the_request ( Command::ST),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::lp ( void )
{
    Command the_request ( Command::LP),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::tk ( char *talk_string )
{

    Command the_request ( Command::TK),the_reply;
    the_request.setSize ( ( strlen ( talk_string ) +4 ) /4 );

    strcpy ( ( char * ) the_request.getMessage(), talk_string );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::dp ( int x, int y )
{
    Command the_request ( Command::DP),the_reply;
    the_request.setSize ( 2 );
    the_request.getMessage() [0] = x;
    the_request.getMessage() [1] = y;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::zr()
{
    Command the_request ( Command::ZR),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::conf_ir ( int history, int order[16] )
{
    int i;

    Command the_request ( Command::CONF_IR),the_reply;
    the_request.setSize ( 17 );
    the_request.getMessage() [0] = history;
    for ( i=0; i<16; i++ )
    {
        the_request.getMessage() [i+1] = order[i];
    }

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::conf_sn ( int rate, int order[16] )
{
    int i;

    Command the_request ( Command::CONF_SN),the_reply;
    the_request.setSize ( 17 );
    the_request.getMessage() [0] = rate;
    for ( i=0; i<16; i++ )
    {
        the_request.getMessage() [i+1] = order[i];
    }

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::conf_cp ( int mode )
{
    Command the_request ( Command::CONF_CP),the_reply;
    the_request.getMessage() [0] = mode;
    the_request.setSize ( 1 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::conf_ls ( unsigned int mode,
                                unsigned int threshold,
                                unsigned int width,
                                unsigned int num_data,
                                unsigned int processing )
{
    Command the_request ( Command::CONF_LS),the_reply;
    the_request.setSize ( 5 );
    the_request.getMessage() [0] = mode;
    the_request.getMessage() [1] = threshold;
    the_request.getMessage() [2] = width;
    the_request.getMessage() [3] = num_data;
    the_request.getMessage() [4] = processing;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::conf_tm ( unsigned char timeout )
{
    Command the_request ( Command::CONF_TM),the_reply;
    the_request.setSize ( 1 );
    the_request.getMessage() [0] = timeout;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::get_ir ( void )
{
    Command the_request ( Command::GET_IR),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_infrared_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::get_sn ( void )
{
    Command the_request ( Command::GET_SN),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_sonar_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::get_rc ( void )
{
    Command the_request ( Command::GET_RC),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_configuration_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::get_rv ( void )
{
    Command the_request ( Command::GET_RV),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_velocity_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::get_ra ( void )
{
    Command the_request ( Command::GET_RA),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_velocity_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::get_cp ( void )
{
    Command the_request ( Command::GET_CP),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_compass_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::get_ls ( void )
{
    int temp_laser_mode = 0;
    bool return_value;

    Command the_request ( Command::GET_LS),the_reply;
    the_request.setSize ( 0 );

    if ( ( status_.getLaserMode() == 33 ) || ( status_.getLaserMode() == 1 ) )
        status_.setLaserMode(51);

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return_value = status_.process_laser_reply ( the_reply );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return_value = false;
    }

    status_.setLaserMode(temp_laser_mode);
    return return_value;
}

bool ServerConnector::get_bp ( void )
{
    Command the_request ( Command::GET_BP),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_bumper_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::conf_sg ( unsigned int threshold,
                                unsigned int min_points,
                                unsigned int gap )
{
    Command the_request ( Command::CONF_SG),the_reply;
    the_request.setSize ( 3 );
    the_request.getMessage() [0] = threshold;
    the_request.getMessage() [1] = min_points;
    the_request.getMessage() [2] = gap;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::get_sg ( void )
{
    int temp_laser_mode;
    bool return_value;

    Command the_request ( Command::GET_SG),the_reply;
    the_request.setSize ( 0 );

    temp_laser_mode = status_.getLaserMode();
    status_.setLaserMode(33);

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return_value =  status_.process_laser_reply ( the_reply ) ;
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return_value = false;
    }

    status_.setLaserMode(temp_laser_mode);
    return return_value;
}

bool ServerConnector::da ( int th, int tu )
{
    Command the_request ( Command::DA),the_reply;
    the_request.setSize ( 2 );
    the_request.getMessage() [0] = th;
    the_request.getMessage() [1] = tu;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::ws ( unsigned char t_ws, unsigned char s_ws,
                           unsigned char r_ws, unsigned char timeout )
{
    Command the_request ( Command::WS),the_reply;
    the_request.setSize ( 4 );
    the_request.getMessage() [0] = t_ws;
    the_request.getMessage() [1] = s_ws;
    the_request.getMessage() [2] = r_ws;
    the_request.getMessage() [3] = timeout;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_state_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::get_rpx ( long *robot_pos )
{
    Command the_request ( Command::GET_RPX),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_rpx_reply ( the_reply, robot_pos ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

/*****************************
*                           *
* World Interface Functions *
*                           *
*****************************/

bool ServerConnector::add_obstacle ( long obs[2*MAX_VERTICES+1] )
{
    int i;

    Command the_request ( Command::ADDOBS),the_reply;
    the_request.setSize ( obs[0]*2+1 );
    for ( i=0; i<obs[0]*2+1; i++ )
    {
        the_request.getMessage() [i] = obs[i];
    }

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::delete_obstacle ( long obs[2*MAX_VERTICES+1] )
{
    int i;

    Command the_request ( Command::DELETEOBS),the_reply;
    the_request.setSize ( obs[0]*2+1 );
    for ( i=0; i<obs[0]*2+1; i++ )
    {
        the_request.getMessage() [i] = obs[i];
    }

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::move_obstacle ( long obs[2*MAX_VERTICES+1], long dx, long dy )
{
    int i;

    Command the_request ( Command::MOVEOBS),the_reply;
    the_request.setSize ( obs[0]*2+3 );
    for ( i=0; i<obs[0]*2+1; i++ )
    {
        the_request.getMessage() [i] = obs[i];
    }
    the_request.getMessage() [2*obs[0]+1] = dx;
    the_request.getMessage() [2*obs[0]+2] = dy;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_obstacle_reply ( the_reply, obs ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::new_world ( void )
{
    Command the_request ( Command::NEWWORLD),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

/****************************
*                          *
* Graphics refresh control *
*                          *
****************************/
bool ServerConnector::refresh_all ( void )
{
    Command the_request ( Command::REFRESHALL),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::refresh_all_traces ( void )
{
    Command the_request ( Command::REFRESHALLTRACES),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::refresh_actual_trace ( void )
{
    Command the_request ( Command::REFRESHACTTRACE),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::refresh_encoder_trace ( void )
{
    Command the_request ( Command::REFRESHENCTRACE),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::refresh_all_sensors ( void )
{
    Command the_request ( Command::REFRESHALLSENSORS),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::refresh_bumper_sensor ( void )
{
    Command the_request ( Command::REFRESHBPSENSOR),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::refresh_infrared_sensor ( void )
{
    Command the_request ( Command::REFRESHIRSENSOR),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::refresh_sonar_sensor ( void )
{
    Command the_request ( Command::REFRESHSNSENSOR),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::refresh_laser_sensor ( void )
{
    Command the_request ( Command::REFRESHLSSENSOR),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::refresh_client_graphics ( void )
{
    Command the_request ( Command::REFRESHGRAPHICS),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}


/*******************************
*                             *
* Miscellaneous robot control *
*                             *
*******************************/

bool ServerConnector::place_robot ( int x, int y, int th, int tu )
{
    Command the_request ( Command::RPLACE),the_reply;
    the_request.setSize ( 4 );
    the_request.getMessage() [0] = x;
    the_request.getMessage() [1] = y;
    the_request.getMessage() [2] = th;
    the_request.getMessage() [3] = tu;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::special_request ( unsigned char *user_send_buffer,
                                        unsigned char *user_receive_buffer )
{

    unsigned short num_data;
    int i;

    Command the_request ( Command::SPECIAL),the_reply;
    num_data = user_send_buffer[0]+256*user_send_buffer[1];
    if ( num_data>USER_BUFFER_LENGTH-5 )
    {
        printf ( "Data + protocol bytes exceeding %d, truncating\n",USER_BUFFER_LENGTH );
        num_data  = USER_BUFFER_LENGTH-5; /* num_data already includes the 4 bytes of user packets protocol */
    }
    the_request.setSize ( num_data );
    for ( i=0; i<num_data; i++ )
    {
        the_request.getMessage() [i] = user_send_buffer[i];
    }
    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_special_reply ( the_reply, user_receive_buffer ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

/*******************************
*                             *
* Graphic Interface Functions *
*                             *
*******************************/

bool ServerConnector::draw_robot ( long x, long y, int th, int tu, int mode )
{
    Command the_request ( Command::DRAWROBOT),the_reply;
    the_request.setSize ( 5 );
    the_request.getMessage() [0] = x;
    the_request.getMessage() [1] = y;
    the_request.getMessage() [2] = th;
    the_request.getMessage() [3] = tu;
    the_request.getMessage() [4] = ( long ) ( mode );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::draw_line ( long x_1, long y_1, long x_2, long y_2, int mode )
{
    Command the_request ( Command::DRAWLINE),the_reply;
    the_request.setSize ( 5 );
    the_request.getMessage() [0] = x_1;
    the_request.getMessage() [1] = y_1;
    the_request.getMessage() [2] = x_2;
    the_request.getMessage() [3] = y_2;
    the_request.getMessage() [4] = ( long ) ( mode );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::draw_arc ( long x_0, long y_0, long w, long h, int th1, int th2, int mode )
{
    Command the_request ( Command::DRAWARC),the_reply;
    the_request.setSize ( 7 );
    the_request.getMessage() [0] = x_0;
    the_request.getMessage() [1] = y_0;
    the_request.getMessage() [2] = w;
    the_request.getMessage() [3] = h;
    the_request.getMessage() [4] = th1;
    the_request.getMessage() [5] = th2;
    the_request.getMessage() [6] = ( long ) mode;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

/*************************************
*                                   *
* Miscellaneous Interface Functions *
*                                   *
*************************************/

bool ServerConnector::server_is_running ( void )
{
    return ( connect_robot() );
}

bool ServerConnector::quit_server ( void )
{
    Command the_request ( Command::QUIT),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::real_robot ( void )
{
    Command the_request ( Command::REALROBOT),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::simulated_robot ( void )
{
    Command the_request ( Command::SIMULATEDROBOT),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_simple_reply ( the_reply ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::predict_sensors ( int x, int y, int th, int tu, long *state, int *laser )
{
    Command the_request ( Command::PREDICTSENSOR),the_reply;
    the_request.setSize ( 4 );
    the_request.getMessage() [0] = x;
    the_request.getMessage() [1] = y;
    the_request.getMessage() [2] = th;
    the_request.getMessage() [3] = tu;

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_predict_reply ( the_reply, state, laser ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::motion_check ( long type, double a1, double a2, double a3, double a4,
                                     double a5, double a6, double a7, double collide[3] )
{
    Command the_request ( Command::MCHECK),the_reply;
    the_request.setSize ( 8 );
    the_request.getMessage() [0] = type;
    the_request.getMessage() [1] = ( long ) ( a1*100.0 );
    the_request.getMessage() [2] = ( long ) ( a2*100.0 );
    if ( type == 0 ) /* line segment */
    {
        the_request.getMessage() [3] = ( long ) ( a3*100.0 );
        the_request.getMessage() [4] = ( long ) ( a4*100.0 );
    }
    else /* arc */
    {
        the_request.getMessage() [3] = ( long ) ( a3*1000000.0 );
        the_request.getMessage() [4] = ( long ) ( a4*1000000.0 );
        the_request.getMessage() [5] = ( long ) ( a5*100.0 );
        the_request.getMessage() [6] = ( long ) ( a6*100.0 );
        the_request.getMessage() [7] = ( long ) ( a7*100.0 );
    }

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_mcheck_reply ( the_reply, collide ) );
    }
    else
    {
        status_.setError ( IPC_ERROR );  /* indicate IPC_ERROR */
        return false;
    }
}

bool ServerConnector::get_robot_conf ( long *conf )
{
    Command the_request ( Command::GET_CONF),the_reply;
    the_request.setSize ( 0 );

    if ( ipc_comm ( the_request, the_reply ) )
    {
        return ( status_.process_conf_reply ( the_reply, conf ) );
    }
    else
    {
        return false;
    }
}


/************************************************************/
/************************************************************/

bool ServerConnector::open_socket()
{
    struct hostent *hp;
    struct sockaddr_in serv_addr;
    char addr[10];
    if(socket_!=0) return false;
  
    if ( ((hp = gethostbyname(host_.c_str())) == NULL) &&
          ((hp = gethostbyaddr(convertAddr(host_.c_str(),addr),4,AF_INET)) == NULL ) )
    {
        printf("host %s not valid\n", host_.c_str()); 
        return(false);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
//    memset ( (char *) &serv_addr, 0, sizeof(serv_addr) );
//    bcopy(hp->h_addr, (char *) &(serv_addr.sin_addr), hp->h_length);
//    memcpy ( (char *) &(serv_addr.sin_addr), hp->h_addr, hp->h_length );
    memcpy ( (char *) &(serv_addr.sin_addr), hp->h_addr_list[0], hp->h_length );
    serv_addr.sin_family = AF_INET;            /* address family */
    serv_addr.sin_port = htons(port_);  /* internet TCP port number */
  
    if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) <= 0){
        printf("ERROR: in open_socket_to_send_data, socket failed.\n");
        socket_ = 0;
        return(false);
    }
  
    if (connect(socket_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        printf("ERROR: in open_socket_to_send_data, connect failed.\n");
        return(false);
    }
  
    return(true);
}

bool ServerConnector::close_socket()
{
    if (socket_==0){
        return(false);
    }
    close(socket_);
    socket_=0;
    return true;
}


bool ServerConnector::ipc_comm(Command & this_request,Command & this_reply)
{
    int read_result;
  
    if (connectedp_!=1){
        printf("Not connected to any robot\n");
        return(false);
    }
  
    if (socket_==0){
        if(!open_socket()){
            return(false);
        }
    }

    write_request_to_socket(this_request);
  
    if (this_reply.getType()!=Command::DISCONNECT){
        read_result = read_reply_from_socket(this_reply);
        return(read_result != -1);
    }else {
        close_socket(); /* this_reply is set to DISCONNECT_MSG to indicate disconnect; close the socket */
        return(true);
    }
  
}


bool ServerConnector::read_reply_from_socket(Command & this_reply)
{
    short rep_size, rep_type;
    long n, mesg_size;
    /* 
        mesg size has to be a long, because bytes are carried as longs, thus the
        maximum size is 0xFFFF*4, which needs a long to be represented;
        this waists lots of bandwidth. I do not change it now, because we will
        move soon to a decent socket communications protocol 
    */
    n = readn(socket_, (char *) &rep_type, sizeof(short));
    n = readn(socket_, (char *) &rep_size, sizeof(short));
    if (n <= 0){
        if (n == 0){
            printf("ERROR: read 0 byte\n");
        } else{
            printf("ERROR: in read_reply_from_socket.\n");
        }
        return  false;
    }
  
    this_reply.setTypeNet(rep_type);
    this_reply.setSizeNet(rep_size);

    mesg_size = this_reply.getSize()*sizeof(long);
    long * rep_buff = new long[this_reply.getSize()];
    n = readn(socket_, (char *) rep_buff, mesg_size);

    if (n < mesg_size) {
        printf("ERROR: in read_request_from_socket, readn 2 didn't.\n");
        delete[] rep_buff;
        return false;
    }else{
        this_reply.setMessageNet(rep_buff,rep_size);
        delete[] rep_buff;
    }
    
    return true;
}


/* 
 * write_request_to_socket - sends the reply to the client who requested
 *                           service
 */
bool ServerConnector::write_request_to_socket(Command & this_request)
{
    long request_size,tmp;
    short          type_net = this_request.getTypeNet();
    unsigned short size_net = this_request.getSizeNet();
    
    this_request.hostToNet();
  
    /* changing to network data format before sending */
    
    request_size = 2*sizeof(short) + this_request.getSize()*sizeof(long);
    
    tmp = writen(socket_, (char *) & type_net, sizeof(short));
    tmp +=writen(socket_, (char *) & size_net, sizeof(short));
    tmp +=writen(socket_, (char *) this_request.getMessageNet(), this_request.getSize()*sizeof(long));
  
    return (request_size==tmp);
}



int ServerConnector::posDataRequest ( int posRequest )
{
  /* check if the argument is okay */
  if ( posRequest & 
      !( POS_INFRARED | POS_SONAR | POS_BUMPER | POS_LASER | POS_COMPASS ) )
    return  false;

  /* The value in Smask[ SMASK_POS_DATA ] is passed through entire system */
  status_.getSmaskVector()[0] = posRequest;
  ct();

  return  true;
}



}
