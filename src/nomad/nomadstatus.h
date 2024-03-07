//
// C++ Interface: nomadstatus
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef NOMADSTATUS_H
#define NOMADSTATUS_H

#include "../nomad/nomadcommand.h"
#include "../nomad/nomadpacket.h"

#define NUM_STATE        45
#define NUM_MASK         44 
#define NUM_LASER        482 

/* the number of sonars and infrareds */
#define SONARS           16
#define INFRAREDS        16

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
 * For requesting the PosData the following defines should be used.
 * Each sensor has a bit, if it is set the pos-data is attached
 * when the sensory data is returned.
 */

#define POS_NONE          ( 0 << 0 )
#define POS_INFRARED      ( 1 << 0 )
#define POS_SONAR         ( 1 << 1 )
#define POS_BUMPER        ( 1 << 2 )
#define POS_LASER         ( 1 << 3 )
#define POS_COMPASS       ( 1 << 4 )


#define NUM_STATE        45
#define NUM_MASK         44 
#define NUM_LASER        482 

namespace Nomad{

class Status{
public:
    /**
    * The following type definitions are used for the PosData.
    * PosData is an information packet that is attached to 
    * each sensor reading, if requested. Note that the use of 
    * PosData could cause compatibility problems when different
    * releases of the software are used on the robot and on the
    * server side. 
    *
    * The information packet can be used to determine how up-to-date
    * a sensory reading is. It contains the configuration of the robot.
    * This is the most updated configuration at the time of the sensor
    * reading. However, it is possible that the sensory reading
    * was taken after the integration of the coniguration.
    * To determine the interval that has passed two timestamps are in-
    * cluded in this information package: a timestamp for the computation
    * of the configuration and another timestamp for the actual capturing
    * of the senor reading.
    *
    * The timestamps are in milliseconds of the internal clock of the 
    * board that handles the sensors (Intellisys 100 sensor-board).
    */
    
    /**
    * TimeData contains the current time of the Intellisys 100 
    * in milliseconds
    */    
    typedef unsigned long TimeData;
    
    /**
    * ConfigData is where the i486 writes the current configuration
    * of the robot, so that the Intellisys 100 can attach current
    * integration values to the sensor readings.
    * It is also used inside of the Pos data.
    */
    
    typedef struct _ConfigData
    {
        /* the configuration of the robot */
        long          configX;
        long          configY;
        long          configSteer;
        long          configTurret;
    
        /* the velocities of the robot*/
        long          velTrans;
        long          velSteer;
        long          velTurret;
    
        /* time of integration in milliseconds (Intellisys 100 time) */
        TimeData      timeStamp;
    
    } ConfigData;
    
    
    /**
    * PosData contains information that is attached to a sensor
    * reading in order to determine how recent it is.
    */
    typedef struct _PosData
    {
        /* the configuration of the robot at the time of the reading */
        ConfigData config;
    
        /* the time of the sensing in milliseconds (Intellisys 100 time) */
        TimeData   timeStamp;
    } PosData;

    /*
    * PosDataAll is a struct that contains the Pos information for
    * all sensors. It is used to pass/store the Pos info within the 
    * server. It contains the laser, although the laser is not
    * used in the dual ported ram.
    */
    
    typedef struct _PosDataAll
    {
        PosData infrared [INFRAREDS];
        PosData sonar    [SONARS   ];
        PosData bumper;
        PosData laser;
        PosData compass;
    } PosDataAll;


public:
    Status();
    ~Status();
    long getSimulationSpeed();
    long getInfraredData(int sensor);
    long getSonarData(int sensor);
    long getBumperData();
    bool getBumperData(int sensor);
    long getX();
    long getY();
    long getSteeringOrient();
    long getTurretOriente();
    long getTransVel();
    long getSteeringVel();
    long getTurretVel();
    bool isMotorTransActive();
    bool isMotorSteeringActive();
    bool isMotorTurretActive();
    long getStatusMotor();
    long getLaserStatus();
    long getCompasData();
    long getError();
    void setError(long error);
    void setLaserMode(int mode){laser_mode_ = mode;}
    int getLaserMode(){return laser_mode_;}
    
    long *getStateVector(){return state_;}
    int  *getSmaskVector(){return smask_;}
    unsigned char byteSmask(int bite);
    void init_mask();
    float voltCpuRawGet()  {return voltageCPU_;}
    float voltMotorRawGet(){return voltageMotor_;}

    /* 
    * process_???_pkg - processes packet from the robot to extract the
    *                     appropriate information 
    */
    bool Process_Robot_Resp(Packet & packet);
    void Process_State_Pkg(Packet & packet);
    void Process_Compass_Conf_Pkg(Packet & packet);
    void Process_Infrared_Pkg(Packet & packet);
    void Process_Sonar_Pkg(Packet & packet);
    void Process_Configuration_Pkg(Packet & packet);
    void Process_Velocity_Pkg(Packet & packet);
    void Process_Acceleration_Pkg(Packet & packet);
    void Process_Compass_Pkg(Packet & packet);
    void Process_Laser_Point_Pkg(Packet & packet);
    void Process_Bumper_Pkg(Packet & packet);
    void Process_Laser_Line_Pkg(Packet & packet);
    void Process_Special_Pkg(Packet & packet);


    /* 
    * process_???_reply - processes reply from the server to extract the
    *                     appropriate information 
    */
    bool process_state_reply(Command & this_reply);
    bool process_infrared_reply(Command & this_reply);
    bool process_sonar_reply(Command & this_reply);
    bool process_configuration_reply(Command & this_reply);
    bool process_conf_reply(Command & this_reply, long *conf);
    
    bool process_velocity_reply(Command & this_reply);
    bool process_compass_reply(Command & this_reply);
    bool process_bumper_reply(Command & this_reply);
    bool process_laser_reply(Command & this_reply);
    bool process_predict_reply(Command & this_reply, long *state, int *laser);
    
    bool process_simple_reply(Command & this_reply);
    bool process_obstacle_reply(Command & this_reply, long *obs);
    bool process_rpx_reply(Command & this_reply, long *robot_pos);
    int process_socket_reply(Command & this_reply);
    /* this is something special for Greg at Yale */
    bool process_mcheck_reply (Command & this_reply, double collide[3]);
    bool process_special_reply(Command & this_reply, unsigned char *data);

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
    * FUNCTION:     voltCpuGet
    * PURPOSE:      get the voltage of the power supply for the CPU
    * ARGUMENTS:    None
    * ALGORITHM:    ---
    * RETURN:       float (the voltage in volt)
    * SIDE EFFECT:  
    * CALLS:        
    * CALLED BY:    
    ***************/
    float voltCpuGet         ( void );
    
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
    float voltMotorGet         ( void );
    
private:    
    
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
    int posDataCheck ( void );
    
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
    int posInfraredRingGet ( PosData posData[INFRAREDS] );
    
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
    int posInfraredGet     ( PosData *posData , int infraredNumber);
    
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
    int posSonarRingGet    ( PosData posData[SONARS] );
    
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
    int posSonarGet        ( PosData *posData , int sonarNumber);
    
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
    int posBumperGet       ( PosData *posData );
    
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
    int posLaserGet        ( PosData *posData );
    
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
    int posCompassGet      ( PosData *posData );
    
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
    int posTimeGet         ( void );
    
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
    float voltConvert ( unsigned char reading , float range );
    
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
    int posDataProcess (long *buffer, int current, PosData *posData);
    
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
    int timeDataProcess ( long *buffer, int current, TimeData *theTime );
    
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
    int voltDataProcess (long *buffer, int current, 
                         unsigned char *voltCPU, unsigned char *voltMotor);
    
    /****************************************************************/


    /***************
    * FUNCTION:     posPackageProcess
    * PURPOSE:      processes the part of the package with pos information
    * ARGUMENTS:    unsigned char *inbuf : pointer to the data in chars
    *               PosData *posData : this is were the posData are written to
    * ALGORITHM:    regroup the bytes and assign variables
    * RETURN:       int (the number of bytes read from the buffer)
    * SIDE EFFECT:  
    * CALLS:        
    * CALLED BY:    
    ***************/
    int posPackageProcess ( Packet & packet, PosData *posData );

    /***************
    * FUNCTION:     timePackageProcess
    * PURPOSE:      processes the part of the package with the 6811 time
    * ARGUMENTS:    unsigned char *inbuf : pointer to the data in chars
    *               unsigned long *time : this is were the time is written to
    * ALGORITHM:    ---
    * RETURN:       static int 
    * SIDE EFFECT:  
    * CALLS:        
    * CALLED BY:    
    ***************/
    int timePackageProcess ( Packet & packet, unsigned long *timeS );
    
    /***************
    * FUNCTION:     voltPackageProcess
    * PURPOSE:      processes the part of the package with the voltages
    * ARGUMENTS:    unsigned char *inbuf : pointer to the data in chars
    *               unsigned long *time : this is were the time is written to
    * ALGORITHM:    ---
    * RETURN:       static int 
    * SIDE EFFECT:  
    * CALLS:        
    * CALLED BY:    
    ***************/
    int voltPackageProcess (Packet & packet, unsigned char *voltCPU, unsigned char *voltMotor);



private:

    long   state_[NUM_STATE];
    int    smask_[NUM_MASK];
    int    laser_[2*NUM_LASER+1];
    int    laser_mode_;
/* this is where all the incoming posData is stored */
    PosDataAll posDataAll_;
    unsigned long posDataTime_;

/* for the voltages of motor/CPU as raw data */
    unsigned char voltageCPU_;
    unsigned char voltageMotor_;
    int own_tcp_port_;

};

};

#endif
