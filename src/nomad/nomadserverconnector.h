//
// C++ Interface: nomadserverconnector
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADSERVERCONNECTOR_H
#define NOMADSERVERCONNECTOR_H

#include "../nomad/nomadclientconnector.h"
#include "../nomad/nomadcommand.h"

#include <string>



#define MAX_VERTICES     10

/**
	@author Javier Garcia Misis
*/
namespace Nomad{

class ServerConnector : public Nomad::Connector
{
public:
    ServerConnector(long robot_id,std::string host,int port);

    ~ServerConnector();
    /*****************************
    *                           *
    * Robot Interface Functions *
    *                           *
    *****************************/
    virtual bool create_robot();
    virtual bool connect_robot();
    virtual bool disconnect_robot();


    virtual bool ac(int t_ac, int s_ac, int r_ac);
    virtual bool sp(int t_sp, int s_sp, int r_sp);
    virtual bool pr(int t_pr, int s_pr, int r_pr);
    virtual bool pa(int t_pa, int s_pa, int r_pa);
    virtual bool vm(int t_vm, int s_vm, int r_vm);
    virtual bool mv(int t_mode, int t_mv, int s_mode, int s_mv, int r_mode, int r_mv);
    virtual bool ct(void);
    virtual bool gs(void);
    virtual bool st(void);
    virtual bool lp(void);
    virtual bool tk(char *talk_string);
    virtual bool dp(int x, int y);
    virtual bool zr();
    virtual bool conf_ir(int history, int order[16]);
    virtual bool conf_sn(int rate, int order[16]);
    virtual bool conf_cp(int mode);
    virtual bool conf_ls(unsigned int mode,
                        unsigned int threshold,
                        unsigned int width,
                        unsigned int num_data,
                        unsigned int processing);
    virtual bool conf_tm(unsigned char timeout);
    virtual bool get_ir(void);
    virtual bool get_sn(void);
    virtual bool get_rc(void);
    virtual bool get_rv(void);
    virtual bool get_ra(void);
    virtual bool get_cp(void);
    virtual bool get_ls(void);
    virtual bool get_bp(void);
    virtual bool conf_sg(unsigned int threshold, 
                        unsigned int min_points, 
                        unsigned int gap);
    virtual bool get_sg(void);
    virtual bool da(int th, int tu);
    virtual bool ws(unsigned char t_ws, unsigned char s_ws,
                unsigned char r_ws, unsigned char timeout);
    virtual bool get_rpx(long *robot_pos);

    /*****************************
    *                           *
    * World Interface Functions *
    *                           *
    *****************************/
    
    virtual bool add_obstacle(long obs[2*MAX_VERTICES+1]);
    virtual bool delete_obstacle(long obs[2*MAX_VERTICES+1]);
    virtual bool move_obstacle(long obs[2*MAX_VERTICES+1], long dx, long dy);
    virtual bool new_world(void);

    /****************************
    *                          *
    * Graphics refresh control *
    *                          *
    ****************************/
    virtual bool refresh_all(void);
    virtual bool refresh_all_traces(void);
    virtual bool refresh_actual_trace(void);
    virtual bool refresh_encoder_trace(void);
    virtual bool refresh_all_sensors(void);
    virtual bool refresh_bumper_sensor(void);
    virtual bool refresh_infrared_sensor(void);
    virtual bool refresh_sonar_sensor(void);
    virtual bool refresh_laser_sensor(void);
    virtual bool refresh_client_graphics(void);

    /*******************************
    *                             *
    * Miscellaneous robot control *
    *                             *
    *******************************/

    virtual bool place_robot(int x, int y, int th, int tu);
    virtual bool special_request(unsigned char *user_send_buffer,
                                unsigned char *user_receive_buffer);

    /*******************************
    *                             *
    * Graphic Interface Functions *
    *                             *
    *******************************/
    
    virtual bool draw_robot(long x, long y, int th, int tu, int mode);
    virtual bool draw_line(long x_1, long y_1, long x_2, long y_2, int mode);
    virtual bool draw_arc(long x_0, long y_0, long w, long h, int th1, int th2, int mode);

    /*************************************
    *                                   *
    * Miscellaneous Interface Functions *
    *                                   *
    *************************************/
    
    virtual bool server_is_running(void);
    virtual bool quit_server(void);
    virtual bool real_robot(void);
    virtual bool simulated_robot(void);
    virtual bool predict_sensors(int x, int y, int th, int tu, long *state, int *laser);
    virtual bool motion_check(long type, double a1, double a2, double a3, double a4, 
            double a5, double a6, double a7, double collide[3]);
    virtual bool get_robot_conf(long *conf);







    /***************
    * FUNCTION:     posDataRequest
    * PURPOSE:      request position information for sensors
    * ARGUMENTS:    int posRequest : 
    *               The argument of this function specifies the sensors 
    *               for which the position information (PosData) should 
    *               be attached to the sensory reading.
    *               Its value is obtained by ORing the desired defines. 
    * EXAMPLE:      To attach PosData to sonars and laser:
    *               posDataRequest ( POS_SONAR | POS_LASER );
    * ALGORITHM:    currently sets the global variable Smask[0] and
    *               then calls ct() to transmit the change to the server
    * RETURN:       TRUE if the argument was correct, else false
    * SIDE EFFECT:  Smask[ SMASK_POS_DATA ]
    * CALLS:        
    * CALLED BY:    
    ***************/
    int posDataRequest ( int posRequest );

private:
    bool open_socket();
    bool close_socket();
    bool ipc_comm(Command & this_request,
                  Command & this_reply);
    bool write_request_to_socket(Command & this_request);
    bool read_reply_from_socket(Command & this_reply);


private:
    std::string host_;
    int port_;
    int socket_;
    int connectedp_;
};

};
#endif
