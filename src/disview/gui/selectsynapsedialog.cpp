#include "selectsynapsedialog.h"

#include <QVariant>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QButtonGroup>
#include "../../net/synapse.h"

/*
 *  Constructs a selectSynapseDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
selectSynapseDialog::selectSynapseDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    buttonGroupFrom = new QButtonGroup(this);
    buttonGroupFrom->addButton(radioFromInput, 0);
    buttonGroupFrom->addButton(radioFromHidden, 1);
    buttonGroupFrom->addButton(radioFromOutput, 2);

    QObject::connect(buttonEditNeuronCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(buttonEditNeuronOK, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ListBoxTo, SIGNAL(highlighted(int)), this, SLOT(setNum_IdSynapse(int)));
    QObject::connect(buttonGroupFrom, SIGNAL(buttonClicked(int)), this, SLOT(setLayerFrom(int)));
    QObject::connect(SpinBoxFrom, SIGNAL(valueChanged(int)), this, SLOT(setNum_IdNeuronFrom(int)));
    QObject::connect(SpinBoxFrom, SIGNAL(valueChanged(QString)), this, SLOT(getTextFrom(QString)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
selectSynapseDialog::~selectSynapseDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void selectSynapseDialog::languageChange()
{
    retranslateUi(this);
}

int selectSynapseDialog::getNum_IdSynapse(void)
{
    return num_IdSynapse;
}

int selectSynapseDialog::getNum_IdNeuronFrom(void)
{
    return num_IdNeuronFrom = SpinBoxFrom->cleanText().toInt();
}
bool selectSynapseDialog::validNum_IdNeuronFrom( int n_i )
{
    bool ret = false;

    switch(layerFrom) {
    case 0:
        if(n_i > 0 && n_i <= n_in)
            ret = true;
        break;
    case 1:
        if(n_i > n_in && n_i <= n_in + n_hid)
            ret = true;
        break;
    case 2:
        if(n_i > n_in + n_hid && n_i <= n_in + n_hid + n_out)
            ret = true;
        break;
    }
    return ret;
}

void selectSynapseDialog::setNum_IdNeuronFrom( int n_i )
{     
    Neuron *neuron_from = 0;
    Synapse *synapse ;
    int tam;
    bool ret = false;

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
        QList<Synapse*> Axon = neuron_from->getAxon();
        QListIterator<Synapse*> its(Axon);
        while(its.hasNext()) {
            synapse = its.next();
            tam++;
            QListWidgetItem* item = new QListWidgetItem(QString("%1").
                                   arg(QString::number(synapse->getTo()).rightJustified(6)));
            ListBoxTo->insertItem(tam, item);
            num_syn.resize(tam + 1);
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

void selectSynapseDialog::setNum_IdSynapse( int tam)
{
    num_IdSynapse = ulong(num_syn[tam]);
}

int selectSynapseDialog::getLayerFrom(void)
{
    return layerFrom;
}

void selectSynapseDialog::setMinMaxValues( int n_i, int n_h, int n_o, QList<Neuron *> In_Ptr,
                                           QList<Neuron *> Hid_Ptr, QList<Neuron *> Out_Ptr)
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

void selectSynapseDialog::setLayerFrom( int l_ )
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

void selectSynapseDialog::getTextFrom( const QString& texto )
{  
    if(!validNum_IdNeuronFrom(texto.toInt()))
//        QMessageBox::information(nullptr,tr("selectSynapse"), tr("Neuron from ... out of range"),tr("&Ok"));
    QMessageBox::information(nullptr,tr("selectSynapse"), tr("Neuron from ... out of range"), QMessageBox::Ok);
}
void selectSynapseDialog::accept(  )
{
    QString textoF = SpinBoxFrom->text();
    if(validNum_IdNeuronFrom(textoF.toInt())) {
        QDialog::accept();
    }
    else
//        QMessageBox::information(nullptr,tr("selectSynapse"), tr("Neuron from... out of range"),tr("&Ok"));
            QMessageBox::information(nullptr,tr("selectSynapse"), tr("Neuron from... out of range"), QMessageBox::Ok);
}
