#ifndef NDARIAROBOTKINETICSTAB_H
#define NDARIAROBOTKINETICSTAB_H

#include "ui_robotkineticstab.h"
#include <qewextensibledialog.h>

class NDAriaRobot;

class NDAriaRobotKineticsTab : public QewExtensibleDialog, public Ui::RobotKineticsTab
{
    Q_OBJECT

  public:
    NDAriaRobotKineticsTab(QWidget* parent = 0, const char* name = 0);
    ~NDAriaRobotKineticsTab();

    virtual void init(NDAriaRobot * rob);
    virtual double getX(void);
    virtual double getY(void);
    virtual double getRotation(void);
    virtual double getRVel(void);
    virtual double getLVel(void);

  public slots:
    virtual void setX(double d);
    virtual void setY(double d);
    virtual void setRotation(double d);
    virtual void setRVel(double d);
    virtual void setLVel(double d);
    virtual void setSaveKinetics(bool sk);

  protected:
    NDAriaRobot* robot;
    double rotation;
    double r_vel;
    double l_vel;
    double y;
    double x;
    bool saveKinetics;

    void saveThis(void);

  protected slots:
    virtual void languageChange();

};

#endif // NDARIAROBOTKINETICSTAB_H
