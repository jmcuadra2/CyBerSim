#include "editneurondialog.h"

#include <QVariant>
#include <QMessageBox>
#include <QButtonGroup>

/*
 *  Constructs a editNeuronDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
editNeuronDialog::editNeuronDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    buttonGroupLayer = new QButtonGroup(this);
    buttonGroupLayer->addButton(radioLayerInput, 0);
    buttonGroupLayer->addButton(radioLayerHidden, 1);
    buttonGroupLayer->addButton(radioLayerOutput, 2);

    QObject::connect(buttonEditNeuronCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(buttonGroupLayer, SIGNAL(buttonClicked(int)), this, SLOT(setLayer(int)));
    QObject::connect(SpinBox1, SIGNAL(valueChanged(QString)), this, SLOT(getText(QString)));
    QObject::connect(SpinBox1, SIGNAL(valueChanged(int)), this, SLOT(setNum_IdNeuron(int)));
    QObject::connect(buttonEditNeuronOK, SIGNAL(clicked()), this, SLOT(accept()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
editNeuronDialog::~editNeuronDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void editNeuronDialog::languageChange()
{
    retranslateUi(this);
}

int editNeuronDialog::getNum_IdNeuron(void)
{
    return num_IdNeuron = SpinBox1->cleanText().toInt();
}

bool editNeuronDialog::setNum_IdNeuron( int n_i )
{
     bool ret = false;
     switch(layer) {
         case 0:
  if(n_i > 0 && n_i <= n_in) {
         num_IdNeuron  = n_i;
         ret = true;
     }
  else 
      SpinBox1->setValue( 1);
  
 break;
         case 1:
 if(n_i > n_in && n_i <= n_in + n_hid) {
         num_IdNeuron  = n_i; 
         ret = true;
     }
  else 
      SpinBox1->setValue( n_in + 1);

 break;
         case 2:
 if(n_i > n_in + n_hid && n_i <= n_in + n_hid + n_out) {
         num_IdNeuron  = n_i; 
         ret = true;
     }
  else 
      SpinBox1->setValue( n_in + 1);

 break;
    }
    return ret;
}

int editNeuronDialog::getLayer(void)
{
    return layer;
}

void editNeuronDialog::setMinMaxValues( int n_i, int n_h, int n_o )
{
    n_in = n_i;
    n_hid = n_h;
    n_out = n_o;
    if(n_in == 0) 
 radioLayerInput->setEnabled(false);
    if(n_hid == 0) 
 radioLayerHidden->setEnabled(false);
    if(n_out == 0) 
 radioLayerOutput->setEnabled(false);
}

void editNeuronDialog::setLayer( int l_ )
{
    layer = l_;
     switch(layer) {
      case 0:
 SpinBox1->setRange(1, n_in );
 SpinBox1->setValue(1);
 break;
      case 1:
 SpinBox1->setRange(n_in, n_in+ n_hid);
 SpinBox1->setValue(n_in);
 break;
      case 2:
 SpinBox1->setRange(n_in + n_hid, n_in + n_hid + n_out );
 SpinBox1->setValue(n_in + n_hid);
 break;
    }
}

void editNeuronDialog::getText( const QString& texto )
{  
    if(!setNum_IdNeuron(texto.toInt()))
//        QMessageBox::information(nullptr,tr("editNeuron"), tr("Neuron out of range"),tr("&Ok"));
    QMessageBox::information(nullptr,tr("editNeuron"), tr("Neuron out of range"), QMessageBox::Ok);
}

void editNeuronDialog::accept(  )
{
    QString texto = SpinBox1->cleanText();
    if(setNum_IdNeuron(texto.toInt()))
        QDialog::accept();
    else
//         QMessageBox::information(nullptr,tr("editNeuron"), tr("Neuron out of range"),tr("&Ok"));
             QMessageBox::information(nullptr,tr("editNeuron"), tr("Neuron out of range"), QMessageBox::Ok);
}
