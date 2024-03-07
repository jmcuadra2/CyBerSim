#include "synapsevaluesdialog.h"

#include <QVariant>
#include <QMessageBox>
#include <QPen>
#include <QPainter>
#include <QListWidgetItem>

#include "../../net/neuron.h"
#include "../../neuraldis/ndmath.h"

/*
 *  Constructs a synapseValuesDialog which is a child of 'parent', with the
 *  name 'name'.'
 */
synapseValuesDialog::synapseValuesDialog(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);

    QObject::connect(ListBoxW, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(setWeight(QTableWidgetItem *)));
    QObject::connect(ListBoxW, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(setIdColor(QTableWidgetItem *)));
    QObject::connect(FloatSpinWeight, SIGNAL(valueChanged(double)), this, SLOT(setWeight(double)));
    QObject::connect(recmonCheckBox, SIGNAL(toggled(bool)), this, SLOT(setRecMonable(bool)));
}

/*
 *  Destroys the object and frees any allocated resources
 */
synapseValuesDialog::~synapseValuesDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void synapseValuesDialog::languageChange()
{
    retranslateUi(this);
}

void synapseValuesDialog::init(Synapse *syn, bool mode, bool adding )
{

    Neuron *neuron_to, *neuron_from;
    mode_edit = mode;
    synapse = syn;
    setNewCaption(adding);
    setDecimalsMantisa(6, 3);
    setWeightsList();
    setWeight(synapse->getWeight());
    setNumId(synapse->getNumId());
    neuron_to = synapse->getPointerTo();
    neuron_from = synapse->getPointerFrom();
    numIdSpinBox->setEnabled(false);
    setRecMonable(synapse->isRecMonable());
}

void synapseValuesDialog::setNumId(int n_id) {
    numIdSpinBox->setValue(n_id);
}

double synapseValuesDialog::getWeight( void )
{
    return FloatSpinWeight->value();
}

void synapseValuesDialog::setWeight( double w_)
{
    w = w_ ;
    FloatSpinWeight->setValue(w);
}

void synapseValuesDialog::saveThis(void )
{
    const QTableWidgetItem* f_item;
    QString s_float = FloatSpinWeight->cleanText();
    double weight = s_float.toDouble();
    FloatSpinWeight->setValue(weight);
    s_float = formatFloat(FloatSpinWeight->value());
    QList<QTableWidgetItem*> list = ListBoxW->findItems(s_float, Qt::MatchExactly);
    if(list.size() > 0) {
        f_item = list.at(0);
        if(f_item->isSelected()) {
            synapse->setIdColor(getIdColor());
            synapse->setWeight(getWeight());
        }
        else
//            QMessageBox::information(nullptr,tr("editSynapse"),
//                                 QString(tr("Duplicated value in %1, %2")).
//                                 arg(ListBoxW->row(f_item) + 1).arg(ListBoxW->column(f_item) + 1), tr("&Ok"));
            QMessageBox::information(nullptr,tr("editSynapse"),
                                     QString(tr("Duplicated value in %1, %2")).
                                     arg(ListBoxW->row(f_item) + 1).arg(ListBoxW->column(f_item) + 1), QMessageBox::Ok);
        FloatSpinWeight->setFocus();
    }
    else {
        synapse->setIdColor(getIdColor());
        synapse->setWeight(getWeight());
        synapse->setRecMonable(rec_mon);
    }
}

void synapseValuesDialog::setWeightsList(void)
{       
    int i;
    int n_weights = synapse->weightsMask.size();
    nonedit_pos.resize(0);
    nonedit_pos = synapse->getNonEditableWeights();
    QPixmap pixmap(20, 20);
    QPen pen(Qt::black, 1);
    QPainter painter(&pixmap);
    painter.setPen(pen);

    setIdColor(int(synapse->getIdColor()));
    ListBoxW->setColumnCount(4);
    int rows = n_weights % 4 == 0 ? n_weights / 4 : n_weights / 4 + 1;
    ListBoxW->setRowCount(rows);
    for(i = 0; i < n_weights; i++){
        painter.save();
        painter.setBrush(QBrush(QColor(Synapse::synapse_colors[6 + i])));
        painter.drawRect(0, 0, 20 ,20);
        painter.restore();
        QTableWidgetItem* item = new QTableWidgetItem(pixmap, formatFloat(synapse->weightsMask.at(i)));
        ListBoxW->setItem(i / 4, i % 4, item);
    }
    painter.end();
    ListBoxW->setCurrentCell(n_item_color/4, n_item_color % 4);
}

void synapseValuesDialog::setIdColor(int n_id)
{
    n_item_color = n_id;
    if(nonedit_pos.size() > 0) {
        if(nonedit_pos.indexOf(n_item_color) > -1)
            FloatSpinWeight->setEnabled(false);
        else
            FloatSpinWeight->setEnabled(true);
    }
    else
        FloatSpinWeight->setEnabled(true);
}


uint synapseValuesDialog::getIdColor( void )
{
    return uchar(n_item_color);
}

void synapseValuesDialog::setIdColor(QTableWidgetItem *item)
{
    setIdColor(item->row() * 4 + item->column());
}

void synapseValuesDialog::setWeight(QTableWidgetItem *item)
{
    setWeight(item->text().toDouble());
}

void synapseValuesDialog::setDecimalsMantisa( int n_d, int mant_ )
{
    n_dec = uint(n_d);
    mant = uint(mant_);
    FloatSpinWeight->setDecimals(n_dec);

}

QString synapseValuesDialog::formatFloat( double f )
{
    int  ent;
    double frac;
    QString sent, sfrac;

    if(f < 0){
        ent = int(ceil(f));
        frac=fabs(f - ent) * pow(10.0, int(n_dec));
    }
    else {
        ent = int(floor(f));
        frac=( f - ent) * pow(10.0, int(n_dec));
    }
    sent = QString::number(ent);
    sfrac = QString::number(frac);
    if(f < 0 && f > -1)
        sent.insert(0, '-');
    return QString("%1.%2").arg(sent.rightJustified(mant, ' ')).
            arg(sfrac.rightJustified(n_dec, '0'));

}

void synapseValuesDialog::setNewCaption(bool adding)
{
    QString c = QString(" " + synapse->connName(false) + " " +
                        tr("from") + " " + synapse->getPointerFrom()->
                        devName(false) + ": %1" +
                        " " + tr("to") + " " + synapse->getPointerTo()->
                        devName(false) + ": %2").
            arg(synapse->getPointerFrom()->getNumId()).
            arg(synapse->getPointerTo()->getNumId());

    if(adding) setWindowTitle(tr("Adding") + c);
    else setWindowTitle(tr("Editing") + c);
}

void synapseValuesDialog::setRecMonable( bool recmon )
{

    rec_mon = recmon;
    recmonCheckBox->setChecked(rec_mon);
}

void synapseValuesDialog::accept( void )
{
    if(mode_edit)
        QewExtensibleDialog::accept();
}
