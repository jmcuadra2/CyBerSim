#include "addneurondialog.h"

#include <QVariant>
#include <QMessageBox>
#include <QButtonGroup>
#include <iostream>

/*
 *  Constructs a addNeuronDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */

addNeuronDialog::addNeuronDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);
    
    init();

    buttonGroupLayer = new QButtonGroup(this);
    buttonGroupLayer->addButton(radioLayerHidden, 0);
    buttonGroupLayer->addButton(radioLayerInput, 1);
    buttonGroupLayer->addButton(radioLayerOutput, 2);

    QObject::connect(buttonAddNeuronCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(buttonGroupLayer, SIGNAL(buttonClicked(int)), this, SLOT(setLayer(int)));
    QObject::connect(SpinBox1, SIGNAL(valueChanged(int)), this, SLOT(setNum_IdNeuron(int)));
    QObject::connect(devListBox, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setDevice(QListWidgetItem*)));
    QObject::connect(buttonAddNeuronOK, SIGNAL(clicked()), this, SLOT(accept()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
addNeuronDialog::~addNeuronDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void addNeuronDialog::languageChange()
{
    retranslateUi(this);
}

void addNeuronDialog::init()
{
   NeuronViewFactory* neuronViewFactory = NeuronViewFactory::instance();  
   names_map = neuronViewFactory ->getNamesMap(); 
   NameClass_Map::Iterator it;
   devListBox->clear();
   device = -1;
   int i = 0;
   for (it =  names_map.begin(); it !=  names_map.end(); ++it) {
       devListBox->insertItem(i, it.key());
       i++;
   }
}

int addNeuronDialog::getNum_IdNeuron(void)
{
    return num_IdNeuron = SpinBox1->cleanText().toInt();
}

bool addNeuronDialog::setNum_IdNeuron( int n_i )
{
     bool ret = false;
     switch(layer) {
         case 0:
  if(n_i > 0 && n_i <= n_in+1) {
         num_IdNeuron  = n_i;
         ret = true;
     }
  else 
      SpinBox1->setValue( 1);
  
 break;
         case 1:
 if(n_i > n_in && n_i <= n_in + n_hid+1) {
         num_IdNeuron  = n_i; 
         ret = true;
     }
  else 
      SpinBox1->setValue( n_in + 1);

 break;
         case 2:
 if(n_i > n_in + n_hid && n_i <= n_in + n_hid + n_out+1) {
         num_IdNeuron  = n_i; 
         ret = true;
     }
  else 
      SpinBox1->setValue( n_in + 1);

 break;
    }
    return ret;
}

int addNeuronDialog::getLayer(void)
{
    return layer;
}

void addNeuronDialog::setMinMaxValues( int n_i, int n_h, int n_o )
{
    n_in = n_i;
    n_hid = n_h;
    n_out = n_o;
} 

void addNeuronDialog::setLayer( int l_ )
{
    layer = l_;
    switch(layer) {
    case 0:
        SpinBox1->setRange(1, n_in + 1);
        SpinBox1->setValue(1);
        break;
    case 1:
        SpinBox1->setRange(n_in + 1, n_in+ n_hid + 1);
        SpinBox1->setValue(n_in + 1);
        break;
    case 2:
        SpinBox1->setRange(n_in + n_hid + 1, n_in + n_hid + n_out + 1);
        SpinBox1->setValue(n_in + n_hid + 1);
        break;
    }
}

int addNeuronDialog::getDevice( void )
{
    return device;
}

void addNeuronDialog::setDevice( int d)
{
    NameClass_Map::Iterator it;
    int i = 0;
    for (it = names_map.begin(); it != names_map.end(); ++it) {
        if(it.value() == d) {
          device = d;
//          devListBox->setCurrentItem(i);
          devListBox->setCurrentRow(i);
          break;
        }
        ++i;
    }    
    
}

void addNeuronDialog::setDevice( QListWidgetItem *item )
{    
    device =  names_map[item->text()];
}

void addNeuronDialog::accept(  )
{
    QString texto = SpinBox1->text();
    if(setNum_IdNeuron(texto.toInt()))
      QDialog::accept();
    else
//         QMessageBox::information(nullptr,tr("addNeuron"), tr("Neuron out of range"),tr("&Ok"));
    QMessageBox::information(nullptr,tr("addNeuron"), tr("Neuron out of range"), QMessageBox::Ok);
}
