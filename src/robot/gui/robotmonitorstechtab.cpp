#include "robotmonitorstechtab.h"

#include <QVariant>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include "../robot.h"
#include "../sensorsset.h"
#include "../robotledmonitor.h"
#include "../robotgraphmonitor.h"
#include "../../recmon/graphicmonitor.h"
#include "../../recmon/devicerecorder.h"
#include "../../neuraldis/settings.h"
#include "../robotmonitorstechnician.h"

/*
 *  Constructs a RobotMonitorsTechTab which is a child of 'parent', with the
 *  name 'name'.'
 */
RobotMonitorsTechTab::RobotMonitorsTechTab(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);

    connect(fileCheckBox, SIGNAL(toggled(bool)), this, SLOT(setVFileRec(bool)));
    connect(monCheckBox, SIGNAL(toggled(bool)), this, SLOT(setVMon(bool)));
    connect(timeupdSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTimeUpd(int)));
    connect(tLabelsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTLabels(int)));
    connect(minvel_linFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMinVelLin(double)));
    connect(maxvel_linFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMaxVelLin(double)));
    connect(minvel_radFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMinVelRad(double)));
    connect(maxvel_radFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMaxVelRad(double)));
    connect(vLinLabelsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setVLinLabels(int)));
    connect(vRadLabelsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setVRadLabels(int)));
    connect(vFileButton, SIGNAL(clicked()), this, SLOT(chooseVFile()));
    connect(vFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setVFile(QString)));
    connect(viewListBox, SIGNAL(currentRowChanged(int)), this, SLOT(setViewType(int)));
    connect(saveFileButton, SIGNAL(clicked()), this, SLOT(saveToFile()));
    connect(loadFileButton, SIGNAL(clicked()), this, SLOT(loadFromFile()));

}

/*
 *  Destroys the object and frees any allocated resources
 */
RobotMonitorsTechTab::~RobotMonitorsTechTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void RobotMonitorsTechTab::languageChange()
{
    retranslateUi(this);
}

void RobotMonitorsTechTab::
init(RobotMonitorsTechnician* m_Tech) {

    monsTech = m_Tech;
    setViewType(false);
    maxvel_lin = 1000000000;
    minvel_lin = -1000000000;
    maxvel_rad = 1000000000;
    minvel_rad = -1000000000;
    setTimeUpd(60);
    setTLabels(6);
    setMinVelLin(-100);
    setMaxVelLin(100);
    setVLinLabels(7);
    setMinVelRad(-1200);
    setMaxVelRad(1200);
    setVRadLabels(7);

    Settings* prog_settings = Settings::instance();
    setIniDir(prog_settings->getSimsDirectory());
    readVars();

}

bool RobotMonitorsTechTab::getVFileRec( void )
{
    return v_filerec;
}

void RobotMonitorsTechTab::setVFileRec(bool v_fr )
{
    v_filerec = v_fr;
    fileCheckBox->setChecked(v_filerec);
    if(v_filerec && vFileLineEdit->text().trimmed().isEmpty()) {
        chooseVFile();
    }
}


QString RobotMonitorsTechTab::getVFile( void)
{
    return vFile;
}

void  RobotMonitorsTechTab::setVFile( const QString& f )
{
    if(f.trimmed().isEmpty()) {
        if(vFile.trimmed().isEmpty()) {
            vFile = ini_dir + monsTech->robot->fullName() + "_" + tr("path");
            if(v_filerec ) {
//                QMessageBox::information(nullptr, tr("Edit recording..."),
//                                tr("File name for velocities recording is empty.") + "\n" + tr("Restoring previous or default name."), tr("&Return"));
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText(tr("Edit recording..."));
                msgBox.setInformativeText(tr("File name for velocities recording is empty.") + "\n" + tr("Restoring previous or default name."));
                QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
                msgBox.exec();
            }
        }
    }
    else vFile = f;
    vFileLineEdit->setText(vFile);
}

void RobotMonitorsTechTab::setIniDir(const QString& d)
{
    ini_dir = d;
}

void RobotMonitorsTechTab::chooseVFile(void )
{
    QString file;
    QFileDialog* fd;
    fd = new QFileDialog();
    fd->setWindowTitle(tr("Robot path recording file"));
    fd -> setDirectory(vFile);
    QStringList filters;
    filters << tr("Data") + " (*.dat)" << tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd -> setFileMode(QFileDialog::AnyFile);
    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();
        if(fileNames.size() == 0) {
            delete fd;
            return;
        }
        file = fileNames.at(0);
        if(file.section(QDir::toNativeSeparators("/"), -1).indexOf(".") == -1)
            file += ".dat";
        setVFile(file);
    }
    else {
        if(file.trimmed().isEmpty())
            setVFile(file);
    }
    delete fd;
}

bool RobotMonitorsTechTab::getVMon( void )
{
    return v_mon;
}

void RobotMonitorsTechTab::setVMon(bool v_m )
{
    v_mon = v_m;
    monCheckBox->setChecked(v_m);
}

int RobotMonitorsTechTab::getViewType(void)
{
    return view_type;
}

void  RobotMonitorsTechTab::setViewType(int vt)
{
    view_type = vt;
    viewListBox->setCurrentRow(vt);
}

int RobotMonitorsTechTab::getTimeUpd( void )
{
    return timeupd;
}

void RobotMonitorsTechTab::setTimeUpd( int tu)
{
    timeupd = tu;
    timeupdSpinBox->setValue(tu);
}

int RobotMonitorsTechTab::getTLabels( void )
{
    return tLabels;
}

void RobotMonitorsTechTab::setTLabels( int tl)
{
    tLabels = tl;
    tLabelsSpinBox->setValue(tl);
}

int RobotMonitorsTechTab::getVLinLabels( void )
{
    return vLinLabels;
}

void RobotMonitorsTechTab::setVLinLabels( int vl)
{
    vLinLabels = vl;
    vLinLabelsSpinBox->setValue(vl);
}

int RobotMonitorsTechTab::getVRadLabels( void )
{
    return vRadLabels;
}

void RobotMonitorsTechTab::setVRadLabels( int vr)
{
    vRadLabels = vr;
    vRadLabelsSpinBox->setValue(vr);
}

double RobotMonitorsTechTab::getMinVelLin( void )
{
    return minvel_linFloatSpinBox->value();
}

void RobotMonitorsTechTab::setMinVelLin(double d)
{

    if(v_mon ) {
        if(d >= maxvel_lin) {
//            QMessageBox::information(nullptr, tr("Edit monitoring..."),
//                            tr("Minimun linear velocity greater than maximun velocity"),  tr("&Return")) ;
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("Edit monitoring..."));
            msgBox.setInformativeText(tr("Minimun linear velocity greater than maximun velocity"));
            QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
            msgBox.exec();
            minvel_linFloatSpinBox->setFocus();
        }
        else
            minvel_lin = d;
    }
    else
        minvel_lin = d;
    minvel_linFloatSpinBox->setValue(minvel_lin);
}

double RobotMonitorsTechTab::getMaxVelLin( void )
{
    return maxvel_linFloatSpinBox->value();
}

void RobotMonitorsTechTab::setMaxVelLin( double d)
{
    if(v_mon ) {
        if(d <= minvel_lin) {
//            QMessageBox::information(nullptr, tr("Edit monitoring..."),
//                                     tr("Maximun linear velocity lesser than minimun velocity"),  tr("&Return"));
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("Edit monitoring..."));
            msgBox.setInformativeText(tr("Maximun linear velocity lesser than minimun velocity"));
            QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
            msgBox.exec();
            maxvel_linFloatSpinBox->setFocus();
        }
        else
            maxvel_lin= d;
    }
    else
        maxvel_lin = d;
    maxvel_linFloatSpinBox->setValue(maxvel_lin);
}

double RobotMonitorsTechTab::getMinVelRad( void )
{
    return minvel_radFloatSpinBox->value();
}


void RobotMonitorsTechTab::setMinVelRad( double d)
{
    if(v_mon ) {
        if(d >= maxvel_rad) {
//            QMessageBox::information(nullptr, tr("Edit monitoring..."),
//                                     tr("Minimun radial velocity greater than minimun velocity"),  tr("&Return"));
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("Edit monitoring..."));
            msgBox.setInformativeText(tr("Minimun radial velocity greater than minimun velocity"));
            QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
            msgBox.exec();
            minvel_radFloatSpinBox->setFocus();
        }
        else
            minvel_rad = d;
    }
    else
        minvel_rad = d;
    minvel_radFloatSpinBox->setValue(minvel_rad);
}


double RobotMonitorsTechTab::getMaxVelRad( void )
{
    return maxvel_radFloatSpinBox->value();
}

void RobotMonitorsTechTab::setMaxVelRad( double d)
{
    if(v_mon ) {
        if(d == minvel_rad) {
//            QMessageBox::information(nullptr, tr("Edit monitoring..."),
//                                     tr("Maximun radial velocity equals minimun velocity"),  tr("&Return"));
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("Edit monitoring..."));
            msgBox.setInformativeText(tr("Maximun radial velocity equals minimun velocity"));
            QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
            msgBox.exec();
            maxvel_radFloatSpinBox->setFocus();
        }
        else
            maxvel_rad = d;
    }
    else
        maxvel_rad = d;
    maxvel_radFloatSpinBox->setValue(maxvel_rad);
}

bool RobotMonitorsTechTab::validateThis( void )
{
    bool ret= true;
    if(v_filerec && vFileLineEdit->text().trimmed().isEmpty()) {
        //     setVFileRec(vFileLineEdit->text());
        ret = false;
    }
    if(v_mon && (minvel_linFloatSpinBox->value() >=
                 maxvel_linFloatSpinBox->value()))
        ret = false;
    if(v_mon && (minvel_radFloatSpinBox->value() >=
                 maxvel_radFloatSpinBox->value()))
        ret = false;
    return ret;
}

void RobotMonitorsTechTab::saveThis(void)
{

    bool change_vels_mon = false;
    bool old_hasVelsMon = monsTech->hasVelsMon;

    if(!old_hasVelsMon) {
        monsTech->defaultVelsMonitors();
        monsTech->constructGraphMons();
        change_vels_mon = true;
    }
    if(monsTech->hasVelsFile != getVFileRec())
        change_vels_mon = true;
    if(monsTech->file_vels_name != getVFile())
        change_vels_mon = true;
    if(monsTech->hasVelsMon != getVMon())
        change_vels_mon = true;
    if(bool(monsTech->velocmonitor->getBackColor()) != getViewType())
        change_vels_mon = true;
    if(monsTech->velocmonitor->getXUpd() != getTimeUpd())
        change_vels_mon = true;
    if(monsTech->velocmonitor->getNXLabels() != getTLabels())
        change_vels_mon = true;
    if(monsTech->velocmonitor->getYMin() != getMinVelLin())
        change_vels_mon = true;
    if(monsTech->velocmonitor->getYMax() != getMaxVelLin())
        change_vels_mon = true;
    if(monsTech->velocmonitor->getNYLabels() != getVLinLabels())
        change_vels_mon = true;
    if(monsTech->rotmonitor->getYMin() != getMinVelRad())
        change_vels_mon = true;
    if(monsTech->rotmonitor->getYMax() != getMaxVelRad())
        change_vels_mon = true;
    if(monsTech->rotmonitor->getNYLabels() != getVRadLabels())
        change_vels_mon = true;
    
    if(change_vels_mon)
        writeThis();

}

void RobotMonitorsTechTab::writeThis(void)
{
    monsTech->hasVelsFile = getVFileRec();
    monsTech->file_vels_name = getVFile();

    monsTech->hasVelsMon = getVMon();

    QDomDocument doc_mon("");
    QDomElement root = doc_mon.createElement("Graphic_monitor_params");
    doc_mon.appendChild(root);
    monsTech->writeVelMonsParams(root, 1, getTimeUpd(), getTLabels(),
                                 getMinVelLin(), getMaxVelLin(),
                                 getVLinLabels(), "", bool(getViewType()));
    monsTech->setLinearMonParams(root);
    monsTech->writeVelMonsParams(root, 1, getTimeUpd(), getTLabels(),
                                 getMinVelRad(), getMaxVelRad(),
                                 getVRadLabels(), "", bool(getViewType()));
    monsTech->setRadialMonParams(root);

}

void RobotMonitorsTechTab::saveToFile(void)
{
    if(validateThis()) {
//        if(QMessageBox::information(nullptr, tr("Saving monitor and recorder"), tr("This operation cannot be cancelled later"), tr("&Continue"), tr("Abort")) == 1)
          if(QMessageBox::information(nullptr, tr("Saving monitor and recorder"), tr("This operation cannot be cancelled later"), QMessageBox::Save | QMessageBox::Abort) == QMessageBox::Abort)
            return;
        saveThis();
        monsTech->save();
    }

}


void RobotMonitorsTechTab::loadFromFile( void )
{
    if( monsTech->setFile(tr("Untitled") + "." + tr("robmon"))) {
        monsTech->constructGraphMons();
        monsTech->constructRecorder();
        readVars();
    }
}

void RobotMonitorsTechTab::readVars( void )
{
    v_filerec = monsTech->hasVelsFile; // necesario para setVFile()
    setVFile(monsTech->file_vels_name);
    setVFileRec(monsTech->hasVelsFile); // necesario para checkBox

    setVMon(monsTech->hasVelsMon);
    velsGroupBox->setEnabled(monsTech->hasVelsMon);
    if(monsTech->hasVelsMon) {
        setViewType(bool(monsTech->velocmonitor->getBackColor()));
        setTimeUpd(monsTech->velocmonitor->getXUpd());
        setTLabels(monsTech->velocmonitor->getNXLabels());

        setMinVelLin(monsTech->velocmonitor->getYMin());
        setMaxVelLin(monsTech->velocmonitor->getYMax());
        setVLinLabels(monsTech->velocmonitor->getNYLabels());

        setMinVelRad(monsTech->rotmonitor->getYMin());
        setMaxVelRad(monsTech->rotmonitor->getYMax());
        setVRadLabels(monsTech->rotmonitor->getNYLabels());
    }
}

void RobotMonitorsTechTab::getExternVal(const QString& var_name, const QVariant& var_val )
{
    bool ok = false;
    if(var_name == "robot_max_vel") {
        double vel = var_val.toInt(&ok);
        if(ok) {
            maxvel_linFloatSpinBox->setMaximum(vel);
            maxvel_linFloatSpinBox->setMinimum(-vel);
            minvel_linFloatSpinBox->setMaximum(vel);
            minvel_linFloatSpinBox->setMinimum(-vel);
        }
    }
}
