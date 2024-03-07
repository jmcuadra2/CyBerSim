#include "editinputneurontab.h"
#include "../../net/inputneuron.h"

#include <QVariant>

/*
 *  Constructs a EditInputNeuronTab which is a child of 'parent', with the
 *  name 'name'.' 
 */

EditInputNeuronTab::EditInputNeuronTab(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);
    QObject::connect(identSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setIdentification(int)));
    QObject::connect(groupComboBox, SIGNAL(activated (const QString &)), this, SLOT(setGroupID(const QString &)));
    QObject::connect(groupComboBox, SIGNAL(editTextChanged (const QString &)), this, SLOT(setGroupID(const QString &)));
            
    sensorGroupID = tr("Unknown");
}

/*
 *  Destroys the object and frees any allocated resources
 */
EditInputNeuronTab::~EditInputNeuronTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void EditInputNeuronTab::languageChange()
{
    retranslateUi(this);
}

void EditInputNeuronTab::init(InputNeuron *in_neuron)
{
  input_neuron = in_neuron;
  setIdentification(input_neuron->getInputNodeId());
  setGroupID(input_neuron->getSensorGroupID());
  if(in_neuron->getInputNodeId() > -1) {
    labelConnection->setText(tr("Connected"));
    labelConnection->setEnabled(true);
  }
}

void EditInputNeuronTab::setIdentification(int id)
{
  ident = id;
  identSpinBox->setValue(ident);
}

int EditInputNeuronTab::getIdentification(void)
{
  ident = identSpinBox->value();
  return ident;
}

void EditInputNeuronTab::setGroupID(const QString& group )
{
  if(sensorGroupID != group.simplified ()) {
    sensorGroupID = group.simplified ();
    int i = groupComboBox->findText(sensorGroupID, Qt::MatchFixedString);
    if(i > -1)
      groupComboBox->setCurrentIndex(i);
    else  
      groupComboBox->setEditText(sensorGroupID); 
  }
}

const QString& EditInputNeuronTab::getGroupID( void )
{
  return sensorGroupID;
}

void EditInputNeuronTab::saveThis(void)
{
  input_neuron->setInputNodeId(identSpinBox->value());
  input_neuron->setSensorGroupID(getGroupID());
}
