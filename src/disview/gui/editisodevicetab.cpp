#include "editisodevicetab.h"

#include <QVariant>
#include <QMessageBox>
#include <QListWidgetItem>

#include "../../neuraldis/ndmath.h"

/*
 *  Constructs a EditISODeviceTab which is a child of 'parent', with the
 *  name 'name'.'
 */
EditISODeviceTab::EditISODeviceTab(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);
    resonTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(addButton, SIGNAL(clicked()), this, SLOT(addRes()));
    connect(delButton, SIGNAL(clicked()), this, SLOT(delRes()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(editRes()));
    connect(lrateFloatSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(setLRate(double)));
    connect(nresonSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setNRows(int)));
    connect(replicButton, SIGNAL(clicked()), this, SLOT(replicate()));
    connect(resonTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
                     this, SLOT(editRes()));
    connect(resonTable->verticalHeader(), SIGNAL(sectionClicked(int)),
                     this, SLOT(selectRow(int)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
EditISODeviceTab::~EditISODeviceTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void EditISODeviceTab::languageChange()
{
    retranslateUi(this);
}

void EditISODeviceTab::init(ISODevice *is, bool mod) {

    iso = is;
    mode = mod;
    factory = NetFactory::instance();
    QList<Resonator*> r = iso->getResonators();
    QListIterator<Resonator*> itr(r);
    Resonator *res;
    Resonator *new_res;
    QDomDocument doc("doc");
    while(itr.hasNext()) {
        res = itr.next();
        QDomElement e_cons = doc.createElement("constructor");
        doc.appendChild(e_cons);
        res->write_constructor(e_cons);
        new_res = factory->factoryResonator(NeuralConnection::RESONATOR,
                                            res->getNumId(), e_cons);
        resonators.append(new_res);
        copyResonator(res, new_res);
    }
    nreson = resonators.count();
    nresonSpinBox->setValue(nreson);
    if(!nreson) {
        nresonSpinBox->setEnabled(true);
        addButton->setEnabled(mode);

        editButton->setEnabled(false);
        delButton->setEnabled(false);
        replicButton->setEnabled(false);
    }
    else {
        editButton->setEnabled(true);
        addButton->setEnabled(mode);
        delButton->setEnabled(mode);
        replicButton->setEnabled(mode);
    }

    resonTable->setRowCount(nreson);
    writeTable();
    delButton->setShortcut(QKeySequence(Qt::Key_Delete));
    addButton->setShortcut(QKeySequence(Qt::Key_Insert));
    lrateFloatSpinBox->setDecimals(6);

    setLRate(iso->getLearningRate());
}

double EditISODeviceTab::getLRate( void )
{
    return lrate= lrateFloatSpinBox->value();
} 

void EditISODeviceTab::setLRate( double d)
{
    if(d >= 0.0 &&  d <= 1.0)
        lrate = d;
    lrateFloatSpinBox->setValue(d);
}

void EditISODeviceTab::editRes()
{
    Resonator *res;
    int i = 0;
    int j, hp;
    double damp, weight;
    bool iscs, wfix, posw ;
    QListIterator<Resonator*> it(resonators);
    Resonator *lres;

    selectedRows.clear();

    int r = resonTable->currentRow();
    if(r < 0)
        return;

    res = resonators.at(r);
    hp = res->getHalfPeriod();
    damp = res->getDamp();
    iscs = res->isCStim();
    weight = res->getWeight();
    wfix = res->getWeightFix();
    posw = res->getPosWeight();
    if(res->edit(mode)) {
        writeTableItem(r);
        if(res->isCStim()) {
            if(hp != res->getHalfPeriod()) {
                while(it.hasNext()) {
                    lres = it.next();
                    if(!lres->isCStim()) {
                        ++i;
                        continue;
                    }
                    if(res != lres) {
                        if(res->getHalfPeriod() < lres->getHalfPeriod())
                            break;
                    }
                    ++i;
                }
            }
            else
                i = r;
        }
        else {
            while(it.hasNext()) {
                lres = it.next();
                if(res == lres) {
                    continue;
                }
                if(!lres->isCStim()) {
//                    QMessageBox::information(nullptr,tr("editISO"),
//                                             tr("Only one US is allowed"),tr("&Ok"));
                    QMessageBox::information(nullptr,tr("editISO"),
                                             tr("Only one US is allowed"), QMessageBox::Ok);
                    res->setCStim(iscs);
                    res->setHalfPeriod(hp);
                    res->setDamp(damp);
                    res->setWeight(weight );
                    res->setWeightFix(wfix);
                    res->setPosWeight(posw);
                    writeTableItem(r);
                    return;
                }
            }
        }
        if(r != i) {
            resonators.removeAt(r);
            if(r < i) {
                resonators.insert(i -1, res);
                for(j = r; j <i -1 ; j++)
                    resonTable->verticalHeader()->swapSections(j, j + 1);
            }
            else {
                resonators.insert(i, res);
                for(j = r; j >i ; j--)
                    resonTable->verticalHeader()->swapSections(j, j - 1);
            }
        }
        resonTable->clearSelection();
    }

} 

void EditISODeviceTab::addRes(void)
{
    int i = 0;
    uint n_id = iso->getNumId() * 10000;
    QListIterator<Resonator*> it(resonators);
    Resonator *lres;
    while(it.hasNext()) {
        lres = it.next();
        if(n_id < lres->getNumId())
            n_id = lres->getNumId();
    }
    Resonator *res = factory->factoryResonator(NeuralConnection::RESONATOR,
                                               n_id + 1);
    it.toFront();
    if(res->edit()) {
        while(it.hasNext()) {
            lres = it.next();
            if(res->getHalfPeriod() < lres->getHalfPeriod())
                break;
            ++i;
        }
        resonators.insert(i, res);
        nresonSpinBox->setEnabled(false);

        resonTable->insertRow(i);
        nresonSpinBox->setValue(++nreson);
        writeTableItem(i);
        resonTable->clearSelection();
    }
    else delete
        res;
    selectedRows.clear();
}  

void EditISODeviceTab::delRes()
{
    QVector<int> ldel;
    int i, s, nr;
    nr = resonTable->rowCount();
    s = 0;
    Resonator *res;
    for(i = 0; i < nr; i++) {
        if(selectedRows.contains(i)) {
            s = ldel.size();
            ldel.resize(s + 1);
            ldel[s] = i;
            ++s;
        }
    }
    for(i = s - 1; i >= 0; i--) {
        res = resonators.at(ldel[i]);
        resonators.removeAt(ldel[i]);
        resonTable->removeRow(ldel[i]);
        delete res;
    }

//    resonTable->removeRow(ldel);
    nr = resonTable->rowCount();
    nreson = nr;
    nresonSpinBox->setValue(nr);
    if(!nr)
        nresonSpinBox->setEnabled(true);
    resonTable->clearSelection();

    selectedRows.clear();
} 

double EditISODeviceTab::damp2first(int hper, double damp, double sampling_period) {
    double th;
    double fmax;
    th =  3.1415926535/hper;
    //	fmax = 1/(50*th)*atan(50*th/damp);
    fmax = 1/th*atan(th/sampling_period/damp);
    return fmax;
}

void EditISODeviceTab::setNRows(int n) {
    int i;
    nresonSpinBox->setEnabled(false);
    editButton->setEnabled(true);
    addButton->setEnabled(mode);
    delButton->setEnabled(mode);
    replicButton->setEnabled(mode);
    resonTable->setRowCount(n);
    for(i = 0; i < n; i++)
        resonators.append(factory->factoryResonator(NeuralConnection::RESONATOR, i + 1));
    writeTable();
}

void EditISODeviceTab::writeTable(void) {
    QListIterator<Resonator*> it(resonators);
    Resonator *res = 0;
    int i = 0;
    while(it.hasNext()) {
        res = it.next();
        writeTableItem(i);
        ++i;
    }
    selectedRows.clear();
}

void EditISODeviceTab::writeTableItem(int i) {
    if(i >= resonators.size())
        return;
    Resonator *res = resonators.at(i);

    QString isUS = "";
    QString  isFix = "";
    QString  isPosW = "";
    if(res){
        if(!res->isCStim())
            isUS = "***";
        if(res->getWeightFix())
            isFix = "***";
        if(res->getPosWeight())
            isPosW = "***";
        QTableWidgetItem* item = new QTableWidgetItem(isUS);
        resonTable->setItem(i, 0, item);
        item = new QTableWidgetItem(QString::number(res->getHalfPeriod()));
        resonTable->setItem(i, 1, item);
        item = new QTableWidgetItem(QString::number(res->getDamp(),'f',5));
        resonTable->setItem(i, 2, item);
        item = new QTableWidgetItem(QString::number(damp2first(res->getHalfPeriod(),
                                        res->getDamp(), res->getSamplingPeriod()),'f' ,4));
        resonTable->setItem(i, 3, item);
        item = new QTableWidgetItem(QString::number(res->getWeight(),'f',6));
        resonTable->setItem(i, 4, item);
        item = new QTableWidgetItem(isFix);
        resonTable->setItem(i, 5, item);
        item = new QTableWidgetItem(isPosW);
        resonTable->setItem(i, 6, item);
    }
}

void EditISODeviceTab::replicate(void) {
    QListIterator<Resonator*> it(resonators);
    Resonator *res;
    Resonator *lres;
    int r = resonTable->currentRow();
    res = resonators.at(r);
    if(selectedRows.contains(r)) {
        while(it.hasNext()) {
            lres = it.next();
            if(res != lres) {
                if(res->isCStim())
                    lres->setCStim(true);
                lres->setHalfPeriod(res->getHalfPeriod());
                lres->setDamp(res->getDamp());
            }
        }
        writeTable();
    }
    selectedRows.clear();
} 

bool EditISODeviceTab::validateThis(void) {
    bool ret = false;
    lrate = lrateFloatSpinBox->value();
    if(lrate < 0.0 &&  lrate > 1.0) {
//        QMessageBox::information(nullptr,tr("editISO"),
//                                 tr("Learning rate must be greater or equal than 0\nand lesser or equal than 1"),tr("&Ok"));
        QMessageBox::information(nullptr,tr("editISO"),
                                 tr("Learning rate must be greater or equal than 0\nand lesser or equal than 1"), QMessageBox::Ok);
        return ret;
    }
    int cnt = 0;
    QListIterator<Resonator*> it(resonators);
    Resonator *res;
    while(it.hasNext()) {
        res = it.next();
        if(!res->isCStim())
            cnt++;
    }
    if(cnt >= 2)
//        QMessageBox::information(nullptr,tr("editISO"),
//                                 tr("Only one US is allowed"),tr("&Ok"));
        QMessageBox::information(nullptr,tr("editISO"),
                                 tr("Only one US is allowed"), QMessageBox::Ok);
    else
        ret = true;
    return ret;
}

void EditISODeviceTab::saveThis(void) {
    iso->setLearningRate(getLRate());
    iso->clearResonators();
    QListIterator<Resonator*> ed_res(resonators);
    QList<Resonator*> iso_res;
    Resonator* res;
    Resonator* new_res;
    QDomDocument doc("doc");
    while(ed_res.hasNext()) {
        res = ed_res.next();
        QDomElement e_cons = doc.createElement("constructor");
        doc.appendChild(e_cons);
        res->write_constructor(e_cons);
        new_res = factory->factoryResonator(NeuralConnection::RESONATOR,
                                            res->getNumId(), e_cons);
        if(res->isCStim())
            iso_res.append(new_res);
        else
            iso_res.prepend(new_res);
        copyResonator(res, new_res);
    }
    iso->setResonators(iso_res);
    selectedRows.clear();

}

void EditISODeviceTab::copyResonator(Resonator * res_from, Resonator * res_to)
{

    res_to->setStabWeightMode(res_from->getStabWeightMode());
    res_to->setStabWeightVal(res_from->getStabWeightVal());
    res_to->setLinkWeight(res_from->getLinkWeight());

}

void EditISODeviceTab::cleanThis(void) {
    QMutableListIterator<Resonator*> it(resonators);
    Resonator *res;
    while(it.hasNext()) {
        res = it.next();
        delete res;
    }
    resonators.clear();
    selectedRows.clear();
}

void EditISODeviceTab::selectRow(int row)
{
    if(selectedRows.contains(row))
        selectedRows.removeOne(row);
    else
        selectedRows.append(row);
}
