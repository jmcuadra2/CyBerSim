#include "robotkineticstab.h"

#include <QVariant>
#include "../robot.h"

/*
 *  Constructs a RobotKineticsTab which is a child of 'parent', with the
 *  name 'name'.'
 */
RobotKineticsTab::RobotKineticsTab(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);

    QObject::connect(rotationFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setRotation(double)));
    QObject::connect(velLFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setLVel(double)));
    QObject::connect(velRFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setRVel(double)));
    QObject::connect(xFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setX(double)));
    QObject::connect(yFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setY(double)));
    QObject::connect(saveCheckBox, SIGNAL(toggled(bool)), this, SLOT(setSaveKinetics(bool)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
RobotKineticsTab::~RobotKineticsTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void RobotKineticsTab::languageChange()
{
    retranslateUi(this);
}

void RobotKineticsTab::init(Robot *rob) 
{

    robot = rob;
    /*  if(!robot->driving) { // para filmación. quitar
    velLFloatSpinBox->setMaximum(robot->max_vel);
    velLFloatSpinBox->setMinimum(-robot->max_vel);
    velRFloatSpinBox->setMaximum(robot->max_vel);
    velRFloatSpinBox->setMinimum(-robot->max_vel);
  }*/
    setWindowTitle(windowTitle() + " " + robot->fullName());
    setX(robot->getX());
    init_x = x;
    setY(robot->getY());
    init_y = y;
    setRotation(robot->getRotation());
    init_rotation = rotation;

    setRVel(robot->getInitRightVel());
    setLVel(robot->getInitLeftVel());
    //   setRVel(robot->getSprite()->getRVel()*robot->robotScale());
    //   setLVel(robot->getSprite()->getLVel()*robot->robotScale());

    //  setSensorMode(robot->getSensorMode());

}

double RobotKineticsTab::getX( void )
{
    return xFloatSpinBox->value();
}

void RobotKineticsTab::setX( double d)
{
    x = d;
    xFloatSpinBox->setValue(d);
}

double RobotKineticsTab::getY( void )
{
    return yFloatSpinBox->value();
}

void RobotKineticsTab::setY( double d)
{
    y = d;
    yFloatSpinBox->setValue(d);
}

double RobotKineticsTab::getRotation( void )
{
    return rotationFloatSpinBox->value();
}

void RobotKineticsTab::setRotation( double d)
{
    rotation = d;
    if(rotation < -180)
        rotation += 360;
    rotationFloatSpinBox->setValue(d);
}

double RobotKineticsTab::getRVel( void )
{
    return velRFloatSpinBox->value();
}

void RobotKineticsTab::setRVel( double d)
{
    r_vel = d;
    velRFloatSpinBox->setValue(d);

    emit emitExternVal("robot_max_vel", QVariant((fabs(r_vel) + fabs(getLVel()))/2.0));
}

double RobotKineticsTab::getLVel( void )
{
    return velLFloatSpinBox->value();
}

void RobotKineticsTab::setLVel( double d)
{
    l_vel = d;
    velLFloatSpinBox->setValue(d);

    emit emitExternVal("robot_max_vel", QVariant((fabs(getRVel()) + fabs(l_vel))/2.0));
}

//void RobotKineticsTab::setSensorMode( int sm )
//{
//  sensorMode = sm;
//  sensorModeButtonGroup->buttons().at(sn)->setChecked(true);
//}


//int RobotKineticsTab::getSensorMode( void )
//{
//  return sensorMode;
//}

void RobotKineticsTab::saveThis(void)
{
    bool positionChanged = false;
    if(x != init_x) {
        robot->setX(x);
        positionChanged = true;
    }
    if(y != init_y) {
        robot->setY(y);
        positionChanged = positionChanged || true;
    }
    if(rotation != init_rotation) {
        robot->rotation(rotation);
        positionChanged = positionChanged || true;
        robot->robot_sprite->clearPath(true);
    }

    robot->setInitRightVel(getRVel());
    robot->setInitLeftVel(getLVel());
    robot->initMotors();
    //  robot->getSprite()->setRotation(robot->getInitRot());
    robot->getSprite()->setLVel(robot->getInitLeftVel()/
                                robot->robotScale());
    robot->getSprite()->setRVel(robot->getInitRightVel()/
                                robot->robotScale());
    robot->readSensors();
    //  robot->setSensorMode(getSensorMode());
    robot->setSaveKinetics(saveKinetics);

    //   if(robot->driving)
    if(fabs(getRVel() + getLVel()) > 0)
        robot->setMaxVel(fabs(getRVel() + getLVel())/2.0);
    if(positionChanged) {
        robot->getMainMotor()->reset(true);
        robot->getSensorsSet()->rotate();
    }
}

void RobotKineticsTab::setSaveKinetics(bool sk)
{
    saveKinetics = sk;
}

