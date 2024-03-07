//
// C++ Interface: basesimulationdialog
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "basesimulationdialog.h"

#include <QVariant>
#include <QMessageBox>
#include <QDir>
#include <QButtonGroup>

#include "../../sim/basesimulation.h"
#include "../../control/basesimcontrol.h"
#include "../../sim/clocksdispatcher.h"

/*
 *  Constructs a baseSimulationDialog which is a child of 'parent', with the
 *  name 'name'.'
 */
baseSimulationDialog::baseSimulationDialog(QWidget* parent, const char* name)
    : QewTreeViewDialog(parent, name)
{
    setupUi(this);
    QewListView->header()->setWindowTitle(tr( "Dialogs" ) );
    QewListView->clear();
    QTreeWidgetItem * item = new QTreeWidgetItem( QewListView, 0 );
    item->setText( 0, tr( "Simulation" ) );

    simsButtonGroup = new QButtonGroup(this);
    simsButtonGroup->addButton(realtRadioButton, 0);
    simsButtonGroup->addButton(slowmRadioButton, 1);
    simsButtonGroup->addButton(smaxRadioButton, 2);

    clockResetButtonGroup = new QButtonGroup(this);
    clockResetButtonGroup->addButton(continueRadioButton, 0);
    clockResetButtonGroup->addButton(resetRadioButton, 1);

    supButtonGroup = new QButtonGroup(this);
    supButtonGroup->addButton(contRadioButton, 0);
    supButtonGroup->addButton(everyRadioButton, 1);

    QObject::connect(clockResetButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(setClockReset(int)));
    QObject::connect(durationTimeEdit, SIGNAL(timeChanged(QTime)),
                     this, SLOT(setDuration(QTime)));
    QObject::connect(everySpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(setScrUpdate(int)));
    QObject::connect(simsButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(setTypeSimSpeed(int)));
    QObject::connect(slowmSpinBox, SIGNAL(valueChanged(int)), this,
                     SLOT(setSimSpeed(int)));
    QObject::connect(supButtonGroup, SIGNAL(buttonClicked(int)), this,
                     SLOT(setTypeScrUpdate(int)));
    QObject::connect(adv_periodSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(setAdvPeriod(int)));


}

/*
 *  Destroys the object and frees any allocated resources
 */
baseSimulationDialog::~baseSimulationDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void baseSimulationDialog::languageChange()
{
    retranslateUi(this);
}

void baseSimulationDialog::init(BaseSimulation *simul) {

    sim = simul;
    simname = sim->getFileName().section(QDir::toNativeSeparators("/"), -1).
            section('.', 0, 0 );
    setWindowTitle(windowTitle() + simname);
    advance_period = 20;
    every_tenth = 0;
    type_scr_upd = 0;
    setAdvPeriod(sim->real_advance_period);
    setTypeSimSpeed(sim->type_sim_advper);
    setSimSpeed(sim->sim_advance_period);
    setClockReset(int(sim->crono_reset));
    setTypeScrUpdate(sim->type_scr_update);
    setScrUpdate(sim->scr_update);
    setIniDir(sim->prog_settings->getNetDirectory());
    setSimsDir(sim->prog_settings->getSimsDirectory());
    QTime dur(0, 0);
    dur = dur.addMSecs(sim->duration);
    QString s_d = dur.toString();
    setDuration(dur);
    durationTimeEdit->setTime(dur);

}

void baseSimulationDialog::setSimsDir(const QString& d)
{
    simulation_dir = d;
    emit  emitExternVal("simulation_dir", QVariant(simulation_dir));
}

int baseSimulationDialog::getAdvPeriod(void)
{
    return advance_period;
}

void  baseSimulationDialog::setAdvPeriod(int ap)
{
    advance_period = ap;
    adv_periodSpinBox->setValue(ap);
    if(type_scr_upd)
        setScrUpdate(every_tenth);
    emit  emitExternVal("advance_period", QVariant(advance_period));
}

int baseSimulationDialog::getSimSpeed( void)
{
    return sim_speed;
}

void  baseSimulationDialog::setSimSpeed(int s)
{
    sim_speed = s;
    slowmSpinBox->setValue(s);
}

int baseSimulationDialog::getTypeSimSpeed( void)
{
    return type_sim_speed;
}

void  baseSimulationDialog::setTypeSimSpeed(int s)
{
    type_sim_speed = s;
    simsButtonGroup->buttons().at(s)->setChecked(true);
}

int baseSimulationDialog::getScrUpdate( void)
{
    return every_tenth;
}

void  baseSimulationDialog::setScrUpdate(int s)
{
    if(type_scr_upd)
        if(!verifyPeriodScrUpd(s))
            setTypeScrUpdate(0);
    everySpinBox->setValue(every_tenth);
}

bool baseSimulationDialog::getClockReset( void )
{
    return clockReset;
}

void baseSimulationDialog::setClockReset( int c)
{
    clockReset = bool(c);
    clockResetButtonGroup->buttons().at(c)->setChecked(true);
    return;
}

int baseSimulationDialog::getTypeScrUpdate( void)
{
    return type_scr_upd;
}

void  baseSimulationDialog::setTypeScrUpdate(int s)
{
    type_scr_upd = s;
    supButtonGroup->buttons().at(s)->setChecked(true);
    if(type_scr_upd) setScrUpdate(every_tenth);

}

void  baseSimulationDialog::setIniDir( const QString& d)
{
    if(simname.isEmpty())
        simname = tr("Untitled_sim");
    QString f_name = simname + "_" + tr("path" );
    ini_dir = d + f_name;
}

bool baseSimulationDialog::validateThis( void )
{
    bool ret= true;
    if(type_scr_upd && ((everySpinBox->text().toInt()*100)%adv_periodSpinBox->cleanText().toInt()))
        ret = false;
    return ret;
}

bool baseSimulationDialog::verifyPeriodScrUpd(int period)
{
    int r, q;
    bool ret = false;
    r = (100*period) % advance_period;
    q = (100*period) / advance_period;
    if(!r) {
        every_tenth = period;
        ret = true;
    }
    else {
        if(type_scr_upd) {
//            QMessageBox::information(nullptr, tr("Edit recording..."),
//                                     tr("Screen update period") +" (" + QString::number(period) + ") " + tr("multiplied by 100") + "\n" + tr("must be multiple of advance period") +" (" + QString::number(advance_period)  + ").\n\n" + tr("Changing to continuos mode."), tr("&Return"));
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("Edit recording..."));
            msgBox.setInformativeText(tr("Screen update period") +" (" + QString::number(period) + ") " + tr("multiplied by 100") + "\n" + tr("must be multiple of advance period") +" (" + QString::number(advance_period)  + ").\n\n" + tr("Changing to continuos mode."));
            QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
            msgBox.exec();
            type_scr_upd = 0;
            every_tenth = 0;
        } else {
            every_tenth = period;
            ret = true;
        }
    }
    return ret;
}


void baseSimulationDialog::saveThis(void)
{
    sim->real_advance_period = getAdvPeriod();
    sim->control->setSamplingPeriod(sim->real_advance_period/1000.0);
    sim->advance_timer->setRealAdvPeriod(sim->real_advance_period);
    sim->type_sim_advper = getTypeSimSpeed();
    sim->sim_advance_period = getSimSpeed();
    sim->type_scr_update = getTypeScrUpdate();
    sim->scr_update = getScrUpdate();
    sim->crono_reset = getClockReset();
    sim->duration = getDuration();
}

void baseSimulationDialog::setDuration( const QTime & d )
{
    QString s_d = d.toString();
    duration = d;
    //  durationTimeEdit->setTime(d);

}


int baseSimulationDialog::getDuration( void )
{
    duration = durationTimeEdit->time();
    QTime dur(0,0,0);
    return dur.msecsTo(duration);
}

QStringList baseSimulationDialog::designedCaptions( void )
{
    QStringList cap_list;
    cap_list << tr("General");
    return cap_list;
}
