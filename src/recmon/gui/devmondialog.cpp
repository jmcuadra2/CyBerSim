#include "devmondialog.h"

#include <QVariant>
#include <QMessageBox>
#include "../graphicmonitor.h"

/*
 *  Constructs a DevMonDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */

DevMonDialog::DevMonDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    QObject::connect(minFloatSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setMin(double)));
    QObject::connect(maxFloatSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setMax(double)));
    QObject::connect(OkButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(timeupdSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(setTimeUpd(int)));
    QObject::connect(tLabelsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(setTLabels(int)));
    QObject::connect(vLabelsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(setVLabels(int)));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(minDiffFloatSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setMinDiff(double)));
    QObject::connect(maxDiffFloatSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setMaxDiff(double)));
    QObject::connect(vDiffLabelsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(setVDiffLabels(int)));
    QObject::connect(mon1LineEdit, SIGNAL(textChanged(QString)),
                     this, SLOT(setWindowTitle1(QString)));
    QObject::connect(mon2LineEdit, SIGNAL(textChanged(QString)),
                     this, SLOT(setWindowTitle2(QString)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
DevMonDialog::~DevMonDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DevMonDialog::languageChange()
{
    retranslateUi(this);
}

/*! \class DevMonDialog
 *   \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */

void DevMonDialog::init(GraphicMonitor *gmo, GraphicMonitor *gmd, int dec, int mant)  {
    gmonOut = gmo;
    gmonDiff = gmd;
    minFloatSpinBox->setDecimals(dec);
    maxFloatSpinBox->setDecimals(dec);
    setMin(gmonOut->getYMin());
    setMax(gmonOut->getYMax());
    setTimeUpd(gmonOut->getXUpd());
    setTLabels(gmonOut->getNXLabels());
    setVLabels(gmonOut->getNYLabels());
    setWindowTitle1(gmonOut->getTitulo());
    minDiffFloatSpinBox->setDecimals(dec);
    maxDiffFloatSpinBox->setDecimals(dec);
    setMinDiff(gmonDiff->getYMin());
    setMaxDiff(gmonDiff->getYMax());
    setVDiffLabels(gmonDiff->getNYLabels());
    setWindowTitle2(gmonDiff->getTitulo());
} 

double DevMonDialog::getMin(void )
{ 
    return min = minFloatSpinBox->value();
}

void DevMonDialog::setMin(double d)
{
    min = d;
    minFloatSpinBox->setValue(d);
}

double DevMonDialog::getMax( void )
{
    return max = maxFloatSpinBox->value();
}

void DevMonDialog::setMax( double d)
{
    max = d;
    maxFloatSpinBox->setValue(d);
}

double DevMonDialog::getMinDiff(void )
{ 
    return minDiff = minDiffFloatSpinBox->value();
}

void DevMonDialog::setMinDiff(double d)
{
    minDiff = d;
    minDiffFloatSpinBox->setValue(d);
}

double DevMonDialog::getMaxDiff( void )
{
    return maxDiff = maxDiffFloatSpinBox->value();
}

void DevMonDialog::setMaxDiff( double d)
{
    maxDiff = d;
    maxDiffFloatSpinBox->setValue(d);
}

int DevMonDialog::getTimeUpd( void )
{
    return timeupd = timeupdSpinBox->cleanText().toInt();
}

void DevMonDialog::setTimeUpd( int tu)
{
    if(tu > 0)
        timeupd = tu;
    timeupdSpinBox->setValue(tu);
}

int DevMonDialog::getTLabels( void )
{
    return tLabels = tLabelsSpinBox->cleanText().toInt();
}

void DevMonDialog::setTLabels( int tl)
{
    if(tl > 1)
        tLabels = tl;
    tLabelsSpinBox->setValue(tl);
}

int DevMonDialog::getVLabels( void )
{
    return vLabels = vLabelsSpinBox->cleanText().toInt();
}

void DevMonDialog::setVLabels( int vl)
{
    if(vl > 0)
        vLabels = vl;
    vLabelsSpinBox->setValue(vl);
}

int DevMonDialog::getVDiffLabels( void )
{
    return vDiffLabels = vDiffLabelsSpinBox->cleanText().toInt();
}

void DevMonDialog::setVDiffLabels( int vl)
{
    vDiffLabels = vl;
    vDiffLabelsSpinBox->setValue(vl);
}

void DevMonDialog::setInfor(const QString in) {
    inforLineEdit->setText(in);
}

void DevMonDialog::accept(void) {
    if(getMax() <= getMin()) {
//        QMessageBox::warning(this, tr("Edit monitoring..."),
//                             tr("Maximun output must be greater or equal than minimun output"),  tr("&Return")) ;
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("Edit monitoring..."));
        msgBox.setInformativeText(tr("Maximun output must be greater or equal than minimun output"));
        QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
        msgBox.exec();
        return;
    }
    if(getMaxDiff() <= getMinDiff()) {
//        QMessageBox::warning(this, tr("Edit monitoring..."),
//                             tr("Maximun difference must be greater or equal than minimun difference"),  tr("&Return")) ;
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("Edit monitoring..."));
        msgBox.setInformativeText(tr("Maximun difference must be greater or equal than minimun difference"));
        QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
        msgBox.exec();
        return;
    }
    int sig_cnt1 = 0;
    for(int i = 0; i < firstMonBox->count(); i++) {
        if(firstMonBox->item(i)->isSelected())
            ++sig_cnt1;
    }
    if(sig_cnt1 > GraphicMonitor::N_MAX_GRAPHS) {
        GraphicMonitor::nMaxGraphsMessage();
        return;
    }
    int sig_cnt2 = 0;
    for(int i = 0; i < secondMonBox->count(); i++) {
        if(secondMonBox->item(i)->isSelected())
            ++sig_cnt2;
    }
    if(sig_cnt2 > GraphicMonitor::N_MAX_GRAPHS) {
        GraphicMonitor::nMaxGraphsMessage();
        return;
    }
    gmonOut->setYMin(getMin());
    gmonOut->setYMax(getMax());
    gmonOut->setXUpd(getTimeUpd());
    gmonOut->setNXLabels(getTLabels());
    gmonOut->setNYLabels(getVLabels());
    gmonOut->setTitulo(mon1Caption);
    QVector<int> id_gr = gmonOut->idGraphs();
    while(id_gr.count()) {
        gmonOut->delGraph(id_gr[0]);  // 0 no i
        id_gr.removeFirst();
    }

    for(int i = 0; i < firstMonBox->count(); i++) {
        if(firstMonBox->item(i)->isSelected())
            gmonOut->addGraph(signals_num[i]);
    }

    gmonDiff->setYMin(getMinDiff());
    gmonDiff->setYMax(getMaxDiff());
    gmonDiff->setXUpd(getTimeUpd());
    gmonDiff->setNXLabels(getTLabels());
    gmonDiff->setNYLabels(getVDiffLabels());
    gmonDiff->setTitulo(mon2Caption);
    id_gr = gmonDiff->idGraphs();
    while(id_gr.count()) {
        gmonDiff->delGraph(id_gr[0]);  // 0 no i
        id_gr.removeFirst();
    }
    for(int i = 0; i < secondMonBox->count(); i++) {
        if(secondMonBox->item(i)->isSelected())
            gmonDiff->addGraph(signals_num[i]);
    }
    QDialog::accept();
} 

void DevMonDialog::setSignalNames( const QMap<QString, int> & signals_map )
{
    int s_num = signals_map.count();
    signals_num.resize(s_num);
    QMap<QString, int>::ConstIterator it;
    int i = 0;
    int r = -1;
    QVector<int> mon1_signs = gmonOut->idGraphs();
    QVector<int>mon2_signs = gmonDiff->idGraphs();
    for(it = signals_map.begin(); it != signals_map.end(); ++it) {
        QListWidgetItem* item = new QListWidgetItem(it.key());
        firstMonBox->insertItem(i, item);
        item = new QListWidgetItem(it.key());
        secondMonBox->insertItem(i, item);
        signals_num[i] = it.value();
        if(mon1_signs.count()) {
            r = mon1_signs.indexOf(signals_num[i]);
            if(r != -1)
                firstMonBox->item(i)->setSelected(true);
        }
        if(mon2_signs.count()) {
            r = mon2_signs.indexOf(signals_num[i]);
            if(r != -1)
                secondMonBox->item(i)->setSelected(true);
        }
        i++;
    }

}


void DevMonDialog::setWindowTitle1( const QString & cap )
{
    mon1Caption = cap;
    mon1LineEdit->setText(mon1Caption);
}


void DevMonDialog::setWindowTitle2( const QString & cap )
{
    mon2Caption = cap;
    mon2LineEdit->setText(mon2Caption);
}
