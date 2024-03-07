#include "totsimulrobotkineticstab.h"

#include <QVariant>
#include <QButtonGroup>
#include "../totallysimulatedrobot.h"
#include "../differentialmotor.h"

/*
 *  Constructs a TotSimulRobotKineticsTab which is a child of 'parent', with the
 *  name 'name'.'
 */
TotSimulRobotKineticsTab::TotSimulRobotKineticsTab(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);

    sensorModeButtonGroup = new QButtonGroup(this);
    sensorModeButtonGroup->addButton(khep1RadioButton, 0);
    sensorModeButtonGroup->addButton(khep2RadioButton, 1);

    QObject::connect(sensorModeButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(setSensorMode(int)));
    QObject::connect(noiseSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setNoise(int)));
    QObject::connect(inerListBox, SIGNAL(currentRowChanged(int)), this, SLOT(setIner(int)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
TotSimulRobotKineticsTab::~TotSimulRobotKineticsTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void TotSimulRobotKineticsTab::languageChange()
{
    retranslateUi(this);
}

void TotSimulRobotKineticsTab::init(TotallySimulatedRobot *rob) {

    robot = rob;
    setSensorMode(robot->getSensorMode());
    setIner(robot->getTypeIner());
    setNoise(robot->getNoise());

}

void TotSimulRobotKineticsTab::setSensorMode( int sm )
{
    sensorMode = sm;
    sensorModeButtonGroup->buttons().at(sm)->setChecked(true);
}

int TotSimulRobotKineticsTab::getIner( void )
{
    return iner;
}

void TotSimulRobotKineticsTab::setIner(int i)
{
    iner = i;
    inerListBox->setCurrentRow(i);
}

int TotSimulRobotKineticsTab::getNoise( void )
{
    return noise;
}

void TotSimulRobotKineticsTab::saveThis(void)
{
    robot->setNoise(getNoise());
    robot->setTypeIner(getIner());
    robot->setSensorMode(sensorMode);
    DifferentialMotor* motor = dynamic_cast<DifferentialMotor*>(robot->getMainMotor());
    motor->setModel(noise, noise * 10, iner, DifferentialMotor::GAUSS);
    //     motor->setModel(noise, noise, iner, DifferentialMotor::UNIFORM);
}

void TotSimulRobotKineticsTab::setNoise( int n)
{
    noise = n;
    noiseSpinBox->setValue(n);
}
int TotSimulRobotKineticsTab::getSensorMode( void )
{
    return sensorMode;
}
