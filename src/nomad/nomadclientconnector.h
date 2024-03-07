//
// C++ Interface: nomadclientconnector
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADCLIENTCONNECTOR_H
#define NOMADCLIENTCONNECTOR_H

#define MAX_VERTICES     10

#include "../nomad/nomadstatus.h"

namespace Nomad{
/**
	@author Javier Garcia Misis
*/
class Connector{
public:
    Connector(long robot_id);

    virtual ~Connector();


    /*****************************
    *                           *
    * Robot Interface Functions *
    *                           *
    *****************************/
    
    /**
    * create_robot - requests the server to create a robot with
    *                id = robot_id and establishes a connection with
    *                the robot. This function is disabled in this
    *                version of the software.
    * 
    */
    virtual bool create_robot(){return true;}
    
    /**
    * connect_robot - requests the server to connect to the robot
    *                 with id = robot_id. In order to talk to the server,
    *                 the SERVER_MACHINE_NAME and SERV_TCP_PORT must be
    *                 set properly. If a robot with robot_id exists,
    *                 a connection is established with that robot. If
    *                 no robot exists with robot_id, no connection is
    *                 established.
    */
    virtual bool connect_robot()=0;
    
    /**
    * disconnect_robot - requests the server to close the connect with robot
    *                    with id = robot_id. 
    */
    virtual bool disconnect_robot()=0;
    
    /**
    * ac - sets accelerations of the robot. Currently it has no effect in 
    *      simulation mode.
    *
    * parameters:
    *    int t_ac, s_ac, r_ac -- the translation, steering, and turret
    *                            accelerations. t_ac is in 1/10 inch/sec^2
    *                            s_ac and r_ac are in 1/10 degree/sec^2.
    */
    virtual bool ac(int t_ac, int s_ac, int r_ac)=0;
    
    /**
    * sp - sets speeds of the robot, this function will not cause the robot to
    *      move. However, the set speed will be used when executing a pr()
    *      or a pa().
    *
    * parameters:
    *    int t_sp, s_sp, r_sp -- the translation, steering, and turret
    *                            speeds. t_sp is in 1/10 inch/sec and
    *                            s_sp and r_sp are in 1/10 degree/sec.
    */
    virtual bool sp(int t_sp, int s_sp, int r_sp)=0;
    
    /**
    * pr - moves the motors of the robot by a relative distance, using the speeds
    *      set by sp(). The three parameters specify the relative distances for
    *      the three motors: translation, steering, and turret. All the three
    *      motors move concurrently if the speeds are not set to zero and the 
    *      distances to be traveled are non-zero. Depending on the timeout 
    *      period set (by function conf_tm(timeout)), the motion may 
    *      terminate before the robot has moved the specified distances
    *
    * parameters:
    *    int t_pr, s_pr, r_pr -- the specified relative distances of the
    *                            translation, steering, and turret motors.
    *                            t_pr is in 1/10 inch and s_pr and r_pr are
    *                            in 1/10 degrees.
    */
    virtual bool pr(int t_pr, int s_pr, int r_pr)=0;

    /**
    * pa - moves the motors of the robot to the specified absolute positions 
    *      using the speeds set by sp().  Depending on the timeout period set 
    *      (by conf_tm()), the motion may terminate before the robot has 
    *      moved to the specified positions.
    *
    * parameters:
    *    int t_pa, s_pa, r_pa -- the specified absolute positions of the
    *                            translation, steering, and turret motors.
    *                            t_pa is in 1/10 inch and s_pa and r_pa are
    *                            in 1/10 degrees.
    */
    virtual bool pa(int t_pa, int s_pa, int r_pa)=0;

    
    /**
    * vm - velocity mode, command the robot to move at translational
    *      velocity = tv, steering velocity = sv, and rotational velocity =
    *      rv. The robot will continue to move at these velocities until
    *      either it receives another command or this command has been
    *      timeout (in which case it will stop its motion).
    *
    * parameters: 
    *    int t_vm, s_vm, r_vm -- the desired translation, steering, and turret
    *                            velocities. tv is in 1/10 inch/sec and
    *                            sv and rv are in 1/10 degree/sec.
    */
    virtual bool vm(int t_vm, int s_vm, int r_vm)=0;
    
    /**
    * mv - move, send a generalized motion command to the robot.
    *      For each of the three axis (translation, steering, and
    *      turret) a motion mode (t_mode, s_mode, r_mode) can be 
    *      specified (using the values MV_IGNORE, MV_AC, MV_SP,
    *      MV_LP, MV_VM, and MV_PR defined above):
    *
    *         MV_IGNORE : the argument for this axis is ignored
    *                     and the axis's motion will remain 
    *                     unchanged.
    *         MV_AC :     the argument for this axis specifies
    *                     an acceleration value that will be used
    *                     during motion commands.
    *         MV_SP :     the argument for this axis specifies
    *                     a speed value that will be used during
    *                     position relative (PR) commands.
    *         MV_LP :     the arguemnt for this axis is ignored
    *                     but the motor is turned off.
    *         MV_VM :     the argument for this axis specifies
    *                     a velocity and the axis will be moved
    *                     with this velocity until a new motion
    *                     command is issued (vm,pr,mv) or 
    *                     recieves a timeout.
    *         MV_PR :     the argument for this axis specifies
    *                     a position and the axis will be moved
    *                     to this position, unless this command
    *                     is overwritten by another (vm,pr,mv).
    *
    * parameters: 
    *    int t_mode - the desired mode for the tranlation axis
    *    int t_mv   - the value for that axis, velocity or position,
    *                 depending on t_mode
    *    int s_mode - the desired mode for the steering axis
    *    int s_mv   - the value for that axis, velocity or position,
    *                 depending on t_mode
    *    int r_mode - the desired mode for the turret axis
    *    int r_mv   - the value for that axis, velocity or position,
    *                 depending on t_mode
    */
    virtual bool mv(int t_mode, int t_mv, int s_mode, int s_mv, int r_mode, int r_mv)=0;
    
    /**
    * ct - send the sensor mask, Smask, to the robot. You must first change
    *      the global variable Smask to the desired communication mask before
    *      calling this function. 
    */
    virtual bool ct(void)=0;
    
    /**
    * gs - get the current state of the robot according to the mask (of 
    *      the communication channel)
    */
    virtual bool gs(void)=0;
    
    /**
    * st - stops the robot (the robot holds its current position)
    */
    virtual bool st(void)=0;
    
    /**
    * lp - set motor limp (the robot may not hold its position).
    */
    virtual bool lp(void)=0;
    
    /**
    * tk - sends the character stream, talk_string, to the voice synthesizer
    *      to make the robot talk.
    *
    * parameters:
    *    char *talk_string -- the string to be sent to the synthesizer.
    */
    virtual bool tk(char *talk_string)=0;
    
    /**
    * dp - define the current position of the robot as (x,y)
    * 
    * parameters:
    *    int x, y -- the position to set the robot to.
    */
    virtual bool dp(int x, int y)=0;
    
    /**
    * zr - zeroing the robot, align steering and turret with bumper zero.
    *      The position, steering and turret angles are all set to zero.
    *      This function returns when the zeroing process has completed.
    */
    virtual bool zr()=0;
    
    /**
    * conf_ir - configure infrared sensor system.
    *
    * parameters: 
    *    int history -- specifies the percentage dependency of the current 
    *                   returned reading on the previous returned reading.
    *                   It should be set between 0 and 10: 0 = no dependency 
    *                   10 = full dependency, i.e. the reading will not change
    *    int order[16] --  specifies the firing sequence of the infrared 
    *                      (#0 .. #15). You can terminate the order list by a 
    *                      "255". For example, if you want to use only the 
    *                      front three infrared sensors then set order[0]=0,
    *                      order[1]=1, order[2]=15, order[3]=255 (terminator).
    */
    virtual bool conf_ir(int history, int order[16])=0;
    
    /**
    * conf_sn - configure sonar sensor system.
    *
    * parameters:
    *    int rate -- specifies the firing rate of the sonar in 4 milli-seconds 
    *                interval; 
    *    int order[16] -- specifies the firing sequence of the sonar (#0 .. #15).
    *                     You can terminate the order list by a "255". For 
    *                     example, if you want to use only the front three 
    *                     sensors, then set order[0]=0, order[1]=1, order[2]=15, 
    *                     order[3]=255 (terminator).
    */
    virtual bool conf_sn(int rate, int order[16])=0;
    
    /**
    * conf_cp - configure compass system.
    * 
    * parameters:
    *    int mode -- specifies compass on/off: 0 = off ; 1 = on; 2 = calibrate.
    *                When you call conf_cp (2), the robot will rotate slowly 360
    *                degrees. You must wait till the robot stops rotating before
    *                issuing another command to the robot (takes ~3 minutes).
    */
    virtual bool conf_cp(int mode)=0;
    
    /**
    * conf_ls - configure laser sensor system:
    *
    * parameters:
    *    unsigned int mode -- specifies the on-board processing mode of the laser 
    *                         sensor data which determines the mode of the data 
    *                         coming back: 
    *                           the first bit specifies the on/off;
    *                           the second bit specifies point/line mode;
    *                           the third to fifth bits specify the 
    *                           returned data types: 
    *                             000 = peak pixel, 
    *                             001 = rise pixel, 
    *                             010 = fall pixel, 
    *                             011 = magnitude,
    *                             100 = distance;
    *                           the sixth bit specifies data integrity checking.
    *
    *   unsigned int threshold -- specifies the inverted acceptable brightness
    *                             of the laser line. 
    *
    *   unsigned int width -- specifies the acceptable width in terms
    *                         of number of pixels that are brighter than the 
    *                         set threshold.
    *  
    *   unsigned int num_data -- specifies the number of sampling points. 
    *   unsigned int processing --  specifies the number of neighboring 
    *                               pixels for averaging
    *
    * If you don't understand the above, try this one:
    *   conf_ls(51, 20, 20, 20, 4)
    */
    virtual bool conf_ls(unsigned int mode,
                        unsigned int threshold,
                        unsigned int width,
                        unsigned int num_data,
                        unsigned int processing)=0;
    
    /**
    * conf_tm - sets the timeout period of the robot in seconds. If the
    *           robot has not received a command from the host computer
    *           for more than the timeout period, it will abort its 
    *           current motion
    * 
    * parameters:
    *    unsigned int timeout -- timeout period in seconds. If it is 0, there
    *                            will be no timeout on the robot.
    */
    virtual bool conf_tm(unsigned char timeout)=0;
    
    /**
    * get_ir - get infrared data, independent of mask. However, only 
    *          the active infrared sensor readings are valid. It updates
    *          the State vector.
    */
    virtual bool get_ir(void)=0;
    
    /**
    * get_sn - get sonar data, independent of mask. However, only 
    *          the active sonar sensor readings are valid. It updates
    *          the State vector.
    */
    virtual bool get_sn(void)=0;
    
    /**
    * get_rc - get robot configuration data (x, y, th, tu), independent of 
    *          mask. It updates the State vector.
    */
    virtual bool get_rc(void)=0;
    
    /**
    * get_rv - get robot velocities (translation, steering, and turret) data,
    *          independent of mask. It updates the State vector.
    */
    virtual bool get_rv(void)=0;
    
    /**
    * get_ra - get robot acceleration (translation, steering, and turret) data,
    *          independent of mask. It updates the State vector.
    */
    virtual bool get_ra(void)=0;
    
    /**
    * get_cp - get compass data, independent of mask. However, the
    *          data is valid only if the compass is on. It updates the
    *          State vector.
    */
    virtual bool get_cp(void)=0;
    
    /**
    * get_ls - get laser data point mode, independent of mask. However the
    *          data is valid only of the laser is on. It updates the Laser 
    *          vector.
    */
    virtual bool get_ls(void)=0;
    
    /**
    * get_bp - get bumper data, independent of mask. It updates the State
    *          vector.
    */
    virtual bool get_bp(void)=0;
    
    /**
    * conf_sg - configure laser sensor system line segment processing mode:
    *
    * parameters:
    *    unsigned int threshold -- specifies the threshold value for least-square
    *                             fitting. When the error term grows above the 
    *                             threshold, the line segment will be broken
    *    unsigned int min_points -- specifies the acceptable number of points
    *                              to form a line segment.
    *    unsigned int gap -- specifies the acceptable "gap" between two segments
    *                        while they can still be treated as one (in 1/10 inch)
    *
    * If you don't understand the above, try this one:
    *    conf_sg(50, 4, 30)
    */
    virtual bool conf_sg(unsigned int threshold, 
                        unsigned int min_points, 
                        unsigned int gap)=0;
    
    /**
    * get_sg - get laser data line mode, independent of mask. It updates
    *          the laser vector.
    */
    virtual bool get_sg(void)=0;
    
    /**
    * da - define the current steering angle of the robot to be th
    *      and the current turret angle of the robot to be tu.
    * 
    * parameters:
    *    int th, tu -- the steering and turret orientations to set the
    *                  robot to.
    */
    virtual bool da(int th, int tu)=0;
    
    /**
    * ws - waits for stop of motors of the robot. This function is intended  
    *      to be used in conjunction with pr() and pa() to detect the desired
    *      motion has finished
    *
    * parameters:
    *    unsigned char t_ws, s_ws, r_ws -- These three parameters specify 
    *                                      which axis or combination of axis 
    *                                      (translation, steering, and turret) 
    *                                      to wait. 
    *    unsigned char timeout -- specifies how long to wait before timing out 
    *                             (return without stopping the robot).
    */
    virtual bool ws(unsigned char t_ws, unsigned char s_ws,
                unsigned char r_ws, unsigned char timeout)=0;
    
    /**
    * get_rpx - get the position of all nearby robots
    */
    virtual bool get_rpx(long *robot_pos)=0;



    /*****************************
    *                           *
    * World Interface Functions *
    *                           *
    *****************************/
    
    /**
    * add_obstacle - creates an obstacle and adds it to the obstacle list
    *                of the robot environment. 
    * 
    * No effect in direct mode.
    * 
    * parameters:
    *    long obs[2*MAX_VERTICES+1] -- 
    *                The first element of obs specifies the number of 
    *                vertices of the polygonal obstacle (must be no greater 
    *                than MAX_VERTICES). The subsequent elements of obs 
    *                specifies the x and y coordinates of the vertices, 
    *                in counter-clockwise direction.
    */
    virtual bool add_obstacle(long obs[2*MAX_VERTICES+1])=0;
    
    
    /**
    * delete_obstacle - deletes an obstacle specified by obs from the robot 
    *                   environment 
    * parameters:
    *    long obs[2*MAX_VERTICES+1] -- 
    *                The first element of obs specifies the number of 
    *                vertices of the polygonal obstacle (must be no greater 
    *                than MAX_VERTICES). The subsequent elements of obs 
    *                specifies the x and y coordinates of the vertices, 
    *                in counter-clockwise direction.
    */
    virtual bool delete_obstacle(long obs[2*MAX_VERTICES+1])=0;
    
    
    /**
    * move_obstacle - moves the obstacle obs by dx along x direction and 
    *                 dy along y direction. obs is modified.
    *
    * No effect in direct mode.
    * 
    * parameters:
    *    long obs[2*MAX_VERTICES+1] -- 
    *                The first element of obs specifies the number of 
    *                vertices of the polygonal obstacle (must be no greater 
    *                than MAX_VERTICES). The subsequent elements of obs 
    *                specifies the x and y coordinates of the vertices, 
    *                in counter-clockwise direction.
    *    long dx, dy -- the x and y distances to translate the obstacle
    */
    virtual bool move_obstacle(long obs[2*MAX_VERTICES+1], long dx, long dy)=0;
    
    /**
    * new_world - deletes all obstacles in the current robot world
    *
    * No effect in direct mode.
    * 
    */
    virtual bool new_world(void)=0;

    /****************************
    *                          *
    * Graphics refresh control *
    *                          *
    ****************************/
    
    /**
    * refresh_all - causes all temporary drawing in graphics window, including
    *               traces, sensors, and client graphics to be erased
    */
    virtual bool refresh_all(void)=0;
    
    /**
    * refresh_all_traces - causes all robot traces in graphics to be erased
    */
    virtual bool refresh_all_traces(void)=0;
    
    /**
    * refresh_actual_trace - causes actual robot trace in graphics to be erased
    */
    virtual bool refresh_actual_trace(void)=0;
    
    /**
    * refresh_encoder_trace - causes encoder robot trace in graphics to be erased
    */
    virtual bool refresh_encoder_trace(void)=0;
    
    /**
    * refresh_all_sensors - causes all sensor drawings in graphics to be erased
    */
    virtual bool refresh_all_sensors(void)=0;
    
    /**
    * refresh_bumper_sensor - causes bumper drawings in graphics to be erased
    */
    virtual bool refresh_bumper_sensor(void)=0;
    
    /**
    * refresh_infrared_sensor - causes infrared drawings in graphics to be erased
    */
    virtual bool refresh_infrared_sensor(void)=0;
    
    /**
    * refresh_sonar_sensor - causes sonar drawings in graphics to be erased
    */
    virtual bool refresh_sonar_sensor(void)=0;
    
    /**
    * refresh_laser_sensor - causes laser drawings in graphics to be erased
    */
    virtual bool refresh_laser_sensor(void)=0;
    
    /**
    * refresh_client_graphics - causes drawings performed by any clients into
    *                           graphics window to be erased
    */
    virtual bool refresh_client_graphics(void)=0;


    /*******************************
    *                             *
    * Miscellaneous robot control *
    *                             *
    *******************************/
    
    /**
    * init_mask - initialize the sensor mask, Smask.
    */
    virtual void init_mask();
    
    /**
    * init_sensors - initialize the sensor mask, Smask, and send it to the
    *                robot. It has no effect on the sensors 
    */
    virtual bool init_sensors();
    
    /**
    * place_robot - places the robot at configuration (x, y, th, tu). 
    *               In simulation mode, it will place both the Encoder-robot
    *               and the Actual-robot at this configuration. In real robot
    *               mode, it will call dp(x, y) and da(th, tu).
    * 
    * parameters:
    *    int x, y -- x-y position of the desired robot configuration
    *    int th, tu -- the steering and turret orientation of the robot
    *                  desired configuration
    */
    virtual bool place_robot(int x, int y, int th, int tu)=0;
    
    /**
    * special_request - sends a special request (stored in user_send_buffer) 
    *                   to the robot and waits for the robot's response (which
    *                   will be stored in user_receive_buffer). 
    * 
    * parameters:
    *    unsigned char *user_send_buffer -- stores data to be sent to the robot
    *                                       Should be a pointer to an array of
    *                                       1024 elements
    *    unsigned char *user_receive_buffer -- stores data received from the robot
    *                                          Should be a pointer to an array of 
    *                                          1024 elements
    */
    virtual bool special_request(unsigned char *user_send_buffer,
                                unsigned char *user_receive_buffer)=0;



    /*******************************
    *                             *
    * Graphic Interface Functions *
    *                             *
    *******************************/
    
    /**
    * draw_robot - this function allows the client to draw a robot at
    *              configuration x, y, th, tu (using the robot world 
    *              coordinates). 
    * 
    * No effect in direct mode.
    * 
    * parameters:
    *    long x, y -- the x-y position of the robot.
    *    int th, tu -- the steering and turret orientation of the robot
    *    int mode - the drawing mode. If mode = 1, the robot is drawn in 
    *              BlackPixel using GXxor (using GXxor you can erase the trace 
    *              of robotby drawing over it). If mode = 2, the robot is 
    *              drawn in BlackPixel using GXxor and in addition, a small arrow
    *              is drawn at the center of the robot using GXcopy (using this 
    *              mode you can leave a trace of small arrow). If mode = 3, 
    *              the robot is drawn in BlackPixel using GXcopy. When mode > 3,
    *              the robot is drawn in color (GXxor) using color(mode-3), see
    *              Color table below.
    */
    virtual bool draw_robot(long x, long y, int th, int tu, int mode)=0;
    
    
    /**
    * draw_line - this function allows the client to draw a line from
    *             (x_1, y_1) to (x_2, y_2) (using the robot world coordinates). 
    *
    * No effect in direct mode.
    * 
    * parameters:
    *    long x_1, y_1, x_2, y_2 -- the two end-points of the line
    *    int mode -- the mode of drawing: when mode is 1, the drawing is 
    *                done in BlackPixel using GXcopy; when mode is 2, the drawing
    *                is done in BlackPixel using GXxor, when mode > 2, the drawing
    *                is done in color (GXxor) using color(mode-2), see Color table
    *                below.
    */
    virtual bool draw_line(long x_1, long y_1, long x_2, long y_2, int mode)=0;
    
    
    /**
    * draw_arc - this function allows the client to draw arc which is part
    *            of an ellipse (using the robot world coordinates). 
    *
    * No effect in direct mode.
    * 
    * parameters:
    *    long x_0, y_0, w, h -- (x_0, y_0) specifies the upper left corner of the 
    *                          rectangle bounding the ellipse while w and h
    *                          specifies the width and height of the bounding 
    *                          rectangle, respectively.
    *    int th1, th2 -- th1 and th2 specifies the angular range of the arc.
    *    int mode -- the mode of drawing: when mode is 1, the drawing is 
    *                done in BlackPixel using GXcopy; when mode is 2, the drawing
    *                is done in BlackPixel using GXxor, when mode > 2, the drawing
    *                is done in color (GXxor) using color(mode-2), see Color table
    *                below.
    */
    virtual bool draw_arc(long x_0, long y_0, long w, long h, int th1, int th2, int mode)=0;
    
    /** 
    * Color table:
    *   color1             = Blue
    *   color2     = NavyBlue
    *   color3     = RoyalBlue
    *   color4     = SteelBlue
    *   color5     = CadetBlue
    *   color6     = Green
    *   color7     = SeaGreen
    *   color8     = ForestGreen
    *   color9     = DarkGreen
    *   color10        = LimeGreen
    *   color11        = Yellow
    *   color12        = Orange
    *   color13        = LightCoral
    *   color14        = DeepPink
    *   color15        = OrangeRed
    *   color16        = Red
    *   color17        = IndianRed
    *   color18        = VioletRed
    *   color19        = DeepPink
    *   color20        = Maroon
    */

    /*************************************
    *                                   *
    * Miscellaneous Interface Functions *
    *                                   *
    *************************************/
    
    /**
    * server_is_running - this function queries the server to see
    *                     if it is up and running.  If so, this function
    *                     returns a TRUE, otherwise it returns false.
    *                     This function is replaced by connect_robot, but 
    *                     is defined here for backward compatibility
    *
    * No effect in direct mode.
    * 
    */
    virtual bool server_is_running(void)=0;
    
    /**
    * quit_server - this function allows the client to quit the server
    *               assuming this feature is enabled in the setup file
    *               of the server
    *
    * No effect in direct mode.
    * 
    */
    virtual bool quit_server(void)=0;
    
    /**
    * real_robot - this function allows the client to switch to
    *              real robot mode in the server
    *
    * No effect in direct mode.
    * 
    */
    virtual bool real_robot(void)=0;
    
    /**
    * simulated_robot - this function allows the client to switch to
    *                   simulated robot mode in the server
    *
    * No effect in direct mode.
    * 
    */
    virtual bool simulated_robot(void)=0;
    
    /**
    * predict_sensors - this function predicts the sensor reading of
    *                   the robot assuming it is at position (x, y)
    *                   and orientation th and tu using the map of the
    *                   simulated robot environment. The predicted sensor
    *                   data values are stored in "state" and "laser".
    * 
    * No effect in direct mode.
    * 
    * parameters:
    *    int x, y, th, tu -- the configuration of the robot
    *    long *state -- where to put the predicted state data
    *    int *laser -- where to put the predicted laser data
    */
    virtual bool predict_sensors(int x, int y, int th, int tu, long *state, int *laser)=0;
    
    /**
    * motion_check - this function computes the intersection of a path
    *                specified by the parameters: type, a1, ..., a7 with
    *                the obstacles in the robot's environment. If there is
    *                collision, the function returns 1 and the x-y configuration
    *                of the robot is stored in collide[0] and collide[1] while
    *                collide[2] stores the inward normal of the obstacle edge
    *                that the robot collides with (this information can be
    *                used to calculate which bumper is hit.). If there is no
    *                collision, the function returns 0.
    *
    * No effect in direct mode.
    * 
    * parameters:
    *    long type - 0 if the path is a line segment
    *                1 if the path is an arc of circle
    *    double a1 a2 - x-y coordinates of the first point of the path (the path
    *                   is directional).
    *    depending on the value of type, a3 - a7 have different meanings.
    *    if (type == 0), line segment mode
    *      double a3 a4 are the x-y coordinates of the second point of the path
    *      a5, a6, a7 have no meaning
    *    if (type == 1), arc of circle mode
    *      double a3 is the angle (in radiance) of the vector connecting the 
    *                center of the circle to the first end-point of the arc
    *      double a4 is the angle of the vector connecting the center
    *                of the circle to the second end-point of the arc
    *      double a5 is the radius of the circle
    *      double a6 a7 are the x-y coordinate of the center of the circle
    */
    virtual bool motion_check(long type, double a1, double a2, double a3, double a4, 
            double a5, double a6, double a7, double collide[3])=0;
    
    /**
    * get_robot_conf - interactively getting the robot's conf, by clicking
    *                  the mouse in the server's Robot window
    * 
    * No effect in direct mode.
    * 
    * parameters:
    *    long *conf -- should be an array of 4 long integers. The configuration
    *                  of the robot is returned in this array.
    */
    virtual bool get_robot_conf(long *conf)=0;

    Nomad::Status * getStatus(){return &status_;}

protected:
    int robot_id_;
    /*estatus data*/

    Nomad::Status status_;
    int    laser_mode_;
};

   
};

#endif
