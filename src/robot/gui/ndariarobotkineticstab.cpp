#include "ndariarobotkineticstab.h"

#include <QVariant>
#include "../ndariarobot.h"

/*
 *  Constructs a NDAriaRobotKineticsTab which is a child of 'parent', with the
 *  name 'name'.' 
 */
NDAriaRobotKineticsTab::NDAriaRobotKineticsTab(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);

    connect(rotationFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setRotation(double)));
    connect(velLFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setLVel(double)));
    connect(velRFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setRVel(double)));
    connect(xFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setX(double)));
    connect(yFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setY(double)));
    connect(saveCheckBox, SIGNAL(toggled(bool)), this, SLOT(setSaveKinetics(bool)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
NDAriaRobotKineticsTab::~NDAriaRobotKineticsTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void NDAriaRobotKineticsTab::languageChange()
{
    retranslateUi(this);
}

void NDAriaRobotKineticsTab::init(NDAriaRobot *rob) {

  robot = rob;
  velLFloatSpinBox->setMaximum(robot->max_vel);
  velLFloatSpinBox->setMinimum(-robot->max_vel);
  velRFloatSpinBox->setMaximum(robot->max_vel);
  velRFloatSpinBox->setMinimum(-robot->max_vel);  
  setWindowTitle(windowTitle() + " " + robot->fullName());
  setX(robot->getX());
  setY(robot->getY());
  setRotation(robot->getRotation());
  setRVel(robot->getSprite()->getRVel()*robot->robotScale());
  setLVel(robot->getSprite()->getLVel()*robot->robotScale());
//  setSensorMode(robot->getSensorMode());

}

double NDAriaRobotKineticsTab::getX( void )
{
  return xFloatSpinBox->value();
}

void NDAriaRobotKineticsTab::setX( double d)
{
  x = d;
  xFloatSpinBox->setValue(d);
}

double NDAriaRobotKineticsTab::getY( void )
{
  return yFloatSpinBox->value();
}

void NDAriaRobotKineticsTab::setY( double d)
{
  y = d;
  yFloatSpinBox->setValue(d);
}

double NDAriaRobotKineticsTab::getRotation( void )
{
  return rotationFloatSpinBox->value();
}

void NDAriaRobotKineticsTab::setRotation( double d)
{
  rotation = d;
  rotationFloatSpinBox->setValue(d);
}

double NDAriaRobotKineticsTab::getRVel( void )
{
  return velRFloatSpinBox->value();
}

void NDAriaRobotKineticsTab::setRVel( double d)
{
  r_vel = d;
  velRFloatSpinBox->setValue(d);

  emit emitExternVal("robot_max_vel", QVariant((fabs(r_vel) + fabs(getLVel()))/2.0));
}

double NDAriaRobotKineticsTab::getLVel( void )
{
  return velLFloatSpinBox->value();
}

void NDAriaRobotKineticsTab::setLVel( double d)
{
  l_vel = d;
  velLFloatSpinBox->setValue(d);
  emit emitExternVal("robot_max_vel", QVariant((fabs(getRVel()) + fabs(l_vel))/2.0));
}


//void NDAriaRobotKineticsTab::setSensorMode( int sm )
//{
//  sensorMode = sm;
//  sensorModeButtonGroup->buttons().at(sm)->setChecked(true);
//}


//int NDAriaRobotKineticsTab::getSensorMode( void )
//{
//  return sensorMode;
//}

void NDAriaRobotKineticsTab::saveThis(void)
{
  
//   robot->gotoPose(getX(), getY(), getRotation());

  robot->setInitRightVel(getRVel());
  robot->setInitLeftVel(getLVel());
  robot->initMotors();
  
/*  robot->setX(getX());
  robot->setY(getY());
  robot->rotation(getRotation());
  robot->setInitRightVel(getRVel());
  robot->setInitLeftVel(getLVel());
  robot->initMotors(); 
  robot->getSprite()->setRotation(robot->getInitRot());
  robot->getSprite()->setLVel(robot->getInitLeftVel()/
  robot->robotScale());
  robot->getSprite()->setRVel(robot->getInitRightVel()/
  robot->robotScale()); */   
//  robot->setSensorMode(getSensorMode());
  robot->setSaveKinetics(saveKinetics);
}

void NDAriaRobotKineticsTab::setSaveKinetics(bool sk)
{
  saveKinetics = sk;
}
