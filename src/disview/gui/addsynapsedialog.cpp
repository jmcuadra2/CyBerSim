#include "addsynapsedialog.h"

#include <QVariant>
#include <QMessageBox>
#include <QButtonGroup>

/*
 *  Constructs a addSynapseDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
addSynapseDialog::addSynapseDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
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

    QObject::connect(buttonEditNeuronCancel, SIGNAL(clicked()),
                     this, SLOT(reject()));
    QObject::connect(SpinBoxTo, SIGNAL(valueChanged(QString)),
                     this, SLOT(getTextTo(QString)));
    QObject::connect(SpinBoxTo, SIGNAL(valueChanged(int)),
                     this, SLOT(setNum_IdNeuronTo(int)));
    QObject::connect(buttonGroupFrom, SIGNAL(buttonClicked(int)),
                     this, SLOT(setLayerFrom(int)));
    QObject::connect(buttonGroupTo, SIGNAL(buttonClicked(int)),
                     this, SLOT(setLayerTo(int)));
    QObject::connect(SpinBoxFrom, SIGNAL(valueChanged(QString)),
                     this, SLOT(getTextFrom(QString)));
    QObject::connect(SpinBoxFrom, SIGNAL(valueChanged(int)),
                     this, SLOT(setNum_IdNeuronFrom(int)));
    QObject::connect(buttonEditNeuronOK, SIGNAL(clicked()), this, SLOT(accept()));

}

/*
 *  Destroys the object and frees any allocated resources
 */
addSynapseDialog::~addSynapseDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void addSynapseDialog::languageChange()
{
    retranslateUi(this);
}

int addSynapseDialog::getNum_IdNeuronFrom(void)
{
    return num_IdNeuronFrom = SpinBoxFrom->cleanText().toInt();
}

int addSynapseDialog::getNum_IdNeuronTo(void)
{
    return num_IdNeuronTo = SpinBoxTo->cleanText().toInt();
}

bool addSynapseDialog::setNum_IdNeuronFrom( int n_i )
{
    bool ret = false;
    switch(layerFrom) {
    case 0:
        if(n_i > 0 && n_i <= n_in) {
            num_IdNeuronFrom  = n_i;
            ret = true;
        }
        else
            SpinBoxFrom->setValue( 1);

        break;
    case 1:
        if(n_i > n_in && n_i <= n_in + n_hid) {
            num_IdNeuronFrom  = n_i;
            ret = true;
        }
        else
            SpinBoxFrom->setValue( n_in + 1);

        break;
    case 2:
        if(n_i > n_in + n_hid && n_i <= n_in + n_hid + n_out) {
            num_IdNeuronFrom  = n_i;
            ret = true;
        }
        else
            SpinBoxFrom->setValue( n_in + n_hid + 1);

        break;
    }
    return ret;
}

bool addSynapseDialog::setNum_IdNeuronTo( int n_i )
{
    bool ret = false;
    switch(layerTo) {
    case 0:
        if(n_i > 0 && n_i <= n_in) {
            num_IdNeuronTo  = n_i;
            ret = true;
        }
        else
            SpinBoxTo->setValue( 1);

        break;
    case 1:
        if(n_i > n_in && n_i <= n_in + n_hid) {
            num_IdNeuronTo  = n_i;
            ret = true;
        }
        else
            SpinBoxTo->setValue( n_in + 1);

        break;
    case 2:
        if(n_i > n_in + n_hid && n_i <= n_in + n_hid + n_out) {
            num_IdNeuronTo  = n_i;
            ret = true;
        }
        else
            SpinBoxTo->setValue( n_in +n_hid + 1);

        break;
    }
    return ret;
}

int addSynapseDialog::getLayerFrom(void)
{
    return layerFrom;
}

int addSynapseDialog::getLayerTo(void)
{
    return layerTo;
}

void addSynapseDialog::setMinMaxValues( int n_i, int n_h, int n_o )
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

void addSynapseDialog::setLayerFrom( int l_ )
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

void addSynapseDialog::setLayerTo( int l_ )
{
    layerTo = l_;
    switch(l_) {
    case 0:
        SpinBoxTo->setRange(1, n_in );
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

void addSynapseDialog::getTextFrom( const QString& texto )
{  
    if(!setNum_IdNeuronFrom(texto.toInt()))
//        QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron from ... out of range"),tr("&Ok"));
            QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron from ... out of range"), QMessageBox::Ok);
}

void addSynapseDialog::getTextTo( const QString& texto )
{  
    if(!setNum_IdNeuronTo(texto.toInt()))
//        QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron to ... out of range"),tr("&Ok"));
            QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron to ... out of range"), QMessageBox::Ok);
}

void addSynapseDialog::accept(  )
{
    QString textoF = SpinBoxFrom->cleanText();
    QString textoT = SpinBoxTo->cleanText();
    if(setNum_IdNeuronFrom(textoF.toInt()) && setNum_IdNeuronTo(textoT.toInt()))
        QDialog::accept();
    else
//        QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron/s... out of range"),tr("&Ok"));
            QMessageBox::information(nullptr,tr("addSynapse"), tr("Neuron/s... out of range"), QMessageBox::Ok);
}
