//
// C++ Interface: ISOcontroltab
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ISOcontroltab.h"

#include <QVariant>
#include <QMessageBox>
#include <QButtonGroup>

#include "../netsimcontrol.h"

/*
 *  Constructs a ISOControlTab which is a child of 'parent', with the
 *  name 'name'.'
 */
ISOControlTab::ISOControlTab(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);

    propButtonGroup = new QButtonGroup(this);
    propButtonGroup->addButton(seqRadioButton, 0);
    propButtonGroup->addButton(instRadioButton, 1);

    weightStabButtonGroup = new QButtonGroup(this);
    weightStabButtonGroup->addButton(asymRadioButton, 0);
    weightStabButtonGroup->addButton(noneRadioButton, 1);
    weightStabButtonGroup->addButton(symRadioButton, 2);

    QObject::connect(learnCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(setLearnRateOver(bool)));
    QObject::connect(learnFloatSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setLearnRate(double)));
    QObject::connect(weightStabButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(setWeightStabMode(int)));
    QObject::connect(weightStabFloatSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setWeightStabValue(double)));
    QObject::connect(posWeightsCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(setPosWeights(bool)));
    QObject::connect(propButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(setInstantPropagation(int)));
    QObject::connect(initwgFloatSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setInitWeightsValue(double)));
    QObject::connect(initwgCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(setInitWeightsMode(bool)));
    QObject::connect(saveFileButton, SIGNAL(clicked()),
                     this, SLOT(saveToFile()));
    QObject::connect(floatSpinBoxDecay, SIGNAL(valueChanged(double)),
                     this, SLOT(setWeightDecay(double)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
ISOControlTab::~ISOControlTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ISOControlTab::languageChange()
{
    retranslateUi(this);
}

/*! \class ISOControlTab
 *   \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */

void ISOControlTab::init(NetSimControl * ctrl) {
    control = ctrl;
    learnFloatSpinBox->setDecimals(6);
    weightStabFloatSpinBox->setDecimals(3);
    setLearnRateOver(control->getCommonLearnMode());
    setLearnRate(control->getCommonLearnRate());
    setWeightStabMode(control->getWeightStabMode());
    setWeightStabValue(control->getWeightStabVal());
    setPosWeights(control->getPositiveWeights());
    setInitWeightsMode(control->getInitWeightsMode());
    setInitWeightsValue(control->getInitWeightsVal());
    setInstantPropagation(control->getInstantPropagation());
    setWeightDecay(control->getLearningDecay());
}

bool ISOControlTab::validateThis(void) {
    if(weightStabValue == 0.0) {
        weightStabMode = 0;
        weightStabButtonGroup->buttons().at(0)->setChecked(true);
    }
    return true;
}

void  ISOControlTab::saveThis(void)
{    
    control->setCommonLearnMode(getLearnRateOver());
    control->setCommonLearnRate(getLearnRate());
    control->setWeightStabMode(getWeightStabMode());
    control->setWeightStabVal(getWeightStabValue());
    control->setPositiveWeights(getPosWeights());
    control->setInitWeightsMode(getInitWeightsMode());
    control->setInitWeightsVal(getInitWeightsValue());
    control->setInstantPropagation(getInstantPropagation());
    control->setLearningDecay(getWeightDecay());
    //    control->setSaveValues(saveVals);
}

void ISOControlTab::setWeightStabValue( double d )
{
    weightStabValue = d;
    weightStabFloatSpinBox->setValue(d);
}

double ISOControlTab::getWeightStabValue(void)
{
    return weightStabFloatSpinBox->value();
}

void ISOControlTab::setWeightStabMode( int ws )
{
    weightStabMode = ws;
    weightStabButtonGroup->buttons().at(ws)->setChecked(true);
}

int ISOControlTab::getWeightStabMode(void)
{
    return weightStabMode;
}

void ISOControlTab::setLearnRate( double d )
{
    learnRateValue = d;
    learnFloatSpinBox->setValue(d);
}

double ISOControlTab::getLearnRate( void )
{
    return learnFloatSpinBox->value();
}

void ISOControlTab::setLearnRateOver( bool b )
{
    learnRateOver = b;
    learnCheckBox->setChecked(b);
}

bool ISOControlTab::getLearnRateOver( void )
{
    return learnRateOver;
}

bool ISOControlTab::getPosWeights( void )
{
    return pos_weights;
}

void ISOControlTab::setPosWeights( bool ps )
{
    pos_weights = ps;
    posWeightsCheckBox->setChecked(ps);
}

void ISOControlTab::setInitWeightsValue( double d )
{
    initWeightsValue = d;
    initwgFloatSpinBox->setValue(d);
}

double ISOControlTab::getInitWeightsValue(void)
{
    return initwgFloatSpinBox->value();
}

void ISOControlTab::setInitWeightsMode( bool b )
{
    initWeightsMode = b;
    initwgCheckBox->setChecked(b);
}

bool ISOControlTab::getInitWeightsMode( void )
{
    return initWeightsMode;
}

void ISOControlTab::setInstantPropagation( int inst_prop )
{
    instant_prop = inst_prop;
    propButtonGroup->buttons().at(inst_prop)->setChecked(true);
}

int ISOControlTab::getInstantPropagation( void )
{
    return instant_prop;
}

void ISOControlTab::cleanThis(void)
{ 
}

void ISOControlTab:: saveToFile(void)
{
    if(validateThis()) {
//        if(QMessageBox::information(nullptr, tr("Saving control parameters"),
//                                    tr("This operation cannot be cancelled later"),
//                                    tr("&Continue"), tr("Abort")) == 1)
        if(QMessageBox::information(nullptr, tr("Saving control parameters"),
                                     tr("This operation cannot be cancelled later"),
                                     QMessageBox::Save | QMessageBox::Abort) == QMessageBox::Abort)
            return;
        saveThis();
        control->save();
    }

}

void ISOControlTab::setWeightDecay( double w_dec )
{
    w_decay = w_dec;
    floatSpinBoxDecay->setValue(w_decay);
}

double ISOControlTab::getWeightDecay(void)
{
    return floatSpinBoxDecay->value();
}
