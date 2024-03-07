//
// C++ Implementation: nomadstatus
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "nomadstatus.h"
#include <string.h>
#include <iostream>
#include <cstdio>
#include "../nomad/nomadhelper.h"

/* 
 * these macros enable the user to determine if the pos-attachment
 * is requested for a specific sensor. 1 is returned if the 
 * attachment is requested, 0 otherwise
 * 
 * Note that the function posDataCheck() is called (see below)
 */

#define POS_INFRARED_P  ( ( (posDataCheck()) & POS_INFRARED ) ? 1 : 0 )
#define POS_SONAR_P     ( ( (posDataCheck()) & POS_SONAR    ) ? 1 : 0 )
#define POS_BUMPER_P    ( ( (posDataCheck()) & POS_BUMPER   ) ? 1 : 0 )
#define POS_LASER_P     ( ( (posDataCheck()) & POS_LASER    ) ? 1 : 0 )
#define POS_COMPASS_P   ( ( (posDataCheck()) & POS_COMPASS  ) ? 1 : 0 )

#define POS_INFRARED_PI(x)  ( ( (x) & POS_INFRARED ) ? 1 : 0 )
#define POS_SONAR_PI(x)     ( ( (x) & POS_SONAR    ) ? 1 : 0 )
#define POS_BUMPER_PI(x)    ( ( (x) & POS_BUMPER   ) ? 1 : 0 )
#define POS_LASER_PI(x)     ( ( (x) & POS_LASER    ) ? 1 : 0 )
#define POS_COMPASS_PI(x)   ( ( (x) & POS_COMPASS  ) ? 1 : 0 )

/*
 * The voltages have different ranges to account for the fact that the
 * CPU measurement is taken after lossage on the slip-ring.
 */

#define RANGE_CPU_VOLTAGE        12.0
//#define RANGE_MOTOR_VOLTAGE      12.85
#define RANGE_MOTOR_VOLTAGE      24.0

/*
 * The user will be able to call a function that fills out a 
 * list of position data for a specific sensor reading. 
 * To access the sensors in that structure the following defines 
 * should be used. They should also be used if data for a single
 * infrared sensor / sonar is requested.
 */

#define POS_IR_1             0
#define POS_IR_2             1
#define POS_IR_3             2
#define POS_IR_4             3
#define POS_IR_5             4
#define POS_IR_6             5
#define POS_IR_7             6 
#define POS_IR_8             7
#define POS_IR_9             8
#define POS_IR_10            9
#define POS_IR_11           10
#define POS_IR_12           11
#define POS_IR_13           12
#define POS_IR_14           13
#define POS_IR_15           14
#define POS_IR_16           15

#define POS_SONAR_1          0
#define POS_SONAR_2          1
#define POS_SONAR_3          2
#define POS_SONAR_4          3
#define POS_SONAR_5          4
#define POS_SONAR_6          5 
#define POS_SONAR_7          6  
#define POS_SONAR_8          7
#define POS_SONAR_9          8
#define POS_SONAR_10         9
#define POS_SONAR_11        10
#define POS_SONAR_12        11
#define POS_SONAR_13        12
#define POS_SONAR_14        13
#define POS_SONAR_15        14
#define POS_SONAR_16        15

/* error types */
#define SERIAL_OPEN_ERROR 1
#define SERIAL_WRITE_ERROR 2
#define SERIAL_READ_ERROR 3
#define SERIAL_PKG_ERROR 4
#define SERIAL_TIMEOUT_ERROR 5


namespace Nomad{

/*********************************************************
 *
 *   Laser Calibration Stuff 
 *
 *********************************************************/

/* Transformation function accordingly to the calibration */
/* xi1 = pixel; yi1 = scanline */
static void ProjectPhy ( double xi1, double yi1, double *x, double *y )
{
    double xi,yi;
    double den;
    double LASER_CALIBRATION[8]   = { -0.003470,  0.000008, 0.011963,  0.001830,
                                     27.5535913,  0.000428, 0.031102, -0.444624 };
    double LASER_OFFSET[2]        = { 0, 0 };

    xi = xi1 - 254.5;
    yi = yi1 - 240.5;

    den = ( LASER_CALIBRATION[0]*xi + LASER_CALIBRATION[1]*yi + 1 );

    *x = ( LASER_CALIBRATION[2]*xi + LASER_CALIBRATION[3]*yi +
           LASER_CALIBRATION[4] ) /den + LASER_OFFSET[0];
    *y = ( LASER_CALIBRATION[5]*xi + LASER_CALIBRATION[6]*yi +
           LASER_CALIBRATION[7] ) /den + LASER_OFFSET[1];
}

static void convert_laser ( int *laser )
{
    int i, num_points, offset, interval;
    double line_num;
    double laserx[483], lasery[483];

    num_points = laser[0];
    interval = NUM_LASER/ ( num_points-1 );
    offset = 3 + ( NUM_LASER- ( num_points * interval ) ) /2;
    for ( i=1; i<=num_points; i++ )
    {
        line_num = ( double ) ( offset+ ( i-1 ) *interval );
        ProjectPhy ( ( double ) laser[i], line_num, &laserx[i], &lasery[i] );
    }
    for ( i=1; i<=num_points; i++ )
    {
        laser[2*i-1] = ( int ) ( laserx[i]*10.0 );
        laser[2*i] = ( int ) ( lasery[i]*10.0 );
    }
    return;
}

Status::Status(){

    for (int i=0; i<NUM_STATE; i++)
        state_[i] = 0;

    for (int i=0;i< (2*NUM_LASER+1);i++)
        laser_[i] = 0;

    init_mask();
    
    voltageCPU_ = 0;
    voltageMotor_ = 0;
    laser_mode_ = 51;
    own_tcp_port_ =0;
}
Status::~Status(){
}



long Status::getSimulationSpeed(){
    return state_[STATE_SIM_SPEED];
}



long Status::getInfraredData(int sensor){
    if (sensor<0 || sensor>15)
        return -1;
    return state_[STATE_IR_0+sensor];
}


long Status::getSonarData(int sensor){
    if (sensor<0 || sensor>15)
        return -1;
    return state_[STATE_SONAR_0+sensor];
}

long Status::getBumperData(){
    return state_[STATE_BUMPER];
}

bool Status::getBumperData(int sensor){
    if (sensor<0 || sensor>19)
        return -1;

    long mask = (0x1 << sensor);

    return ((state_[STATE_BUMPER] & mask)!=0);
}

long Status::getX(){
    return state_[STATE_CONF_X];
}
long Status::getY(){
    return state_[STATE_CONF_Y];
}
long Status::getSteeringOrient(){
    return state_[STATE_CONF_STEER];
}
long Status::getTurretOriente(){
    return state_[STATE_CONF_TURRET];
}
long Status::getTransVel(){
    return state_[STATE_VEL_TRANS];
}
long Status::getSteeringVel(){
    return state_[STATE_VEL_STEER];
}
long Status::getTurretVel(){
    return state_[STATE_VEL_TURRET];
}
bool Status::isMotorTransActive(){
    return ((state_[STATE_MOTOR_STATUS] & (0x1))!=0);
}
bool Status::isMotorSteeringActive(){
    return ((state_[STATE_MOTOR_STATUS] & (0x2))!=0);
}
bool Status::isMotorTurretActive(){
    return ((state_[STATE_MOTOR_STATUS] & (0x4))!=0);
}
long Status::getStatusMotor(){
    return state_[STATE_MOTOR_STATUS];
}
long Status::getLaserStatus(){
    return state_[STATE_LASER];
}
long Status::getCompasData(){
    return state_[STATE_COMPASS];
}
long Status::getError(){
    return state_[STATE_ERROR];
}
void Status::setError(long error){
    state_[STATE_ERROR] = error;
}

unsigned char Status::byteSmask(int bite){
    switch (bite){
        case 0:
            return bits_to_byte (smask_[1], smask_[2], smask_[3], smask_[4],
                                smask_[5], smask_[6], smask_[7], smask_[8]);
        case 1:
            return bits_to_byte (smask_[9], smask_[10], smask_[11], smask_[12],
                                smask_[13], smask_[14], smask_[15], smask_[16]);
        case 2:
            return bits_to_byte (smask_[17], smask_[18], smask_[19], smask_[20],
                                smask_[21], smask_[22], smask_[23], smask_[24]);
        case 3:
            return bits_to_byte (smask_[25], smask_[26], smask_[27], smask_[28],
                                smask_[29], smask_[30], smask_[31], smask_[32]);
        case 4:
            return bits_to_byte (smask_[33], smask_[34], smask_[35], smask_[36],
                            smask_[37], smask_[38], smask_[39], smask_[40]);
        case 5:
            return bits_to_byte (smask_[0], smask_[41], smask_[42], smask_[43], 
                                0,0,0,0);
        case 6:
            return (unsigned char) smask_[0];
    }
    
    return 0;
}

void Status::init_mask(void)
{
    int i;
  
    smask_[ SMASK_POS_DATA ] = 0;
    for (i=1; i<NUM_MASK; i++)
        smask_[i] = 1;
}

/* 
 * process_???_reply - processes reply from the server to extract the
 *                     appropriate information 
 */
bool Status::process_state_reply(Command & this_reply)
{
    int i,j, num_points;
    
    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;;
    }
    else
    {
        /* the state vector */
    
        for ( i=0; i<=44; i++ )
            state_[i] = this_reply.getMessage() [i];
        num_points = this_reply.getMessage() [45];
    
        /* the laser data */
    
        if ( ( laser_mode_ == 1 ) || ( laser_mode_ == 33 ) )
        {
            for ( i=45; i<=45+4*num_points; i++ )
                laser_[i-45] = this_reply.getMessage() [i];
        }
        else
        {
            if ( ( laser_mode_ == 51 ) || ( laser_mode_ == 50 ) || ( laser_mode_ == 19 ) )
                for ( i=45; i<=45+2*num_points; i++ )
                    laser_[i-45] = this_reply.getMessage() [i];
            else
                for ( i=45; i<=45+num_points; i++ )
                    laser_[i-45] = this_reply.getMessage() [i];
        }
        state_[ STATE_ERROR ] = 0;
    
        /*
         * depending on what PosData attachments were required,
         * the data have to be extracted from the buffer.
         * check for each of the sensors consecutively.
         */
    
        /* infrared */
        if ( POS_INFRARED_P )
            for ( j = 0; j < INFRAREDS; j++ )
                i = posDataProcess ( this_reply.getMessage(), i, & ( posDataAll_.infrared[j] ) );
    
        /* sonar */
        if ( POS_SONAR_P )
            for ( j = 0; j < SONARS; j++ )
                i = posDataProcess ( this_reply.getMessage(), i, & ( posDataAll_.sonar[j] ) );
    
        /* bumper */
        if ( POS_BUMPER_P )
            i = posDataProcess ( this_reply.getMessage(), i, & ( posDataAll_.bumper ) );
    
        /* laser */
        if ( POS_LASER_P )
            i = posDataProcess ( this_reply.getMessage(), i, & ( posDataAll_.laser ) );
    
        /* compass */
        if ( POS_COMPASS_P )
            i = posDataProcess ( this_reply.getMessage(), i, & ( posDataAll_.compass ) );
    
        /* the Intellisys 100 time */
        i = timeDataProcess ( this_reply.getMessage(), i, &posDataTime_ );
    
        /* the voltages for CPU and motors */
        i = voltDataProcess ( this_reply.getMessage(), i, &voltageCPU_, &voltageMotor_ );
    
    }
    return true;
}

bool Status::process_infrared_reply ( Command &this_reply )
{
    int i,j;

    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        for ( i= STATE_IR_0 ; i <= STATE_IR_15 ; i++ )
            state_[i] = this_reply.getMessage() [i-STATE_IR_0];

        /*
         * if position attachment was requested for infrared....
         */

        i -= STATE_IR_0;
        if ( POS_INFRARED_P )
            for ( j = 0; j < INFRAREDS; j++ )
                i = posDataProcess ( this_reply.getMessage(), i, & ( posDataAll_.infrared[j] ) );
    }

    /* the Intellisys 100 time */
    i = timeDataProcess ( this_reply.getMessage(), i, &posDataTime_);

    return true;
}

bool Status::process_sonar_reply ( Command &this_reply )
{
    int i,j;

    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        for ( i = STATE_SONAR_0 ; i <= STATE_SONAR_15; i++ )
            state_[i] = this_reply.getMessage() [i - STATE_SONAR_0];

        /*
         * if position attachment was requested for sonar....
         */

        i -= STATE_SONAR_0;
        if ( POS_SONAR_P )
            for ( j = 0; j < SONARS; j++ )
                i = posDataProcess ( this_reply.getMessage(), i, & ( posDataAll_.sonar[j] ) );
    }

    /* the Intellisys 100 time */
    i = timeDataProcess ( this_reply.getMessage(), i, &posDataTime_);

    return true;
}

bool Status::process_configuration_reply ( Command &this_reply )
{
    int i;

    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        for ( i = STATE_CONF_X; i <= STATE_CONF_TURRET; i++ )
            state_[i] = this_reply.getMessage() [i - STATE_CONF_X];
    }
    return true;
}

bool Status::process_conf_reply ( Command &this_reply, long *conf )
{
    int i;

    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        for ( i= 0; i<4; i++ )
            conf[i] = this_reply.getMessage() [i];
    }
    return true;
}

bool Status::process_velocity_reply ( Command &this_reply )
{
    int i;

    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        for ( i = STATE_VEL_TRANS ; i <= STATE_VEL_TURRET; i++ )
            state_[i] = this_reply.getMessage() [i - STATE_VEL_TRANS];
    }
    return true;
}


bool Status::process_compass_reply ( Command &this_reply )
{
    int i;

    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        state_[ STATE_COMPASS ] = this_reply.getMessage() [0];

        /*
         * if position attachment was requested for compass....
         */

        if ( POS_COMPASS_P )
            i = posDataProcess ( this_reply.getMessage(), 1, & ( posDataAll_.compass ) );
    }

    /* the Intellisys 100 time */
    i = timeDataProcess ( this_reply.getMessage(), i, &posDataTime_);

    return true;
}

bool Status::process_bumper_reply ( Command &this_reply )
{
    int i;

    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        state_[ STATE_BUMPER ] = this_reply.getMessage() [0];

        /*
         * if position attachment was requested for bumper....
         */

        if ( POS_BUMPER_P )
            i = posDataProcess ( this_reply.getMessage(), 1, & ( posDataAll_.bumper ) );
    }

    /* the Intellisys 100 time */
    i = timeDataProcess ( this_reply.getMessage(), i, &posDataTime_);

    return true;
}

bool Status::process_laser_reply ( Command &this_reply )
{
    int i, num_points;

    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        num_points = this_reply.getMessage() [0];

        if ( ( laser_mode_ == 1 ) || ( laser_mode_ == 33 ) )
        {
            for ( i=0; i<=4*num_points; i++ )
                laser_[i] = this_reply.getMessage() [i];
        }
        else
        {
            if ( ( laser_mode_ == 51 ) || ( laser_mode_ == 50 ) || ( laser_mode_ == 19 ) )
                for ( i=0; i<=2*num_points; i++ )
                    laser_[i] = this_reply.getMessage() [i];
            else
                for ( i=0; i<=num_points; i++ )
                    laser_[i] = this_reply.getMessage() [i];
        }

        /*
         * if position attachment was requested for laser....
         */

        if ( POS_LASER_P )
            i = posDataProcess ( this_reply.getMessage(), i, & ( posDataAll_.laser ) );
    }

    /* the Intellisys 100 time */
    i = timeDataProcess ( this_reply.getMessage(), i, &posDataTime_);

    return true;
}

bool Status::process_predict_reply ( Command &this_reply, long *state, int *laser )
{
    int i, num_points;

    for ( i=1; i<33; i++ )
        state[i] = this_reply.getMessage() [i];
    for ( i=33; i<=44; i++ )
        state[i] = 0;
    num_points = this_reply.getMessage() [44];
    if ( ( laser_mode_ == 1 ) || ( laser_mode_ == 33 ) )
    {
        for ( i=44; i<=44+4*num_points; i++ )
            laser[i-44] = this_reply.getMessage() [i];
    }
    else
    {
        if ( ( laser_mode_ == 51 ) || ( laser_mode_ == 50 ) || ( laser_mode_ == 19 ) )
            for ( i=44; i<=44+2*num_points; i++ )
                laser[i-44] = this_reply.getMessage() [i];
        else
            for ( i=44; i<=44+num_points; i++ )
                laser[i-44] = this_reply.getMessage() [i];
    }
    return true;
}

bool Status::process_simple_reply ( Command &this_reply )
{
    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        return true;
    }
}

bool Status::process_obstacle_reply ( Command &this_reply, long *obs )
{
    int i;

    for ( i=0; i<this_reply.getSize(); i++ )
    {
        obs[i] = this_reply.getMessage() [i];
    }
    return true;
}

bool Status::process_rpx_reply ( Command &this_reply, long *robot_pos )
{
    int i, num_robots;

    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        num_robots = this_reply.getMessage() [0];
        robot_pos[0] = num_robots;

        for ( i=1; i<=3*num_robots; i++ )
        {
            robot_pos[i] = this_reply.getMessage() [i];
        }
    }
    return true;
}


int Status::process_socket_reply ( Command &this_reply )
{
    own_tcp_port_ = this_reply.getMessage() [0];

    return ( own_tcp_port_ );
}

/* this is something special for Greg at Yale */
bool Status::process_mcheck_reply ( Command &this_reply,
                                  double collide[3] )
{
    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
    }
    else
    {
        if ( this_reply.getMessage() [0] )
        {
            collide[0] = ( double ) this_reply.getMessage() [1]/100.0;
            collide[1] = ( double ) this_reply.getMessage() [2]/100.0;
            collide[2] = ( double ) this_reply.getMessage() [3]/1000000.0;
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}
bool Status::process_special_reply ( Command &this_reply, unsigned char *data )
{
    int i;

    if ( this_reply.getType() == Command::ERROR )
    {
        state_[ STATE_ERROR ] = this_reply.getMessage() [0];
        return false;
    }
    else
    {
        for ( i=0; i<this_reply.getSize(); i++ )
        {
            data[i] = ( unsigned char ) this_reply.getMessage() [i];
        }
    }
    return true;
}

/*
 *
 * 
 *         PosData Attachment
 *         ===================
 *    
 *    Here all procudures are defined that deal with the 
 * 
 *    attachment of PosData to sensory readings.
 * 
 *
 */



/***************
 * FUNCTION:     posDataCheck
 * PURPOSE:      return the sensors for which the PosData attachment
 *               is currently requested. 
 * ARGUMENTS:    None
 * ALGORITHM:    returns the mask that is not globally accessibe and 
 *               that is set by ct() to be the value of Smask[0]
 * RETURN:       int, see posDataRequest
 *               the macros POS_*_P can be used to examine the value
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
int Status::posDataCheck ( void )
{
  return ( smask_[ SMASK_POS_DATA ] );
}

/***************
 * FUNCTION:     posInfraredRingGet
 * PURPOSE:      copy the PosData for all infrareds to accessible memory
 * ARGUMENTS:    PosData posData [INFRAREDS] :
 *               an array of PosData structures that is filled with 
 *               PosData. The position information for each infrared
 *               containts the configuration of the robot at the time 
 *               of the sensory reading and a timestamp for the 
 *               configuration and the senosry reading .
 * ALGORITHM:    copies blocks of memory
 * RETURN:       int, return always TRUE
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
int Status::posInfraredRingGet ( PosData posData[INFRAREDS] )
{
  /* copy the whole thing in one block */
  memcpy ( posData, posDataAll_.infrared, INFRAREDS * sizeof ( PosData ) );

  return  true;
}


/***************
 * FUNCTION:     posInfraredGet
 * PURPOSE:      copy the PosData for a specific infrared to accessible 
 *               memory
 * ARGUMENTS:    int infraredNumber : the number of the infrared
 *               PosData *posData : the memory location that the information
 *                                  will be copied to 
 * ALGORITHM:    copies block of memory
 * RETURN:       int, always returns TRUE
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
int Status::posInfraredGet     ( PosData *posData , int infraredNumber)
{
  /* copy the whole thing in one block */
  memcpy ( posData, &posDataAll_.infrared[infraredNumber], sizeof ( PosData ) );

  return  true;
}

/***************
 * FUNCTION:     posSonarRingGet
 * PURPOSE:      copy the PosData for all sonars to accessible memory
 * ARGUMENTS:    PosData posData [SONARS] :
 *               an array of PosData structures that is filled with 
 *               PosData. The position information for each sonar
 *               containts the configuration of the robot at the time 
 *               of the sensory reading and a timestamp for the 
 *               configuration and the senosry reading .
 * ALGORITHM:    copies blocks of memory
 * RETURN:       int, return always TRUE
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
int Status::posSonarRingGet    ( PosData posData[SONARS] )
{
  /* copy the whole thing in one block */
  memcpy ( posData, posDataAll_.sonar, SONARS * sizeof ( PosData ) );

  return  true;
}

/***************
 * FUNCTION:     posSonarGet
 * PURPOSE:      copy the PosData for a specific sonar to accessible memory
 * ARGUMENTS:    int infraredNumber : the number of the sonar
 *               PosData *posData : the memory location that the information
 *                                  will be copied to 
 * ALGORITHM:    copies block of memory
 * RETURN:       int, always returns TRUE
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
int Status::posSonarGet        ( PosData *posData , int sonarNumber)
{
  /* copy the whole thing in one block */
  memcpy ( posData, &posDataAll_.sonar[sonarNumber], sizeof ( PosData ) );

  return  true;
}

/***************
 * FUNCTION:     posBumperGet
 * PURPOSE:      copy PosData for the bumper to accessible memory
 * ARGUMENTS:    PosData *posData : where the data is copied to 
 * ALGORITHM:    copies a block of memory
 * RETURN:       int, always returns TRUE
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 * NOTE:         The bumper differs from other sensors in that the 
 *               posData is only updated after one of the bumper sensors 
 *               change its value from zero to one. This means that the 
 *               posData for the bumper always contains the position and 
 *               timeStamps of the latest hit, or undefined information 
 *               if the bumper was not hit yet.
 ***************/
int Status::posBumperGet       ( PosData *posData )
{
  /* copy the whole thing in one block */
  memcpy ( posData, &posDataAll_.bumper, sizeof ( PosData ) );

  return  true;
}

/***************
 * FUNCTION:     posLaserGet
 * PURPOSE:      copy PosData for the laser to accessible memory
 * ARGUMENTS:    PosData *posData : where the data is copied to 
 * ALGORITHM:    copies a block of memory
 * RETURN:       int, always returns TRUE
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 * NOTE:         The laser is updated at a frequency of 30Hz.
 ***************/
int Status::posLaserGet        ( PosData *posData )
{
  /* copy the whole thing in one block */
  memcpy ( posData, &posDataAll_.laser, sizeof ( PosData ) );

  return  true;
}

/***************
 * FUNCTION:     posCompassGet
 * PURPOSE:      copy PosData for the compass to accessible memory
 * ARGUMENTS:    PosData *posData : where the data is copied to 
 * ALGORITHM:    copies a block of memory
 * RETURN:       int, always returns TRUE
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 * NOTE:         The compass is updated ad a frequency of 10Hz.
 ***************/
int Status::posCompassGet      ( PosData *posData )
{
  /* copy the whole thing in one block */
  memcpy ( posData, &posDataAll_.compass, sizeof ( PosData ) );

  return  true;
}

/***************
 * FUNCTION:     posTimeGet
 * PURPOSE:      get the PosData time (Intellisys 100) in milliseconds
 * ARGUMENTS:    None
 * ALGORITHM:    ---
 * RETURN:       int 
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 * NOTE:         Use POS_TICKS_TO_MS and POS_MS_TO_TICKS to convert
 *               between ticks and milliseconds. Overflow after 49 days.
 ***************/
int Status::posTimeGet         ( void )
{
  return ( (int) posDataTime_ );
}

/***************
 * FUNCTION:     voltCpuGet
 * PURPOSE:      get the voltage of the power supply for the CPU
 * ARGUMENTS:    None
 * ALGORITHM:    ---
 * RETURN:       float (the voltage in volt)
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
float Status::voltCpuGet         ( void )
{
  return ( voltConvert ( voltageCPU_ , RANGE_CPU_VOLTAGE ) );
}

/***************
 * FUNCTION:     voltMotorGet
 * PURPOSE:      get the voltage of the power supply for the motors
 * ARGUMENTS:    None
 * ALGORITHM:    ---
 * RETURN:       float (the voltage in volt)
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
float Status::voltMotorGet         ( void )
{
  return ( voltConvert ( voltageMotor_ , RANGE_MOTOR_VOLTAGE ) );
}

/***************
 * FUNCTION:     voltConvert
 * PURPOSE:      convert from the DA reading to the right voltage range
 * ARGUMENTS:    unsigned char reading: the reading of the da
 * ALGORITHM:    ---
 * RETURN:       float (the voltage in volt)
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
float Status::voltConvert ( unsigned char reading , float range )
{
  /* 
   * original reading is [0...255] and represents [2...5]volt.
   * the 5 volt value is converted to 12V by multiplying (range/5)
   */
  return ( ( 2.0 +  ( ( (float) (reading*3) ) / 255.0 ) ) * ( range / 5.0 ) );
}

/***************
 * FUNCTION:     posDataProcess
 * PURPOSE:      copy the PosData from the socket buffer to static memory
 * ARGUMENTS:    long *buffer : pointer to the buffer
 *               int current : where are we currently within the buffer
 *               PosData *posData : where should the data be written
 * ALGORITHM:    copies longs
 * RETURN:       static int, returns the next position in the buffer
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
int Status::posDataProcess (long *buffer, int current, PosData *posData)
{
  int count;

  count = current; 

  /* copy the configuration */
  posData->config.configX       = buffer[count++];
  posData->config.configY       = buffer[count++];
  posData->config.configSteer   = buffer[count++];
  posData->config.configTurret  = buffer[count++];
  posData->config.velTrans      = buffer[count++];
  posData->config.velSteer      = buffer[count++];
  posData->config.velTurret     = buffer[count++];
  posData->config.timeStamp     = (TimeData) buffer[count++];
  
  /* copy the timeStamp of the sensory reading */
  posData->timeStamp            = (TimeData) buffer[count++];

  return ( count );
}




/***************
 * FUNCTION:     timeDataProcess
 * PURPOSE:      copy the Intellisys 100 time from the socket buffer to 
 *               static memory
 * ARGUMENTS:    long *buffer : pointer to the buffer
 *               int current : where are we currently within the buffer
 *               posTimeData *time : where the data is written
 * ALGORITHM:    ---
 * RETURN:       static int, returns the next position in the buffer
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
int Status::timeDataProcess ( long *buffer, int current, TimeData *theTime )
{
  *theTime = (unsigned long) buffer[current];

  return ( current + 1 );
}

/***************
 * FUNCTION:     voltDataProcess
 * PURPOSE:      copy the voltages from the socket buffer to static memory
 * ARGUMENTS:    long *buffer : pointer to the buffer
 *               int current : where are we currently within the buffer
 *               unsigned char *voltCPU : the memory for the CPU voltage
 *               unsigned char *voltMotor : the memory for the motor voltage
 * ALGORITHM:    ---
 * RETURN:       static int, returns the next position in the buffer
 * SIDE EFFECT:  
 * CALLS:        
 * CALLED BY:    
 ***************/
int Status::voltDataProcess (long *buffer, int current, 
                unsigned char *voltCPU, unsigned char *voltMotor)
{
  int counter = current;

  *voltCPU   = (unsigned char) buffer[counter++];
  *voltMotor = (unsigned char) buffer[counter++];

  return (counter);
}











/*********************************************************
 *
 *   Processing different types of packages received from the robot 
 *
 *********************************************************/

bool Status::Process_Robot_Resp(Packet & packet)
{
    switch (packet.getType()) { /* type of the returned package */
    case Packet::AC:
    case Packet::SP:
    case Packet::PR:
    case Packet::PA:
    case Packet::VM:
    case Packet::MV:
    case Packet::CT:
    case Packet::GS:
    case Packet::ST:
    case Packet::LP:
    case Packet::DP:
    case Packet::DA:
    case Packet::WS:
    case Packet::ZR:
    case Packet::TK:
    case Packet::CONF_IR:
    case Packet::CONF_SN:
    case Packet::CONF_LS:
    case Packet::CONF_TM:
    case Packet::CONF_SG:
    case Packet::CONF_SER:
    case Packet::SETUP_LS:
        Process_State_Pkg(packet);
        break;
    case Packet::CONF_CP:
        Process_Compass_Conf_Pkg(packet);
        break;
    case Packet::NAK: /* Nak */
        std::cout << "Nak" << std::endl;
        break;
    case Packet::GET_IR: /* Infrared */
        Process_Infrared_Pkg(packet);
        break;
    case Packet::GET_SN: /* Sonar */
        Process_Sonar_Pkg(packet);
        break;
    case Packet::GET_RC: /* Configuration */
        Process_Configuration_Pkg(packet);
        break;
    case Packet::GET_RV: /* Velocity */
        Process_Velocity_Pkg(packet);
        break;
    case Packet::GET_RA: /* Acceleration */
        Process_Acceleration_Pkg(packet);
        break;
    case Packet::GET_CP: /* Compass */
        Process_Compass_Pkg(packet);
        break;
    case Packet::GET_LS: /* Laser */
        Process_Laser_Point_Pkg(packet);
        break;
    case Packet::GET_BP: /* Bumper */
        Process_Bumper_Pkg(packet);
        break;
    case Packet::GET_SG: /* Laser line mode */
        Process_Laser_Line_Pkg(packet);
        break;
    case Packet::SPECIAL: /* User */
        Process_Special_Pkg(packet);
        break;
    default:
        std::cout << "Invalid Robot Response" << std::endl;
        return false;
        break;
    }
    return true;
}

/* process the response received from the robot which
   encodes the state of the robot according to the mask */
void Status::Process_State_Pkg(Packet & packet){

    int i;
    bool low_half_used = false;
    unsigned char tmp_char = 0;
  
    /* infrared */
    for (i = STATE_IR_0 ; i <= STATE_IR_15; i++)
        if (smask_[i] > 0) 
        {
            if (!low_half_used)
            {
                tmp_char = packet.get();
                state_[i] = low_half(tmp_char);
                low_half_used = true;
            }
            else
            {
                state_[i] = high_half(tmp_char);
                low_half_used = false;
            }
        }
  
    /*
    * if the pos attachment was required we read it
    */
    
    if (POS_INFRARED_PI(smask_[SMASK_POS_DATA]))
        for (i = 0; i < INFRAREDS; i++)
            if (smask_[SMASK_IR_1 + i] > 0) 
                posPackageProcess(packet, &(posDataAll_.infrared[i]));

  
    /* sonars */
    for (i = STATE_SONAR_0; i <= STATE_SONAR_15; i++) {
        if ( smask_[i] > 0 ) {
            state_[i] = packet.get();
        }
    }
    /*
    * if the pos attachment was required we read it
    */
    if (POS_SONAR_PI(smask_[SMASK_POS_DATA]))
        for (i = 0; i < SONARS; i++) 
            if (smask_[SMASK_SONAR_1 + i] > 0)
                posPackageProcess(packet, &(posDataAll_.sonar[i]));

    if (smask_[ SMASK_BUMPER ] > 0){
        unsigned char b1,b2,b3;
        b1 = packet.get();
        b2 = packet.get();
        b3 = packet.get();
        state_[ STATE_BUMPER ] = combine_bumper_vector(b3,b2,b1);
    }
    /*
     * if the position attachment was requested for the bumper
     * we have to unpack the package. 
     */
    if (POS_BUMPER_PI(smask_[SMASK_POS_DATA]))
         posPackageProcess( packet, &(posDataAll_.bumper));
  
    /* the position data */
    if (smask_[SMASK_CONF_X] > 0)
        state_[STATE_CONF_X] =  packet.getSignedInt();
    if (smask_[SMASK_CONF_Y] > 0)
        state_[STATE_CONF_Y] =  packet.getSignedInt();
    if (smask_[SMASK_CONF_STEER] > 0)
        state_[STATE_CONF_STEER] =  packet.getSignedInt();
    if (smask_[SMASK_CONF_TURRET] > 0)
        state_[STATE_CONF_TURRET] =  packet.getSignedInt();
  
    /* the velocities */
    if (smask_[SMASK_VEL_TRANS] > 0)
        state_[STATE_VEL_TRANS] =  packet.getSignedInt();
    if (smask_[SMASK_VEL_STEER] > 0)
        state_[STATE_VEL_STEER] =  packet.getSignedInt();
    if (smask_[SMASK_VEL_TURRET] > 0)
        state_[STATE_VEL_TURRET] =  packet.getSignedInt();
 
    /* the compass value */
    if (smask_[SMASK_COMPASS] > 0){
        state_[STATE_COMPASS] = packet.getSignedInt();
      
        /*
        * if the position attachment was requested for the compass
        * we have to unpack the package. 
        */
    
        if (POS_COMPASS_PI(smask_[SMASK_POS_DATA]))
            posPackageProcess(packet, &(posDataAll_.compass));
    }

    /* laser */
    if (smask_[SMASK_LASER] > 0){
        /* the number of points */
        laser_[0] = packet.getUnsignedInt();
    
        /* check the laser mode */
        if ((laser_mode_&0x1e) == 0) /* Line mode */
        {
            if (laser_[0] > NUM_LASER/2){
                printf("error in processing laser reply (1).\n");
                setError(SERIAL_READ_ERROR);
                laser_[0] = 0;
                return;
            }
            for (i=1; i<=4*laser_[0]; i++){
                laser_[i] = packet.getUnsignedInt();
            }
        } else { /* Points of some kind */
            if (laser_[0] > NUM_LASER){
                printf("error in processing laser reply (2).\n");
                setError(SERIAL_READ_ERROR);
                laser_[0] = 0;
                return;
            }
            for (i=1; i<=laser_[0]; i++) 
            {
                laser_[i] = packet.getUnsignedInt();
            }
        }
        if ((laser_mode_&0x1e) == 19)
            convert_laser(laser_);
    
        /*
        * if the position attachment was requested for the laser
        * we have to get it from somewhere else 
        */
    
        if (POS_LASER_PI(smask_[SMASK_POS_DATA]))
            posPackageProcess(packet, &(posDataAll_.laser));
    }
    /* motor active */
    state_[STATE_MOTOR_STATUS] = (long) packet.get();  

    /* process the 6811 time */
    timePackageProcess(packet, &posDataTime_);

    /* process the voltages of motor/CPU */
    voltPackageProcess(packet, &voltageCPU_, &voltageMotor_);
}

/* process the response from the robot which encodes the
   compass reading of the robot */
void Status::Process_Compass_Conf_Pkg(Packet & packet){
    printf("compass calibration score x: %d y: %d z: %d\n",
            packet.get(), packet.get(),packet.get());
}


void Status::Process_Infrared_Pkg(Packet & packet){
    int i;
    /* 
    * the ir datum from one sensor is only a nibble, 
    * two of them are merged into one byte 
    */
    bool low_half_used = false;
    unsigned char tmp_char = 0;
    /* infrared */
    for (i = STATE_IR_0 ; i <= STATE_IR_15; i++)
        if (!low_half_used){
            tmp_char = packet.get();
            state_[i] = low_half(tmp_char);
            low_half_used = true;
        } else {
            state_[i] = high_half(tmp_char);
            low_half_used = false;
        }
  
    /*
    * if the pos attachment was required we read it
    */
    
    if (POS_INFRARED_PI(smask_[SMASK_POS_DATA]))
        for (i = 0; i < INFRAREDS; i++)
            posPackageProcess(packet, &(posDataAll_.infrared[i]));

    /* extract the time data for the 6811 */
    timePackageProcess(packet, &posDataTime_);
}

/* process the response from the robot which encodes the
   active sonar reading */

void Status::Process_Sonar_Pkg(Packet & packet){
  int i;

    /*
    * read the sensory data from the buffer
    */

    for (i = STATE_SONAR_0; i <= STATE_SONAR_15; i++) {
        state_[i] = packet.get();
    }

    /*
    * if the pos attachment was required we read it
    */
    if ( POS_SONAR_PI ( smask_[ SMASK_POS_DATA ]) )
        for (i=0; i<16; i++) 
            posPackageProcess (packet, &( posDataAll_.sonar[i] ) );
    
    /* extract the time data for the 6811 */
    timePackageProcess ( packet, &posDataTime_ );
}

/* process the response from the robot which encodes the
   configuration of the robot */
void Status::Process_Configuration_Pkg(Packet & packet){
    state_[STATE_CONF_X] =  packet.getSignedInt();
    state_[STATE_CONF_Y] =  packet.getSignedInt();
    state_[STATE_CONF_STEER] =  packet.getSignedInt();
    state_[STATE_CONF_TURRET] =  packet.getSignedInt();
}


void Status::Process_Velocity_Pkg(Packet & packet){
    state_[STATE_VEL_TRANS] =  packet.getSignedInt();
    state_[STATE_VEL_STEER] =  packet.getSignedInt();
    state_[STATE_VEL_TURRET] =  packet.getSignedInt();
}


void Status::Process_Acceleration_Pkg(Packet & packet){
    packet.get();
// static void Process_Acceleration_Pkg(unsigned char inbuf[BUFSIZE])
// {
// }

}

/* process the response from the robot which encodes the
   compass reading of the robot */
void Status::Process_Compass_Pkg(Packet & packet){

    state_[ STATE_COMPASS ] = packet.getUnsignedInt();

    /*
    * if the position attachment was requested for the compass
    * we have to unpack the package. 
    */
    if ( POS_COMPASS_PI ( smask_[ SMASK_POS_DATA ] ) )
        posPackageProcess ( packet, &( posDataAll_.compass ) );

    /* extract the time data for the 6811 */
    timePackageProcess (packet, &posDataTime_ );
}


void Status::Process_Laser_Point_Pkg(Packet & packet){
    int i;
    
    laser_[0]=packet.getUnsignedInt();

    for (i=1; i<=laser_[0]; i++) {
        laser_[i] = packet.getUnsignedInt();
    }
    convert_laser(laser_);
    
    /* otra vez ¿esto esta mal? */
    unsigned char num1,num2;
    num1 = packet.get();
    num2 = packet.get();
    laser_[0] = num1 + 256 * num2;
    
    if ( laser_[0] > NUM_LASER ) {
        printf("error in processing laser point reply\n");
        setError(SERIAL_READ_ERROR);
        laser_[0] = 0;
        return;
    }
    for (i=1; i<=laser_[0]; i++) {
        laser_[i] = packet.getUnsignedInt();
    }
    if ((laser_mode_ == 51) || (laser_mode_ == 50) || (laser_mode_ == 19))
        convert_laser(laser_);
    
    /*
    * if the position attachment was requested for the laser
    * we have to unpack the package. 
    */
    if ( POS_LASER_PI ( smask_[ SMASK_POS_DATA ] ) )
        posPackageProcess( packet, &( posDataAll_.laser ) );
    
    /* extract the time data for the 6811 */
    timePackageProcess ( packet, &posDataTime_ );
}

/* process the response from the robot which encodes the
   bumper reading of the robot */
void Status::Process_Bumper_Pkg(Packet & packet){
    unsigned char b1,b2,b3;
    b1 = packet.get();
    b2 = packet.get();
    b3 = packet.get();
    state_[ STATE_BUMPER ] = combine_bumper_vector(b3,b2,b1);
    
    /*
    * if the position attachment was requested for the bumper
    * we have to unpack the package. 
    */
    if ( POS_BUMPER_PI ( smask_[ SMASK_POS_DATA ] ) )
        posPackageProcess ( packet, &( posDataAll_.bumper ) );
    
    /* extract the time data for the 6811 */
    timePackageProcess ( packet, &posDataTime_ );
}


void Status::Process_Laser_Line_Pkg(Packet & packet){
    int i;
    unsigned char num1,num2;

    num1 = packet.get();
    num2 = packet.get();
    laser_[0] = num1 + 256 * num2;
    
    //if (laser_[0] > NUM_LASER) {
    /* arriba lo divide entre dos porque aqui no? lo ponemos*/
    if (laser_[0] > NUM_LASER/2) {
        printf("error in processing laser line reply\n");
        setError(SERIAL_READ_ERROR);
        laser_[0] = 0;
        return;
    }
    for (i=1; i<=4*laser_[0]; i++) {
        laser_[i] = packet.getSignedInt();
    }
    
    /*
    * if the position attachment was requested for the laser
    * we have to unpack the package. 
    */
    if ( POS_LASER_PI ( smask_[ SMASK_POS_DATA ] ) )
        posPackageProcess ( packet, &( posDataAll_.laser ) );
    
    /* extract the time data for the 6811 */
    timePackageProcess ( packet, &posDataTime_ );
}

/* process the response from the robot which encodes special information */
void Status::Process_Special_Pkg(Packet & packet){
    int data_size, i;
    unsigned char * special_buffer;

    special_buffer = packet.getSpecialBuffer();

    if ( special_buffer != NULL){
        special_buffer[0] = packet.get();
        special_buffer[1] = packet.get();
        data_size = two_bytes_to_unsigned_int(special_buffer[0],special_buffer[1]);
        //no se usa para nada --- special_answer_size = ( unsigned short ) data_size;
    
        if (data_size > MAX_USER_BUF) 
            data_size = MAX_USER_BUF;
    
        for (i=2; i<data_size; i++) {
            special_buffer[i] = packet.get();
        }
    } else
        printf("Data buffer for user package is NULL pointer\n");
}


int Status::posPackageProcess ( Packet & packet, PosData *posData ){
    int i = 0;
    
    /* copy the stuff from the buffer into the posData for the current robot */
    posData->config.configX      = packet.getLong();i++;
    posData->config.configY      = packet.getLong();i++;
    posData->config.configSteer  = packet.getLong();i++;
    posData->config.configTurret = packet.getLong();i++;
    posData->config.velTrans     = packet.getLong();i++;
    posData->config.velSteer     = packet.getLong();i++;
    posData->config.velTurret    = packet.getLong();i++;
    posData->config.timeStamp    = packet.getUnsignedLong();i++;
    posData->timeStamp           = packet.getUnsignedLong();i++;
    
    return ( i * sizeof(long) );
}

int Status::timePackageProcess ( Packet & packet, unsigned long *timeS ){
    *timeS = packet.getUnsignedLong();
    return ( 4 );
}
    
    
int Status::voltPackageProcess (Packet & packet, 
                        unsigned char *voltCPU,
                        unsigned char *voltMotor){

    /* read the raw voltages out of the buffer */
    *voltCPU   = packet.get();
    *voltMotor = packet.get();
    
    return ( 2 );
}


}
