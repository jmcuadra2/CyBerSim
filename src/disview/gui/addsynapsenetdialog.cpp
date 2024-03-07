#include "addsynapsenetdialog.h"

#include <QVariant>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QButtonGroup>

/*
 *  Constructs a addSynapseNetDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
addSynapseNetDialog::addSynapseNetDialog(QWidget* parent, const char* name, bool modal,
                                         Qt::WindowFlags fl) : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    init();

    buttonGroupFrom = new QButtonGroup(this);
    buttonGroupFrom->addButton(radioFromInput, 0);
    buttonGroupFrom->addButton(radioFromHidden, 1);
    buttonGroupFrom->addButton(radioFromOutput, 2);

    buttonGroupTo = new QButtonGroup(this);
    buttonGroupTo->addButton(radioToInput, 0);
    buttonGroupTo->addButton(radioToHidden, 1);
    buttonGroupTo->addButton(radioToOutput, 2);

    QObject::connect(cancelButton, SIGNAL(clicked()),
                     this, SLOT(reject()));
    QObject::connect(SpinBoxTo, SIGNAL(valueChanged(int)),
                     this, SLOT(setNum_IdNeuronTo(int)));
    QObject::connect(buttonGroupFrom, SIGNAL(buttonClicked(int)),
                     this, SLOT(setLayerFrom(int)));
    QObject::connect(buttonGroupTo, SIGNAL(buttonClicked(int)),
                     this, SLOT(setLayerTo(int)));
    QObject::connect(SpinBoxFrom, SIGNAL(valueChanged(int)),
                     this, SLOT(setNum_IdNeuronFrom(int)));
    QObject::connect(radioFromNetView, SIGNAL(pressed()),
                     this, SLOT(radioFromNetView_pressed()));
    QObject::connect(radioFromNetwork, SIGNAL(pressed()),
                     this, SLOT(radioFromNetwork_pressed()));
    QObject::connect(radioToNetView, SIGNAL(pressed()),
                     this, SLOT(radioToNetView_pressed()));
    QObject::connect(radioToNetwork, SIGNAL(pressed()),
                     this, SLOT(radioToNetwork_pressed()));
    QObject::connect(radioFromNetView, SIGNAL(stateChanged(int)),
                     this, SLOT(updateFromNeurons(int)));
    QObject::connect(radioToNetView, SIGNAL(stateChanged(int)),
                     this, SLOT(updateToNeurons(int)));
    QObject::connect(radioFromNetwork, SIGNAL(stateChanged(int)),
                     this, SLOT(updateFromCurrentNetwork()));
    QObject::connect(radioToNetwork, SIGNAL(stateChanged(int)),
                     this, SLOT(updateToCurrentNetwork()));
    QObject::connect(SpinBoxFromNetView, SIGNAL(valueChanged(int)),
                     this, SLOT(updateFromNeurons(int)));
    QObject::connect(SpinBoxToNetView, SIGNAL(valueChanged(int)),
                     this, SLOT(updateToNeurons(int)));
    QObject::connect(synListBox, SIGNAL(itemClicked(QListWidgetItem*)),
                     this, SLOT(setType(QListWidgetItem*)));
    QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

}

/*
 *  Destroys the object and frees any allocated resources
 */
addSynapseNetDialog::~addSynapseNetDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void addSynapseNetDialog::languageChange()
{
    retranslateUi(this);
}

void addSynapseNetDialog::init()
{
    SynapseViewFactory* synapseViewFactory = SynapseViewFactory::instance();
    names_map = synapseViewFactory ->getNamesMap();
    NameClass_Map::Iterator it;
    synListBox->clear();
    type = -1;
    int i = 0;
    for(it =  names_map.begin(); it !=  names_map.end(); ++it) {
        synListBox->insertItem(i, it.key());
        i++;
    }
    num_IdNeuronFrom = 0;
    num_IdNeuronTo = 0;

}

int addSynapseNetDialog::getNum_IdNeuronFrom(void)
{
    return num_IdNeuronFrom = SpinBoxFrom->cleanText().toInt();
}

int addSynapseNetDialog::getNum_IdNeuronTo(void)
{
    return num_IdNeuronTo = SpinBoxTo->cleanText().toInt();
}

bool addSynapseNetDialog::setNum_IdNeuronFrom(int n_i)
{
    bool ret = false;
    switch(layerFrom) {
    case 0:
        if(n_i > 0 && n_i <= current_ni) {
            num_IdNeuronFrom  = n_i;
            ret = true;
        }
        else
            SpinBoxFrom->setValue(1);

        break;
    case 1:
        if(n_i > current_ni && n_i <= current_ni + current_nh) {
            num_IdNeuronFrom  = n_i;
            ret = true;
        }
        else
            SpinBoxFrom->setValue(current_ni + 1);

        break;
    case 2:
        if(n_i > current_ni + current_nh && n_i <= current_ni + current_nh + current_no) {
            num_IdNeuronFrom  = n_i;
            ret = true;
        }
        else
            SpinBoxFrom->setValue(current_ni + current_nh + 1);

        break;
    }
    if(ret  && num_IdNeuronTo) {
        synListBox->setEnabled(true);
        setType(Synapse::FIXED);
    }
    return ret;
}

bool addSynapseNetDialog::setNum_IdNeuronTo(int n_i)
{

    bool ret = false;
    switch(layerTo) {
    case 0:
        if(n_i > 0 && n_i <= current_ni) {
            num_IdNeuronTo  = n_i;
            ret = true;
        }
        else
            SpinBoxTo->setValue(1);
        break;
    case 1:
        if(n_i > current_ni && n_i <= current_ni + current_nh) {
            num_IdNeuronTo  = n_i;
            ret = true;
        }
        else
            SpinBoxTo->setValue(current_ni + 1);
        break;
    case 2:
        if(n_i > current_ni + current_nh && n_i <= current_ni + current_nh + current_no) {
            num_IdNeuronTo  = n_i;
            ret = true;
        }
        else
            SpinBoxTo->setValue(current_ni +current_nh + 1);
        break;
    }
    if(ret  && num_IdNeuronFrom) {
        synListBox->setEnabled(true);
        setType(Synapse::FIXED);
    }
    return ret;

}

int addSynapseNetDialog::getLayerFrom(void)
{
    return layerFrom;
}

int addSynapseNetDialog::getLayerTo(void)
{
    return layerTo;
}

int addSynapseNetDialog::getType(void)
{
    return type;
}

void addSynapseNetDialog::setType(int d)
{
    NameClass_Map::Iterator it;
    int i = 0;
    for (it = names_map.begin(); it != names_map.end(); ++it) {
        if(it.value() == d) {
            type = d;
            synListBox->setCurrentItem(synListBox->item(i));
            break;
        }
        ++i;
    }
    
}

void addSynapseNetDialog::setType(QListWidgetItem *item)
{    
    type =  names_map[item->text()];
}



void addSynapseNetDialog::setMinMaxValuesFrom(int n_i, int n_h, int n_o)
{
    objectFrom_ni = n_i;
    objectFrom_nh = n_h;
    objectFrom_no = n_o;
}

void addSynapseNetDialog::setMinMaxValuesTo(int n_i, int n_h, int n_o)
{
    objectTo_ni = n_i;
    objectTo_nh = n_h;
    objectTo_no = n_o;
}



void addSynapseNetDialog::setLayerFrom(int l_)
{
    layerFrom = l_;
    switch(l_) {
    case 0:
        SpinBoxFrom->setRange(1, objectFrom_ni);
        SpinBoxFrom->setValue(1);
        SpinBoxFrom->setEnabled(true);
        break;
    case 1:
        SpinBoxFrom->setRange(objectFrom_ni + 1, objectFrom_ni+ objectFrom_nh);
        SpinBoxFrom->setValue(objectFrom_ni + 1);
        SpinBoxFrom->setEnabled(true);
        break;
    case 2:
        SpinBoxFrom->setRange(objectFrom_ni + objectFrom_nh + 1,
                              objectFrom_ni + objectFrom_nh + objectFrom_no);
        SpinBoxFrom->setValue(objectFrom_ni + objectFrom_nh + 1);
        SpinBoxFrom->setEnabled(true);
        break;
    }
}

void addSynapseNetDialog::setLayerTo(int l_)
{
    layerTo = l_;
    switch(l_) {
    case 0:
        SpinBoxTo->setRange(1, objectTo_ni);
        SpinBoxTo->setValue(1);
        SpinBoxTo->setEnabled(true);
        break;
    case 1:
        SpinBoxTo->setRange(objectTo_ni + 1, objectTo_ni+ objectTo_nh);
        SpinBoxTo->setValue(objectTo_ni + 1);
        SpinBoxTo->setEnabled(true);
        break;
    case 2:
        SpinBoxTo->setRange(objectTo_ni + objectTo_nh + 1,
                            objectTo_ni + objectTo_nh + objectTo_no);
        SpinBoxTo->setValue(objectTo_ni + objectTo_nh + 1);
        SpinBoxTo->setEnabled(true);
        break;
    }
}

void addSynapseNetDialog::getTextFrom(const QString& texto)
{  
    if(!setNum_IdNeuronFrom(texto.toInt()))
        QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron from ... out of range"),
                                 QMessageBox::Ok);
}

void addSynapseNetDialog::getTextTo(const QString& texto)
{  
    if(!setNum_IdNeuronTo(texto.toInt()))
//        QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron to ... out of range"),
//                                 tr("&Ok"));
        QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron to ... out of range"),
                                 QMessageBox::Ok);
}

void addSynapseNetDialog::accept()
{
    QString textoF = SpinBoxFrom->cleanText();
    QString textoT = SpinBoxTo->cleanText();
    QString textoNF=SpinBoxFromNetView->cleanText() ;
    QString textoNT=SpinBoxToNetView->cleanText();

    bool control =setNum_IdNeuronFrom(textoF.toInt()) &&
            setNum_IdNeuronTo(textoT.toInt());
    
    current_network_from=false;
    if (radioFromNetwork->isChecked()){
        // la neurona es de la red actual
        current_network_from=true;
        setNum_IdNetFrom(-1);
    }
    else if (textoNF.toInt()>0){
        setNum_IdNetFrom(textoNF.toInt());
    }
    else {
        control = false;
    }
    
    // neurona To
    current_network_to=false;
    if (radioToNetwork->isChecked()){
        current_network_to=true;
        setNum_IdNetTo(-1);
    }
    else if (textoNT.toInt()>0) {
        setNum_IdNetTo(textoNT.toInt());
    }
    else{
        control = false;
    }
    
    if (control){
        QDialog::accept();
    }
    else {
//        QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron/s... out of range"),
//                                 tr("&Ok"));
        QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron/s... out of range"),
                                 QMessageBox::Ok);
    }
    
}

void addSynapseNetDialog::radioFromNetView_pressed()
{
    // se va a seleccionar una neurona de subnet
    SpinBoxFromNetView->setEnabled(true);
    radioFromNetwork->setChecked(false);
}


void addSynapseNetDialog::radioFromNetwork_pressed()
{
    // se va a seleccionar una neurona de la red principal
    SpinBoxFromNetView->setEnabled(false);
    radioFromNetView->setChecked(false);
    // cargar el numero de neuronas de cada capa para la red principal
    setMinMaxValuesFrom (current_ni , current_nh , current_no);

}

void addSynapseNetDialog::radioToNetView_pressed()
{
    SpinBoxToNetView->setEnabled(true);
    radioToNetwork->setChecked(false);
}

void addSynapseNetDialog::radioToNetwork_pressed()
{

    SpinBoxToNetView->setEnabled(false);
    radioToNetView->setChecked(false);
    // cargar el numero de neuronas de cada capa para la red principal
    setMinMaxValuesTo (current_ni , current_nh , current_no);
}

bool addSynapseNetDialog::setNum_IdNetFrom(int i)
{
    num_IdNetFrom  = i;
    return true;
}

bool addSynapseNetDialog::setNum_IdNetTo(int i)
{
    num_IdNetTo  = i;
    return true;
}

int addSynapseNetDialog::getNum_IdNetFrom(void)
{

    return  num_IdNetFrom = SpinBoxFromNetView->cleanText().toInt();
}

int addSynapseNetDialog::getNum_IdNetTo()
{
    return num_IdNetTo = SpinBoxToNetView->cleanText().toInt();
}

void addSynapseNetDialog::setSubNets(QList<SubNet *> *subnets)
{
    this->subnets = subnets;
    
    // se actualiza el numero de subnets de los desplegables SubNet
    if (subnets->count() > 0)
    {
        SpinBoxFromNetView->setRange(1, subnets->count());
        SpinBoxToNetView->setRange(1, subnets->count());
        radioFromNetView->setEnabled(true);
        radioToNetView->setEnabled(true);
    }
    else {
        // no hay subnets en la estructura principal
        radioFromNetView->setEnabled(false);
        radioToNetView->setEnabled(false);
        SpinBoxFromNetView->setEnabled(false);
        SpinBoxToNetView->setEnabled(false);
    }
    
}

void addSynapseNetDialog::updateFromNeurons(int /*n_*/)
{
    // se ha elegido una nueva subnet para FROM
    // hay que recargar los valores min-max para 'From layer'
    SubNet  *subnet = 0;

    if (radioFromNetView->isChecked()) {
        int num = SpinBoxFromNetView->cleanText().toInt();
        subnet = subnets->at(num-1);
        if (subnet) {
            // cargar el numero de neuronas de cada capa para esta subred
            setMinMaxValuesFrom (subnet->getNumInputNeurons(),
                                 subnet->getNumHiddenNeurons(),
                                 subnet->getNumOutputNeurons());
            radioFromInput->setChecked(true);
            radioFromHidden->setChecked(false);
            radioFromOutput->setChecked(false);

        }
        else {
            // esa subnet no es valida
//            QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron from ... out of range"),tr("&Ok"));
                        QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron from ... out of range"), QMessageBox::Ok);
        }
    }

}


void addSynapseNetDialog::setCurrentMinMaxValues(int ni, int nh, int no)
{
    // se cargan los numeros de neuronas de cada capa (de la red principal)
    this->current_ni = ni;
    this->current_nh = nh;
    this->current_no = no;
    
    setMinMaxValuesFrom(ni, nh, no);
    setMinMaxValuesTo (ni, nh, no);
}

void addSynapseNetDialog::updateToNeurons(int /*n_*/)
{
    // se ha elegido una nueva subnet para TO
    // hay que recargar los valores min-max para 'To layer'
    SubNet  *subnet = 0;

    if (radioToNetView->isChecked()) {
        int num = SpinBoxToNetView->cleanText().toInt();
        subnet = subnets->at(num-1);

        if (subnet) {
            // cargar el numero de neuronas de cada capa para esta subred
            setMinMaxValuesTo (subnet->getNumInputNeurons(),
                               subnet->getNumHiddenNeurons(),
                               subnet->getNumOutputNeurons());
            radioToInput->setChecked(true);
            radioToHidden->setChecked(false);
            radioToOutput->setChecked(false);
        }
        else {
            // esa subnet no es valida
//            QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron from ... out of range"),tr("&Ok"));
                        QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron from ... out of range"), QMessageBox::Ok);
        }
    }

}

void addSynapseNetDialog::updateFromCurrentNetwork()
{
    // llamo a la inicialización de valores de las capas de la red principal
    setLayerFrom(2);
    setLayerFrom(1);
    setLayerFrom(0);
    radioFromInput->setChecked(true);
    radioFromHidden->setChecked(false);
    radioFromOutput->setChecked(false);

}

void addSynapseNetDialog::updateToCurrentNetwork()
{
    setLayerTo(2);
    setLayerTo(1);
    setLayerTo(0);
    radioToInput->setChecked(true);
    radioToHidden->setChecked(false);
    radioToOutput->setChecked(false);
}

bool addSynapseNetDialog::isCurrentNetworkFrom(void)
{
    return current_network_from;
}

bool addSynapseNetDialog::isCurrentNetworkTo(void)
{
    return current_network_to;
}
