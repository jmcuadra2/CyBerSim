#ifndef ROBOTKINETICSTAB_H
#define ROBOTKINETICSTAB_H

#include "ui_robotkineticstab.h"
#include <qewextensibledialog.h>

class Robot;

class RobotKineticsTab : public QewExtensibleDialog, public Ui::RobotKineticsTab
{
    Q_OBJECT

public:
    RobotKineticsTab(QWidget* parent = 0, const char* name = 0);
    ~RobotKineticsTab();

    virtual void init( Robot * rob );
    virtual double getX( void );
    virtual double getY( void );
    virtual double getRotation( void );
    virtual double getRVel( void );
    virtual double getLVel( void );

public slots:
    virtual void setX( double d );
    virtual void setY( double d );
    virtual void setRotation( double d );
    virtual void setRVel( double d );
    virtual void setLVel( double d );
    virtual void setSaveKinetics( bool sk );

protected:
    Robot* robot;
    double rotation, init_rotation;
    double r_vel;
    double l_vel;
    double y, init_y;
    double x, init_x;
    bool saveKinetics;

    void saveThis( void );

protected slots:
    virtual void languageChange();

};

#endif // ROBOTKINETICSTAB_H
