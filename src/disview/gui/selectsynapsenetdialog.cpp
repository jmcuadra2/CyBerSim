#include "selectsynapsenetdialog.h"

#include <QVariant>
#include <QMessageBox>
#include <QButtonGroup>

/*
 *  Constructs a this as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
selectSynapseNetDialog::selectSynapseNetDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    buttonGroupFrom = new QButtonGroup(this);
    buttonGroupFrom->addButton(radioFromInput, 0);
    buttonGroupFrom->addButton(radioFromHidden, 1);
    buttonGroupFrom->addButton(radioFromOutput, 2);

    buttonGroupTo = new QButtonGroup(this);
    buttonGroupTo->addButton(radioToInput, 0);
    buttonGroupTo->addButton(radioToHidden, 1);
    buttonGroupTo->addButton(radioToOutput, 2);

    QObject::connect(buttonEditNeuronCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(buttonGroupFrom, SIGNAL(buttonClicked(int)),
                     this, SLOT(setLayerFrom(int)));
    QObject::connect(buttonGroupFrom, SIGNAL(buttonClicked(int)),
                     this, SLOT(setLayerTo(int)));
    QObject::connect(SpinBoxFrom, SIGNAL(valueChanged(QString)),
                     this, SLOT(getTextFrom(QString)));
    QObject::connect(SpinBoxFrom, SIGNAL(valueChanged(int)),
                     this, SLOT(setNum_IdNeuronFrom(int)));
    QObject::connect(buttonEditNeuronOK, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(radioFromNetwork, SIGNAL(pressed()),
                     this, SLOT(radioFromNetwork_pressed()));
    QObject::connect(radioToNetwork, SIGNAL(pressed()),
                     this, SLOT(radioToNetwork_pressed()));
    QObject::connect(radioFromSubNet, SIGNAL(stateChanged(int)),
                     this, SLOT(updateFromNeurons(int)));
    QObject::connect(radioToSubNet, SIGNAL(stateChanged(int)),
                     this, SLOT(updateToNeurons(int)));
    QObject::connect(radioFromNetwork, SIGNAL(stateChanged(int)),
                     this, SLOT(updateFromCurrentNetwork()));
    QObject::connect(radioToNetwork, SIGNAL(stateChanged(int)),
                     this, SLOT(updateToCurrentNetwork()));
    QObject::connect(SpinBoxFromSubNet, SIGNAL(valueChanged(int)),
                     this, SLOT(updateFromNeurons(int)));
    QObject::connect(SpinBoxToSubNet, SIGNAL(valueChanged(int)),
                     this, SLOT(updateToNeurons(int)));
    QObject::connect(ListBoxTo, SIGNAL(itemClicked(int)),
                     this, SLOT(setNum_IdSynapse(int)));
    QObject::connect(radioFromSubNet, SIGNAL(pressed()),
                     this, SLOT(radioFromSubNet_pressed()));
    QObject::connect(radioToSubNet, SIGNAL(pressed()),
                     this, SLOT(radioToSubNet_pressed()));

}

/*
 *  Destroys the object and frees any allocated resources
 */
selectSynapseNetDialog::~selectSynapseNetDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void selectSynapseNetDialog::languageChange()
{
    retranslateUi(this);
}

int selectSynapseNetDialog::getNum_IdSynapse(void)
{    

    return num_IdSynapse;
}

void selectSynapseNetDialog::setNum_IdSynapse( int tam)
{

    num_IdSynapse = ulong(num_syn[tam]);
    //QMessageBox::information(nullptr,tr("delSynapse"), tr("synapse numero: "+QString::number(num_IdSynapse)),tr("&Ok"));
}

int selectSynapseNetDialog::getNum_IdNeuronFrom(void)
{
    return num_IdNeuronFrom = SpinBoxFrom->cleanText().toInt();
}

bool selectSynapseNetDialog::setNum_IdNeuronFrom( int n_i )
{
    bool ret = false;
    int tam;
    Neuron *neuron_from = nullptr;
    Synapse *synapse ;
    // por si se trata de sinapsis de subred
    SubNet *subnet_from = nullptr;
    SubNetSynapse *subsynapse ;

    if (radioFromSubNet->isChecked()  ) {
        int num = SpinBoxFromSubNet->cleanText().toInt();

//        QMessageBox::information(nullptr, tr("delSynapse"),
//                                 tr("subnet number: ") + QString::number(num), tr("&Ok"));
        QMessageBox::information(nullptr, tr("delSynapse"),
                                 tr("subnet number: ") + QString::number(num), QMessageBox::Ok);
        subnet_from = subnets->at(num-1);
        num_syn.resize(0);
        ListBoxTo->clear();
        tam = -1;

        QList<SubNetSynapse *> Axon = subnet_from->getAxon();
        QListIterator<SubNetSynapse *> its(Axon);
        while(its.hasNext()) {
            subsynapse = its.next();
            tam++;
            QListWidgetItem* item = new QListWidgetItem(QString("%1").
                                        arg(QString::number(subsynapse->getIdNeuronTo()).
                                        rightJustified(6)));
            ListBoxTo->insertItem(tam, item);
            num_syn.resize(tam  + 1);
            num_syn[tam] = subsynapse->getNumId();
        }
        if(tam > -1) {
            ListBoxTo->setModelColumn(3);
            ListBoxTo->setCurrentRow(0);
            ListBoxTo->setEnabled(true);
        }
        else
            ListBoxTo->setEnabled(false);
    }
    else {
        switch(layerFrom) {
        case 0:
            if(n_i > 0 && n_i <= n_in) {
                num_IdNeuronFrom  = n_i;
                neuron_from = InputPtr.at(n_i - 1);
                ret = true;
            }
            else
                SpinBoxFrom->setValue( 1);
            break;
        case 1:
            if(n_i > n_in && n_i <= n_in + n_hid) {
                num_IdNeuronFrom  = n_i;
                neuron_from = HiddenPtr.at(n_i  - n_in- 1);
                ret = true;
            }
            else
                SpinBoxFrom->setValue( n_in + 1);
            break;
        case 2:
            if(n_i > n_in + n_hid && n_i <= n_in + n_hid + n_out) {
                num_IdNeuronFrom  = n_i;
                neuron_from = OutputPtr.at(n_i - n_hid - n_in - 1);
                ret = true;
            }
            else
                SpinBoxFrom->setValue( n_in + n_hid + 1);
            break;
        }

        if(ret) {
            num_syn.resize(0);
            ListBoxTo->clear();
            tam = -1;
            QList<Synapse *> Axon = neuron_from->getAxon();
            QListIterator<Synapse *> its(Axon);
            while(its.hasNext()) {
                synapse = its.next();
                tam++;
                QListWidgetItem* item = new QListWidgetItem(QString("%1").
                                            arg(QString::number(synapse->getTo()).
                                            rightJustified(6)));
                ListBoxTo->insertItem(tam, item);
                num_syn.resize(tam  + 1);
                num_syn[tam] = synapse->getNumId();
            }
            if(tam > -1) {
                ListBoxTo->setModelColumn(3);
                ListBoxTo->setCurrentRow(0);
                ListBoxTo->setEnabled(true);
            }
            else
                ListBoxTo->setEnabled(false);
        }
    }
    return true;
}

int selectSynapseNetDialog::getLayerFrom(void)
{
    return layerFrom;
}

int selectSynapseNetDialog::getLayerTo(void)
{
    return layerTo;
}

void selectSynapseNetDialog::setMinMaxValues( int n_i, int n_h, int n_o )
{
    n_in = n_i;
    n_hid = n_h;
    n_out = n_o;
    if(n_in == 0) {
        radioFromInput->setEnabled(false);
        radioToInput->setEnabled(false);
    }
    if(n_hid == 0) {
        radioFromHidden->setEnabled(false);
        radioToHidden->setEnabled(false);
    }
    if(n_out == 0) {
        radioFromOutput->setEnabled(false);
        radioToOutput->setEnabled(false);
    }
}

void selectSynapseNetDialog::setLayerFrom( int l_ )
{
    layerFrom = l_;
    switch(l_) {
    case 0:
        SpinBoxFrom->setRange(1, n_in );
        SpinBoxFrom->setValue(1);
        SpinBoxFrom->setEnabled(true);
        break;
    case 1:
        SpinBoxFrom->setRange(n_in + 1, n_in+ n_hid);
        SpinBoxFrom->setValue(n_in + 1);
        SpinBoxFrom->setEnabled(true);
        break;
    case 2:
        SpinBoxFrom->setRange(n_in + n_hid + 1, n_in + n_hid + n_out );
        SpinBoxFrom->setValue(n_in + n_hid + 1);
        SpinBoxFrom->setEnabled(true);
        break;
    }
}

void selectSynapseNetDialog::setLayerTo(int l_)
{
    // jose 23/10/2016 revisar la cuestion de subnets
    // se han colocado valores de la subnet de entrada n_in, n_hid, n_out
    // habria que hacerlo con valres de la subnet de salida que no estan definidos
    layerTo = l_;
    switch(l_) {
    case 0:
        SpinBoxTo->setRange(1, n_in);
        SpinBoxTo->setValue(1);
        SpinBoxTo->setEnabled(true);
        break;
    case 1:
        SpinBoxTo->setRange(n_in + 1, n_in+ n_hid);
        SpinBoxTo->setValue(n_in + 1);
        SpinBoxTo->setEnabled(true);
        break;
    case 2:
        SpinBoxTo->setRange(n_in + n_hid + 1, n_in + n_hid + n_out );
        SpinBoxTo->setValue(n_in + n_hid + 1);
        SpinBoxTo->setEnabled(true);
        break;
    }
}

void selectSynapseNetDialog::getTextFrom( const QString& /*texto*/ )
{  
}

void selectSynapseNetDialog::getTextTo( const QString& /*texto*/ )
{  
}

void selectSynapseNetDialog::accept(  )
{
    QString textoF = SpinBoxFrom->cleanText();
    QString textoNF=SpinBoxFromSubNet->cleanText() ;
    QString textoNT=SpinBoxToSubNet->cleanText();
    if(setNum_IdNeuronFrom(textoF.toInt()) || setNum_IdNetTo(textoNT.toInt())||
            setNum_IdNetFrom(textoNF.toInt()))
        QDialog::accept();
    else
//        QMessageBox::information(nullptr,tr("delSynapse"), tr("Neuron/s... out of range"),tr("&Ok"));
        QMessageBox::information(nullptr,tr("delSynapse"), tr("Neuron/s... out of range"), QMessageBox::Ok);
}

void selectSynapseNetDialog::radioFromSubNet_pressed()
{
    // se va a seleccionar una neurona de subnet
    SpinBoxFromSubNet->setEnabled(true);
    radioFromNetwork->setChecked(false);
    SpinBoxFrom->setEnabled(false);
    ListBoxTo->clear();
    
}

void selectSynapseNetDialog::radioFromNetwork_pressed()
{
    // se va a seleccionar una neurona de la red principal
    SpinBoxFromSubNet->setEnabled(false);
    radioFromSubNet->setChecked(false);
    // cargar el numero de neuronas de cada capa para la red principal
    setMinMaxValues (current_ni , current_nh , current_no );

}

void selectSynapseNetDialog::radioToSubNet_pressed()
{
    SpinBoxToSubNet->setEnabled(true);
    radioToNetwork->setChecked(false);
    ListBoxTo->clear();
}

void selectSynapseNetDialog::radioToNetwork_pressed()
{

    SpinBoxToSubNet->setEnabled(false);
    radioToSubNet->setChecked(false);
}

bool selectSynapseNetDialog::setNum_IdNetFrom(int i)
{

    bool ret = false;
    if(i > 0) {
        num_IdNetFrom  = i;
        ret = true;
    }

    return ret;
}

bool selectSynapseNetDialog::setNum_IdNetTo( int i )
{
    bool ret = false;
    if(i > 0) {
        num_IdNetTo  = i;
        ret = true;
    }

    return ret;
}

int selectSynapseNetDialog::getNum_IdNetFrom( void )
{

    return  num_IdNetFrom = SpinBoxFromSubNet->cleanText().toInt();

}

int selectSynapseNetDialog::getNum_IdNetTo()
{
    return num_IdNetTo = SpinBoxToSubNet->cleanText().toInt();
}

void selectSynapseNetDialog::setSubNets( QList<SubNet *> *subnets )
{
    this->subnets = subnets;
    
    // se actualiza el numero de subnets de los desplegables SubNet
    if (subnets->count() > 0)
    {
        SpinBoxFromSubNet->setRange(1, subnets->count());
        SpinBoxToSubNet->setRange(1, subnets->count());
        radioFromSubNet->setEnabled(true);
        radioToSubNet->setEnabled(true);
    }
    else
    {
        // no hay subnets en la estructura principal
        radioFromSubNet->setEnabled(false);
        radioToSubNet->setEnabled(false);
        SpinBoxFromSubNet->setEnabled(false);
        SpinBoxToSubNet->setEnabled(false);
    }
    
}

void selectSynapseNetDialog::updateFromNeurons( int /*n_*/)
{
    // se ha elegido una nueva subnet para FROM
    // este método debe cambiarse y cargar las neuronas a las que están conectadas
    // mediante sinapsis a las subredes para borrar o editar
    // hay que recargar los valores min-max para 'From layer'
    SubNet  *subnet = 0;

    if(radioFromSubNet->isChecked()) {
        int num = SpinBoxFromSubNet->cleanText().toInt();
        subnet = subnets->at(num-1);

        if (subnet) {
            // cargar el numero de neuronas de cada capa para esta subred
            setMinMaxValues (subnet->getNumInputNeurons(),
                             subnet->getNumHiddenNeurons(),
                             subnet->getNumOutputNeurons());
            radioFromInput->setChecked(true);
            radioFromHidden->setChecked(false);
            radioFromOutput->setChecked(false);

        }
        else {
            // esa subnet no es valida
//            QMessageBox::information(nullptr,tr("delSynapse"), tr("Neuron from ... out of range"),tr("&Ok"));
            QMessageBox::information(nullptr,tr("delSynapse"), tr("Neuron from ... out of range"), QMessageBox::Ok);
        }
    }

}

void selectSynapseNetDialog::setCurrentMinMaxValues( int ni, int nh, int no )
{
    // se cargan los numeros de neuronas de cada capa (de la red principal)
    this->current_ni = ni;
    this->current_nh = nh;
    this->current_no = no;
    
    setMinMaxValues( ni, nh, no );
}


void selectSynapseNetDialog::updateToNeurons( int /*n_*/)
{
    // este método debe cambiarse y cargar las neuronas a las que están conectadas
    // mediante sinapsis para borrar o editar
    // se ha elegido una nueva subnet para TO
    // hay que recargar los valores min-max para 'To layer'
    SubNet  *subnet = 0;

    if(radioToSubNet->isChecked()) {
        int num = SpinBoxToSubNet->cleanText().toInt();
        subnet = subnets->at(num-1);

        if (subnet) {
            // cargar el numero de neuronas de cada capa para esta subred
            setMinMaxValues (subnet->getNumInputNeurons(),
                             subnet->getNumHiddenNeurons(),
                             subnet->getNumOutputNeurons());
            radioToInput->setChecked(true);
            radioToHidden->setChecked(false);
            radioToOutput->setChecked(false);
        }
        else {
            // esa subnet no es valida
//            QMessageBox::information(nullptr,tr("delSynapse"), tr("Neuron from ... out of range"),tr("&Ok"));
            QMessageBox::information(nullptr,tr("delSynapse"), tr("Neuron from ... out of range"), QMessageBox::Ok);
        }
    }

}

void selectSynapseNetDialog::updateFromCurrentNetwork()
{
    // llamo a la inicialización de valores de las capas de la red principal
    setLayerFrom(2);
    setLayerFrom(1);
    setLayerFrom(0);
    radioFromInput->setChecked(true);
    radioFromHidden->setChecked(false);
    radioFromOutput->setChecked(false);

}

void selectSynapseNetDialog::updateToCurrentNetwork()
{
    //setLayerTo(2);
    //setLayerTo(1);
    //setLayerTo(0);
    radioToInput->setChecked(true);
    radioToHidden->setChecked(false);
    radioToOutput->setChecked(false);

}

// nuevo método
void selectSynapseNetDialog::setMinMaxValues(int n_i, int n_h, int n_o,
                                             QList<Neuron *> In_Ptr, QList<Neuron *> Hid_Ptr,
                                             QList<Neuron *> Out_Ptr)
{
    n_in = n_i;
    n_hid = n_h;
    n_out = n_o;
    InputPtr = In_Ptr;
    HiddenPtr = Hid_Ptr;
    OutputPtr = Out_Ptr;
    
    if(n_in == 0)
        radioFromInput->setEnabled(false);
    if(n_hid == 0)
        radioFromHidden->setEnabled(false);
    if(n_out == 0)
        radioFromOutput->setEnabled(false);
}
