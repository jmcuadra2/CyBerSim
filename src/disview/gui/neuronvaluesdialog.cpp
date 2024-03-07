#include "neuronvaluesdialog.h"

#include <QVariant>
#include <QListWidgetItem>
#include "../../net/neuron.h"
#include "../../neuraldis/dictionarymanager.h"

/*
 *  Constructs a neuronValuesDialog which is a child of 'parent', with the
 *  name 'name'.'
 */
neuronValuesDialog::neuronValuesDialog(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);

    connect(FloatSpinThres, SIGNAL(valueChanged(double)),
                     this, SLOT(setThreshold(double)));
    connect(FloatSpinInit, SIGNAL(valueChanged(double)),
                     this, SLOT(setInitExtern(double)));
    connect(ListBoxAF, SIGNAL(currentRowChanged(int)),
                     this, SLOT(setActivationFunction(int)));
    connect(recmonCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(setRecMonable(bool)));
    connect(FloatSpinOut, SIGNAL(valueChanged(double)),
                     this, SLOT(setOutput(double)));
    connect(ListBoxAF, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem *)),
                     this, SLOT(ListBoxAF_selectionChanged(QListWidgetItem*)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
neuronValuesDialog::~neuronValuesDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void neuronValuesDialog::languageChange()
{
    retranslateUi(this);
}

void neuronValuesDialog:: init(Neuron *n, bool mode, bool adding)
{
    
    neuron = n;
    mode_edit = mode;
    setNewCaption(adding);
    setSubLayer(neuron->getSubLayer());
    setOutput(neuron->output());
    setThreshold(neuron->getThreshold());
    setInitExtern(neuron->getInitExtern());
    createActFuncList();
    setActivationFunction(neuron->getActivationFunction());
    setRecMonable(neuron->isRecMonable());
    /* si la función de activación es sigmoidal, se leen estos valores */
    if(neuron->getActivationFunction()==AbstractFunction::FUNC_SIGMOIDE){
        setSigmoidalValueA(neuron->getSigmoidalA());
        setSigmoidalValueB(neuron->getSigmoidalB());
    }
    /* fin de los cambios */
}

double neuronValuesDialog::getOutput(void)
{
    return out = FloatSpinOut->value();
}

double neuronValuesDialog::getThreshold(void)
{
    return thres = FloatSpinThres->value();
}

int neuronValuesDialog::getSubLayer(void)
{
    return sub_layer = SpinBoxSubLayer->cleanText().toInt();
}

int neuronValuesDialog::getActivationFunction(void)
{
    return actFunc;
}
double neuronValuesDialog::getInitExtern(void)
{
    return initext = FloatSpinInit->value();
}
/* funciones get de las variables sigmoidales */

double neuronValuesDialog::getSigmoidalA(void)
{
    return (spinBox2->value());
}

double neuronValuesDialog::getSigmoidalB(void)
{
    return (spinBox3->value)();
}

/* fin de los cambios */

void neuronValuesDialog::setSubLayer(int  t)
{
    sub_layer = t;
    SpinBoxSubLayer->setValue(sub_layer);
}

void neuronValuesDialog::setActivationFunction(int d)
{
    NameClass_Map::Iterator it;
    int i = 0;
    disconnect(ListBoxAF, SIGNAL(currentRowChanged(int)),
                        this, SLOT(setActivationFunction(int)));
    for (it = names_map.begin(); it != names_map.end(); ++it) {
        if(it.value() == d) {
            actFunc = d;
            ListBoxAF->setCurrentRow(i);
            break;
        }
        ++i;
    }
    connect(ListBoxAF, SIGNAL(currentRowChanged(int)),
                         this, SLOT(setActivationFunction(int)));
}

void neuronValuesDialog::setActivationFunction(QListWidgetItem *item )
{    
    actFunc =  names_map[item->text()];
}

/* funciones set de las variables sigmoidales */

void neuronValuesDialog::setSigmoidalValueA(double  Value)
{
    spinBox2->setValue(Value);
}

void neuronValuesDialog::setSigmoidalValueB(double  Value)
{
    spinBox3->setValue(Value);
}

/* fin de los cambios */ 

void neuronValuesDialog::setInitExtern(double i )
{
    initext = i;
    FloatSpinInit->setValue(i);
}

void neuronValuesDialog::setOutput(double o)
{
    out =  o;
    FloatSpinOut->setValue(o);
}

void neuronValuesDialog::setThreshold(double t)
{
    thres  = t;
    FloatSpinThres->setValue(t);
}

void neuronValuesDialog::setDecimalsMantisa(int  d, int /*m*/)
{
    FloatSpinOut->setDecimals(uint(d)) ;
    FloatSpinThres->setDecimals(uint(d)) ;
    FloatSpinInit->setDecimals(uint(d)) ;
}

void neuronValuesDialog::setNewCaption(bool adding)
{
    QString c = QString(" " + neuron->devName(false) + " " + tr("number") + ": %1").
            arg(neuron->getNumId());
    if(QewParent()) {
        if(adding)
            QewParent()->setWindowTitle(tr("Adding") + c);
        else
            QewParent()->setWindowTitle(tr("Editing") + c);
    }
    else {
        if(adding)
            setWindowTitle(tr("Adding") + c);
        else
            setWindowTitle(tr("Editing") + c);
    }
}

void neuronValuesDialog::saveThis(void )
{
    neuron->setActivationFunction(getActivationFunction());
    neuron->setSubLayer(getSubLayer());
    neuron->setOutput(getOutput());
    neuron->setThreshold(getThreshold());
    neuron->setInitExtern(getInitExtern());
    neuron->setRecMonable(rec_mon);
    /* guardamos los valores sigmoidales */
    if(neuron->getActivationFunction()==1){
        neuron->setSigmoidalA(getSigmoidalA());
        neuron->setSigmoidalB(getSigmoidalB());
    }
    /* fin de los cambios */
    //
    
}

void neuronValuesDialog::setRecMonable(bool recmon )
{
    rec_mon = recmon;
    recmonCheckBox->setChecked(rec_mon);
    
}


void neuronValuesDialog::accept(void )
{
    if(mode_edit)
        QewExtensibleDialog::accept();
}

void neuronValuesDialog::createActFuncList(void )
{
    QDomElement function_classes;
    DictionaryManager* dict_manager = DictionaryManager::instance();
    bool ret = dict_manager->readFileConf("neuronValuesDialog", "AbstractFunction",
                                          function_classes);
    if(ret) {
        dict_manager->getNamesMap(function_classes, names_map);
        NameClass_Map::Iterator it;
        ListBoxAF->clear();
        int i = 0;
        for (it = names_map.begin(); it != names_map.end(); ++it) {
            ListBoxAF->insertItem(i, it.key());
            i++;
        }
        adjustSize();
    }
    else
        ListBoxAF->setEnabled(false);
    
}

void neuronValuesDialog::ListBoxAF_selectionChanged(QListWidgetItem *newCurrent)
{
    QString t = newCurrent->text();
    if(newCurrent->text() == "Sigmoidal") {
        spinBox2->setEnabled(true);
        spinBox3->setEnabled(true);
    }
    else {
        spinBox2->setEnabled(false);
        spinBox3->setEnabled(false);
    }
}
