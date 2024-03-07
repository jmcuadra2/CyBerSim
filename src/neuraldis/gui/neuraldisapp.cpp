#include "neuraldisapp.h"

#include <QVariant>
#include <QtXml>
#include <QDir>
#include <QFile>
#include <QStringList>
//#include <QTextCodec>
#include <QStatusBar>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

//#include "../../robot/gui/irsmonitor.h"
//#include "../../robot/gui/lightmonitor.h"
#include "../../net/netbuilder.h"
#include "../../sim/simulationbuilder.h"
#include "../exports2fig.h"
#include "../../world/abstractworld.h"
//#include "../../world/world.h"
#include "../../world/rtti.h"
#include "../../sim/basesimulation.h"
#include "../../world/worldfactory.h"
//#include "../../robot/robotbuilder.h"
#include "../../world/worldview.h"
#include "../../disview/gui/textviewer.h"
#include "../../disview/leyend.h"

/*
 *  Constructs a NeuralDisApp as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
NeuralDisApp::NeuralDisApp(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    processLaunched = "";

    connect(fileClose, SIGNAL(triggered(bool)), this, SLOT(slotFileClose()));
    connect(fileNew, SIGNAL(triggered(bool)), this, SLOT(slotFileNew()));
    connect(fileOpen, SIGNAL(triggered(bool)), this, SLOT(slotFileOpen()));
    connect(filePrint, SIGNAL(triggered(bool)), this, SLOT(slotFilePrint()));
    connect(fileQuit, SIGNAL(triggered(bool)), this, SLOT(slotFileQuit()));
    connect(fileSave, SIGNAL(triggered(bool)), this, SLOT(slotFileSave()));
    connect(fileSaveAs, SIGNAL(triggered(bool)), this, SLOT(slotFileSaveAs()));
    connect(fileTestXML, SIGNAL(triggered(bool)), this, SLOT(slotTestXML()));
    connect(netExport, SIGNAL(triggered(bool)), this, SLOT(slotNetExport()));
    connect(pathExport, SIGNAL(triggered(bool)), this, SLOT(slotPathExport()));
    connect(worldExport, SIGNAL(triggered(bool)), this, SLOT(slotWorldExport()));
    connect(actionScilabExport, SIGNAL(triggered(bool)), this, SLOT(slotScilabExport()));
    connect(actionFixWorld, SIGNAL(triggered(bool)), this, SLOT(slotFixWorld()));

    connect(toolDis, SIGNAL(triggered(QAction*)), this, SLOT(setTool(QAction*)));
    connect(disAddNeuron, SIGNAL(triggered(bool)), this, SLOT(slotAddNeuron()));
    connect(disAddSynapse, SIGNAL(triggered(bool)), this, SLOT(slotAddSynapse()));
    connect(disClearNetwork, SIGNAL(triggered(bool)), this, SLOT(slotClearNet()));
    connect(disCreateNetwork, SIGNAL(triggered(bool)), this, SLOT(slotNewNet()));
    connect(disDelNeuron, SIGNAL(triggered(bool)), this, SLOT(slotDelNeuron()));
    connect(disDelSynapse, SIGNAL(triggered(bool)), this, SLOT(slotDelSynapse()));
    connect(disEditNeuron, SIGNAL(triggered(bool)), this, SLOT(slotEditNeuron()));
    connect(disEditSynapse, SIGNAL(triggered(bool)), this, SLOT(slotEditSynapse()));

    connect(editCopy, SIGNAL(triggered(bool)), this, SLOT(slotEditCopy()));
    connect(editCut, SIGNAL(triggered(bool)), this, SLOT(slotEditCut()));
    connect(editPaste, SIGNAL(triggered(bool)), this, SLOT(slotEditPaste()));
    connect(editUndo, SIGNAL(triggered(bool)), this, SLOT(slotEditUndo()));
    connect(editRedo, SIGNAL(triggered(bool)), this, SLOT(slotEditRedo()));
    connect(editSettings, SIGNAL(triggered(bool)), this, SLOT(slotEditSettings()));
    connect(saveSettings, SIGNAL(triggered(bool)), this, SLOT(slotSaveSettings()));

    connect(viewDesignerBar, SIGNAL(toggled(bool)), this, SLOT(slotViewDesignerBar(bool)));
    connect(viewLeyend, SIGNAL(triggered(bool)), this, SLOT(slotViewLeyend()));
    //     connect(viewSensors, SIGNAL(triggered(bool)), this, SLOT(slotViewSensors()));
    connect(actionShowAllSensors, SIGNAL(triggered(bool)), this, SLOT(slotShowAllSensors()));
    connect(actionHideAllSensors, SIGNAL(triggered(bool)), this, SLOT(slotHideAllSensors()));
    connect(viewStatusBar, SIGNAL(toggled(bool)), this, SLOT(slotViewStatusBar(bool)));
    connect(viewToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewToolBar(bool)));
    connect(viewViewRawText, SIGNAL(triggered(bool)), this, SLOT(slotViewRawText()));

    connect(disDelNetwork, SIGNAL(triggered(bool)), this, SLOT(slotDelNetwork()));
    connect(disAddNetwork, SIGNAL(triggered(bool)), this, SLOT(slotAddNetwork()));
    connect(toolAddNetwork, SIGNAL(triggered(bool)), this, SLOT(slotToolAddNetwork()));
    connect(toolAddNeuron, SIGNAL(triggered(bool)), this, SLOT(slotToolAddNeuron()));
    connect(toolAddSynapse, SIGNAL(triggered(bool)), this, SLOT(slotToolAddSynapse()));
    connect(toolDelNetwork, SIGNAL(triggered(bool)), this, SLOT(slotToolDelNetwork()));
    connect(toolDelNeuron, SIGNAL(triggered(bool)), this, SLOT(slotToolDelNeuron()));
    connect(toolDelSynapse, SIGNAL(triggered(bool)), this, SLOT(slotToolDelSynapse()));
    connect(toolEditNeuron, SIGNAL(triggered(bool)), this, SLOT(slotToolEditNeuron()));
    connect(toolEditSynapse, SIGNAL(triggered(bool)), this, SLOT(slotToolEditSynapse()));
    connect(disEditNetwork, SIGNAL(triggered(bool)), this, SLOT(slotEditNetwork()));
    connect(toolEditNetwork, SIGNAL(triggered(bool)), this, SLOT(slotToolEditNetwork()));
    connect(viewShowNetView, SIGNAL(triggered(bool)), this, SLOT(slotViewShowNetView()));
    connect(toolNetView, SIGNAL(triggered(bool)), this, SLOT(slotToolNetView()));

    connect(toolSim, SIGNAL(triggered(QAction*)), this, SLOT(setToolSim(QAction*)));
    connect(simNew, SIGNAL(triggered(bool)), this, SLOT(slotSimNew()));
    connect(simClose, SIGNAL(triggered(bool)), this, SLOT(slotSimClose()));
    connect(simEdit, SIGNAL(triggered(bool)), this, SLOT(slotSimEdit()));
    connect(simLoad, SIGNAL(triggered(bool)), this, SLOT(slotSimLoad2()));
    connect(simRecord, SIGNAL(toggled(bool)), this, SLOT(slotSimRecord(bool)));
    connect(simSave, SIGNAL(triggered(bool)), this, SLOT(slotSimSave()));
    connect(simStep, SIGNAL(triggered(bool)), this, SLOT(slotSimStep()));
    connect(actionDrive_robot, SIGNAL(toggled(bool)), this, SLOT(slotDriveRobot(bool)));
    connect(editWorld, SIGNAL(triggered(bool)), this, SLOT(slotEditWorld()));
    connect(closeWorld, SIGNAL(triggered(bool)), this, SLOT(slotCloseWorld()));
    connect(createWorld, SIGNAL(triggered(bool)), this, SLOT(slotCreateWorld()));
    connect(loadWorld, SIGNAL(triggered(bool)), this, SLOT(slotLoadWorld()));
    connect(worldRestoreZoomAction, SIGNAL(triggered(bool)), this, SLOT(slotWorldRestoreZoom()));
    connect(zoomIn, SIGNAL(triggered(bool)), this, SLOT(slotZoomIn()));
    connect(zoomOut, SIGNAL(triggered(bool)), this, SLOT(slotZoomOut()));
    connect(worldCenterInRobotAction, SIGNAL(toggled(bool)), this, SLOT(slotCenterInRobot(bool)));
    connect(worldScaleAction, SIGNAL(triggered(bool)), this, SLOT(slotWorldScale()));
    connect(actionRotateView, SIGNAL(triggered(bool)), this, SLOT(slotWorldRotate()));
    connect(actionDefault_materials, SIGNAL(triggered(bool)), this, SLOT(slotDefaultMaterials()));

    connect(actionFilm_simulation, SIGNAL(toggled(bool)), this, SLOT(slotSimFilm(bool)));
    connect(actionRecord_sensors, SIGNAL(toggled(bool)), this, SLOT(slotRecordSensors(bool)));

    connect(actionSaveWorld, SIGNAL(triggered(bool)), this, SLOT(slotSaveWorld()));


    connect(helpAPIAction, SIGNAL(triggered(bool)), this, SLOT(slotApiManual()));
    connect(helpAboutApp, SIGNAL(triggered(bool)), this, SLOT(slotHelpAbout()));
    connect(helpManualApp, SIGNAL(triggered(bool)), this, SLOT(slotHelpManual()));

    (void)statusBar();
}

/*
 *  Destroys the object and frees any allocated resources
 */
NeuralDisApp::~NeuralDisApp()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void NeuralDisApp::languageChange()
{
    retranslateUi(this);
}

/*! \class NeuralDisApp
 *   \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */

/*! \fn init(Settings *prog_set)
 *   \brief Inicializa.
 *         Brief description continued.
 *
 *  Inicializa el widget.
 */
/* nuevo include */
/*#include "xmloperator.h"*/


void NeuralDisApp::init(Settings *prog_set)
{

    setWindowTitle(tr("CyBeRSim"));

    // call inits to invoke all other construction parts
    prog_settings = prog_set;
    help = 0;
    helpAPI = 0;
    doc = 0;
    net = 0;
    view = 0;
    world = 0;
    sim = 0;
    browser_prog = 0;
    browser_api = 0;

    QList<QGraphicsScene *> *pCanvasList = new QList<QGraphicsScene *>;
    prog_settings->setCanvasList(pCanvasList);

    initDoc();
    initView();
    initHelp();

    initMenuBar();
    initToolBar();
    initStatusBar();
    initDesignerBar();
    initSimulatorBar();

    viewToolBar->setChecked(true);
    viewStatusBar->setChecked(true);
    viewDesignerBar->setChecked(true);
    slotEnableDisMenu(false);
    slotEnableSimMenu();
    menuShow_sensors->setEnabled(false);

}

void NeuralDisApp::destroy()
{
    browser_api->deleteLater();
    browser_prog->deleteLater();
}

void NeuralDisApp::initMenuBar()
{

    connect(pWindowMenu, SIGNAL(aboutToShow()), this, SLOT(windowMenuAboutToShow()));

}

void NeuralDisApp::slotEnableDisMenu(bool enabl)
{

    viewLeyend->setEnabled(enabl);
    viewViewRawText->setEnabled(enabl);
    /* nueva opción para vistas de red */
    viewShowNetView->setEnabled(false);
    disClearNetwork->setEnabled(enabl);
    disDelNeuron->setEnabled(enabl);
    disEditNeuron->setEnabled(enabl);
    disAddSynapse->setEnabled(enabl);
    disDelSynapse->setEnabled(enabl);
    disEditSynapse->setEnabled(enabl);

    toolPointer->setEnabled(enabl);
    toolDelNeuron->setEnabled(enabl);
    toolEditNeuron->setEnabled(enabl);
    toolAddSynapse->setEnabled(enabl);
    toolDelSynapse->setEnabled(enabl);
    toolEditSynapse->setEnabled(enabl);
    toolAddNetwork->setEnabled(false);
    toolDelNetwork->setEnabled(false);
    toolEditNetwork->setEnabled(false);
    toolNetView->setEnabled(false);
    fileSave->setEnabled(enabl);
    fileSaveAs->setEnabled(enabl);
    filePrint->setEnabled(enabl);
    netExport->setEnabled(enabl);

}

void NeuralDisApp::slotEnableSimMenu(void)
{

    if(sim) {
        simLoad->setEnabled(false);
        simNew->setEnabled(false);
        slotEnableSimCommonMenu(true);
    }
    else {
        simLoad->setEnabled(true);
        simNew->setEnabled(true);
        slotEnableSimCommonMenu(false);
    }

}

void NeuralDisApp::slotEnableSimCommonMenu(bool on)
{

    simEdit->setEnabled(on);
    simSave->setEnabled(on);
    simClose->setEnabled(on);
    simGo->setEnabled(on);
    simStop->setEnabled(on);
    simRecord->setEnabled(on);
    toolSim->setEnabled(on);
    simulatorBar->setEnabled(on);

}
void NeuralDisApp::initToolBar()
{
    fileToolbar->show();
}

void NeuralDisApp::initDesignerBar()
{
    designerBar->show();
}

void NeuralDisApp::initSimulatorBar()
{
    simulatorBar->setEnabled(false);
    simulatorBar->show();
}


void NeuralDisApp::initStatusBar()
{
    statusBar()->showMessage(tr("Ready."), 2000);
}

void NeuralDisApp::initHelp()
{
    // execute in QTCreator with Run in terminal option
    //QLocale::setDefault(QLocale("es_ES"));
    QString local(QLocale().bcp47Name());
//    std::cout << local.toStdString() + "\n";
    local = local.section('_', 0, 0 );
    manual_dir = prog_settings->getAppDirectory() + QDir::toNativeSeparators("/docs/user/") +
                            local + QDir::toNativeSeparators("/html/");
    if(prog_settings->getHelpCommand() == "CyBeRSim")
        help = new HelpWindow(manual_dir + "index.html");
    else
        browser_prog = new QProcess(this);
}

void NeuralDisApp::initApi()
{
    QString local(QLocale().bcp47Name());
    local = local.section('_', 0, 0 );
    api_dir = prog_settings->getAppDirectory() +
                        QDir::toNativeSeparators("/docs/api/doxygen/html/");
    if(prog_settings->getHelpCommand() == "CyBeRSim")
        helpAPI = new HelpWindow(api_dir + "index.html");
    else
        browser_api = new QProcess(this);
}

void NeuralDisApp::initDoc()
{
    doc = NeuralDisDoc::instance();
    connect(this, SIGNAL(isInitView(void)), doc, SLOT(initView()));

}

void NeuralDisApp::initView()
{
    // set the main widget here
    pWorkspace = new QMdiArea(this);
    prog_settings->setWorkspace(pWorkspace);
    pWorkspace->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    pWorkspace->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(pWorkspace);

    connect(windowCascade, SIGNAL(triggered(bool)), pWorkspace, SLOT(cascadeSubWindows()));
}

bool NeuralDisApp::queryExit(void)
{
    int exit=QMessageBox::information(this, tr("Quit..."),
                                      tr("Do your really want to quit?"),
                                      QMessageBox::Ok, QMessageBox::Cancel);
    return (exit==QMessageBox::Ok);
}

bool NeuralDisApp::queryClose(const QString& func)
{

    view = prog_settings->getNetView();
    if(sim)
    {
        if(sim->isRunning())
            return false;
        else if(sim->hasChanged()) {
//            if(QMessageBox::information(nullptr, func,
//                                        "\t" + tr("Changes in simulation will be lost.") + "\n\t\t" + tr("Continue?"),
//                                        QMessageBox::Ok, QMessageBox::Cancel) == 1)
            if(QMessageBox::information(nullptr, func,
                                         "\t" + tr("Changes in simulation will be lost.") + "\n\t\t" + tr("Continue?"),
                                         QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
                return false;
        }
    }
    int opt = 0;
    if(func == "File close" && !view)
        return false;
    if(doc->isModified() )
    { // TODO QMessageBox
//        opt = QMessageBox::information(nullptr, func,
//                                       "\t" + tr("File has been modified") + "\n\t\t" + tr("Save?"),
//                                       tr("&Ok"), tr("&No"), tr("&Cancel"), 0, 2);
        opt = QMessageBox::information(nullptr, tr("function"),
                                       "\t" + tr("File has been modified") + "\n\t\t" + tr("Save?"), QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Ok);
        // quitar esto es para comprobar la operación sobre la red y ver si se guarda el cambio
//        if(opt == 0)
        if(opt == QMessageBox::Ok)
        {
            if(!slotFileSave()){
                opt = 2;
            }
        }
//        else if(opt == 1)
        else if(opt == QMessageBox::No)
        {
            if(view)
                view->getMdiSubWindow()->hide();
            doc->setModified(false);
        }
    }

//    if(opt != 2)
    if(opt != QMessageBox::Cancel)
    {
        setWindowTitle(tr("CyBeRSim" ));
        if(view) {
            view->getMdiSubWindow()->hide();
            view->simConnection(false);
        }
        if(sim)
            slotSimClose();
        if(func != "File quit") {
            prog_settings->deleteNet();
            net = 0; // habría que centralizar o informar del delete de net
            prog_settings->deleteNetView();
            view = 0;// habría que centralizar o informar del delete de view
            doc->newDoc();
            doc->setModified(false);
            viewViewRawText->setText(tr("Show raw text"));
            viewLeyend->setText(tr("Show weights leyend"));
        }
    }
    return !(opt == QMessageBox::Cancel);
}
/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void NeuralDisApp::slotFileNew()
{

    if(net) {
        if(net->notEmpty()) {
//            if(QMessageBox::warning(nullptr,tr("File new"),
//                               tr("This operation will clear the net, continue ?"),
//                                    tr("&Ok"), tr("&Cancel")) == 1)
            if(QMessageBox::warning(nullptr,tr("File new"),
                                 tr("This operation will clear the net, continue ?"),
                                 QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
                return;
        }
    }

    if(queryClose("File new")) {
        statusBar()->showMessage(tr("Creating new file..."));
        doc->newDoc();
        doc->setModified(true);
        prog_settings->deleteNet();
        prog_settings->deleteNetView();
        NetBuilder* netBuilder = new NetBuilder();
        net = netBuilder->newNet();
        if(!net) {
            statusBar()->showMessage(tr("Aborted."));
            return;
        }
        view = prog_settings->getNetView();
        netBuilder->setView(view);
        delete netBuilder;

        connectNetViewElements();
        view->update();

        statusBar()->showMessage(tr("Ready."));
    }
}

void NeuralDisApp::connectNetViewElements()
{
    connect(view, SIGNAL(closeView(void)),
            this, SLOT(slotFileClose(void)));
    connect(view, SIGNAL(enableMenu(bool)),
            this, SLOT(slotEnableDisMenu(bool)));
    connect(view->getLeyend(), SIGNAL(closeLeyend(void)),
            this, SLOT(slotViewLeyend(void)));
    connect(view->getTextViewer(), SIGNAL(closeViewer(void)),
            this, SLOT(slotViewRawText(void)));
    view->netViewConnected();
}

void NeuralDisApp::slotFileOpen()
{

    statusBar()->showMessage(tr("Opening file..."));
    QFileDialog* fd;
    QString fileName;
    fd = new QFileDialog(this);
    fd->setWindowTitle(tr("Net load"));
    fd->setDirectory(prog_settings->getNetDirectory());

    QStringList filters;
    QStringList fileNames;
    filters << tr("Nets") + " (" + prog_settings->getNetExtension() + ")" <<
               tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd->setFileMode(QFileDialog::ExistingFile);
    if ( fd->exec() == QDialog::Accepted )
        fileNames = fd->selectedFiles();
    delete fd;

    if (fileNames.size() != 0) {
        if(!queryClose("File open"))
            return;

        fileName = fileNames.at(0);
        if(doc->load(fileName)) {
            NetBuilder* netBuilder = new NetBuilder();
            net = netBuilder->construct();
            if(!net) {
                statusBar()->showMessage(tr("Aborted."));
                return;
            }
            net->setName(fileName);
            view = prog_settings->getNetView();

            netBuilder->setView(view);
            connectNetViewElements();

            slotEnableSimMenu();
            delete netBuilder;

            view->update();
        }
    }
    else
        statusBar()->showMessage(tr("Opening aborted"), 2000);

}

// nuevo slotfileopen pasándole cadena
void NeuralDisApp::slotSubNetOpen(QString file)
{

    statusBar()->showMessage(tr("Opening file..."));

    QFileDialog* fd;
    fd = new QFileDialog(this);
    fd->setDirectory(prog_settings->getNetDirectory());
    QStringList filters;
    filters << tr("Nets") + " (" + prog_settings->getNetExtension() + ")" <<
               tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd->setFileMode(QFileDialog::ExistingFile);
    fd->selectFile(file);
    QStringList fileNames;
    if ( fd->exec() == QDialog::Accepted )
        fileNames = fd->selectedFiles();
    delete fd;

    if (fileNames.size() == 0) {
        if(!queryClose("File open"))
            return;

        file = fileNames.at(0);
        if(doc->load(file)) {
            NetBuilder* netBuilder = new NetBuilder();
            net = netBuilder->construct();
            if(!net) {
                statusBar()->showMessage(tr("Aborted."));
                return;
            }
            net->setName(file);
            view = prog_settings->getNetView();
            connectNetViewElements();
            netBuilder->setView(view);
            slotEnableSimMenu();
            delete netBuilder;

            view->update();
        }
    }
    else
        statusBar()->showMessage(tr("Opening aborted"), 2000);

}


bool NeuralDisApp::slotFileSave()
{

    bool ret = false;
    if(!doc->isModified()) {
        statusBar()->showMessage(tr("File not modified"), 2000);
        return (ret = true);
    }
    statusBar()->showMessage(tr("Saving file..."));
    ret = doc->saveNet();

    if (ret) {
        setWindowTitle(doc->getFileName());
        if(view)
            view->setWindowTitle(view->windowTitle().section(":", 0, 0) + ": " +
                             doc->getFileName().section(QDir::toNativeSeparators("/"), -1));
        statusBar()->showMessage(tr("Ready."));
    }
    else
        statusBar()->showMessage(tr("Saving aborted"), 2000);
    return  ret;

}

bool NeuralDisApp::slotFileSaveAs()
{
    bool ret = false;
    QString f_name;
    statusBar()->showMessage(tr("Saving file under new filename..."));

    QString file = QFileDialog::getSaveFileName(this, tr("Save As"),
                        prog_settings->getNetDirectory(),
                        tr("Nets") + " (" + prog_settings->getNetExtension() + ");;"
                        + tr("All") + "(*.*)");
    if (!file.isEmpty()) {
        if(file.section(QDir::toNativeSeparators("/"), -1).indexOf(".") == -1)
            file += prog_settings->getNetExtension().section("*", -1);
        if(doc->saveAs(file)) {
            statusBar()->showMessage(tr("Ready."));
            f_name = doc->getFileName();
            setWindowTitle(f_name);
            doc->setModified(false);
            if(view)
                view->setWindowTitle(view->windowTitle().section(":", 0, 0) + ": " +
                                 f_name.section(QDir::toNativeSeparators("/"), -1));
            ret = true;
        }
    }
    else {
        statusBar()->showMessage(tr("Saving aborted"), 2000);
    }

    statusBar()->showMessage(tr("Ready."));
    return ret;
}

void NeuralDisApp::slotFileClose()
{

    // da error el queryClose
    if(!queryClose("File close"))
        return;

    statusBar()->showMessage(tr("Closing file..."));;
    toolPointer->setChecked(true);
    if(view)
        view->setTool(0);
    slotSimClose();
    prog_settings->deleteNetView();

    slotEnableSimMenu();
    doc->newDoc();
    doc->setModified(false);
    statusBar()->showMessage(tr("Ready."));

}

void NeuralDisApp::slotFilePrint()
{
    statusBar()->showMessage(tr("Printing..."));
    QPrinter printer;
    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {
        QPainter painter;
        painter.begin(&printer);

        // TODO: Define printing by using the QPainter methods here

        painter.end();
    };

    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::slotNetExport()
{

    QString preambulo, file_name;
    QFile file_fig;
    QFileDialog* fd;

    statusBar()->showMessage(tr("Exporting net..."));
    if(net) {
        if(!net->notEmpty())
        {
//            QMessageBox::information(nullptr, tr("Export fig"),
//                                     tr("Empty net, nothing to export"), tr("&Ok"));
            QMessageBox::information(nullptr, tr("Export fig"),
                                     tr("Empty net, nothing to export"), QMessageBox::Ok);
            statusBar()->showMessage(tr("Aborted."), 2000);
            return;
        }
    }
    else {
//        QMessageBox::information(nullptr, tr("Export fig"),
//                                 tr("No net loaded, nothing to export"), tr("&Ok"));
        QMessageBox::information(nullptr, tr("Export fig"),
                                 tr("No net loaded, nothing to export"), QMessageBox::Ok);
        statusBar()->showMessage(tr("Aborted."), 2000);
        return;
    }

    fd = new QFileDialog(this);
    fd->setDirectory(prog_settings->getNetDirectory());
    QStringList filters;
    filters << tr("Xfig") + " (*.fig)" << tr("Pdf") + " (*.pdf)" <<
               tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd->setFileMode(QFileDialog::AnyFile);

    if (fd->exec() == QDialog::Accepted) {
        QStringList fileNames = fd->selectedFiles();
        if(fileNames.size() == 0) {
            delete fd;
            return;
        }
        file_name = fileNames.at(0);
        if(fd->selectedNameFilter() == "Xfig (*.fig)") {
            if(file_name.section(QDir::toNativeSeparators("/"), -1).indexOf(".") == -1)
                file_name += ".fig";
        }
        else if(fd->selectedNameFilter() == "Pdf (*.pdf)") {
            if(file_name.section(QDir::toNativeSeparators("/"), -1).indexOf(".") == -1)
                file_name += ".pdf";
        }

        QString local(QLocale().bcp47Name());
        local = local.section('_', 0, 0 );
        QString file_base_name = QDir::toNativeSeparators(Settings::instance()->getAppDirectory() + "src/imagenes/base_net.") + local + ".fig";
        QFile file_base(file_base_name);
        if (file_base.open(QIODevice::ReadOnly)) {
            QTextStream is(&file_base);
            preambulo = is.readAll();
            file_base.close();
        }
        else {
//            QMessageBox::critical(nullptr, tr("Export fig"),
//                                  tr("No base information for exporting"), tr("&Cancel"));
            QMessageBox::critical(nullptr, tr("Export fig"),
                                  tr("No base information for exporting"), QMessageBox::Cancel);
            statusBar()->showMessage(tr("Aborted."), 2000);
            delete fd;
            return;
        }

        if(fd->selectedNameFilter() == "Pdf (*.pdf)")
            file_fig.setFileName("tmpfig.fig");
        else
            file_fig.setFileName(file_name);

        if(file_fig.open(QIODevice::WriteOnly)) {
            QTextStream ts(&file_fig);
            ts << preambulo;
            view->exportFig(ts);
            file_fig.close();

            QString filt = fd->selectedNameFilter();
            if(filt == "Pdf (*.pdf)") {
                QProcess* proc = new QProcess( this );
                connect(proc, SIGNAL(errorOccurred(QProcess::ProcessError)),
                                this, SLOT(slotProcessError(QProcess::ProcessError)));
                connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
                processLaunched = "fig2dev";
                QStringList arguments;
                arguments << "-L" << "pdf" << "tmpfig.fig" << file_name;
                proc->start(processLaunched, arguments);
            }
        }
    }
    delete fd;
    statusBar()->showMessage(tr("Ready."));

    return;    

}

void NeuralDisApp::slotPathExport()
{

    QString preambulo, file_fig_name, file_dat_name;
    QFile file_dat, file_fig;
    QFileDialog* fd;
    QTextStream ts_dat, ts_fig;
    QString baseName, ext;
    bool toFig = false;

    Exports2Fig* exports2fig = Exports2Fig::instance();

    statusBar()->showMessage(tr("Exporting path..."));

    fd = new QFileDialog(this);
    fd->setDirectory(prog_settings->getSimsDirectory());
    QStringList filters;
    filters << tr("Data") + " (*.dat)" << tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd->setFileMode(QFileDialog::AnyFile);

    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();        
        if(fileNames.size() == 0) {
            delete fd;
            return;
        }
        file_dat_name = fileNames.at(0);
        file_dat.setFileName(file_dat_name);
        if(file_dat.open(QIODevice::ReadOnly)) {
            ts_dat.setDevice(&file_dat);
        }
        else {
//            QMessageBox::critical(nullptr, tr("Export path"),
//                                  tr("Couldn't open data file for exporting"), tr("&Cancel"));
            QMessageBox::critical(nullptr, tr("Export path"),
                                  tr("Couldn't open data file for exporting"), QMessageBox::Cancel);
            statusBar()->showMessage(tr("Aborted."), 2000);
            return;
        }
    }
    else {
        delete fd;
        statusBar()->showMessage(tr("Aborted."), 2000);
        return;
    }

    fd->setDirectory(prog_settings->getSimsDirectory());
    filters.clear();
    filters << tr("Svg") + " (*.svg)" << tr("Xfig") + " (*.fig)" << tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd->setFileMode(QFileDialog::AnyFile);

    if(fd->exec() == QDialog::Accepted) {
        QStringList fileNames = fd->selectedFiles();
        toFig = fd->selectedNameFilter().contains("fig");
        delete fd;
        if(fileNames.size() == 0) {
            return;
        }
        file_fig_name = fileNames.at(0);
        int sep = file_fig_name.lastIndexOf(".");
        if(sep > -1) {
            baseName = file_fig_name.left(sep);
            ext = file_fig_name.right(file_fig_name.length() - sep -1);
        }
        else {
            baseName = file_fig_name;
//            ext ="fig";
            ext = toFig ? "fig" : "svg";
        }

        if(toFig) {
            QFile file_base(QDir::toNativeSeparators(Settings::instance()->getAppDirectory() +
                                                     "/src/imagenes/base_fig.fig"));
            if (file_base.open(QIODevice::ReadOnly)) {
                QTextStream is(&file_base);
                preambulo = is.readAll();
                file_base.close();
            }
            else {
//                QMessageBox::critical(nullptr, tr("Export path"),
//                                      tr("No base information for exporting"), tr("&Cancel"));
                QMessageBox::critical(nullptr, tr("Export path"),
                                      tr("No base information for exporting"), QMessageBox::Cancel);
                file_dat.close();
                statusBar()->showMessage(tr("Aborted."), 2000);
                return;
            }
        }

        int cont = 1;
        while(!ts_dat.atEnd()) {
            QString file_fig_name = baseName + "-" +
                            QString::number(cont).rightJustified(3, '0') + "." + ext;
            file_fig.setFileName(file_fig_name);
            if(file_fig.open(QIODevice::WriteOnly)) {
                ts_fig.setDevice(&file_fig);
                if(toFig) {
                    ts_fig << preambulo;
                    exports2fig->exportPath(ts_dat, ts_fig);
                }
                else
                    exports2fig->exportPath(ts_dat, ts_fig, Exports2Fig::ToSvg);
                file_fig.close();
            }
            else {                
//                QMessageBox::critical(nullptr, tr("Export path"),
//                                      tr("Couldn't open ") + ext + tr(" file for exporting"), tr("&Cancel"));
                QMessageBox::critical(nullptr, tr("Export path"),
                                      tr("Couldn't open ") + ext + tr(" file for exporting"), QMessageBox::Cancel);
                file_dat.close();
                statusBar()->showMessage(tr("Aborted."), 2000);
                return;
            }
            ++cont;
        }
        file_dat.close();
    }
    else
        delete fd;
    statusBar()->showMessage(tr("Ready."));

}

void NeuralDisApp::slotWorldExport()
{

    QString preambulo, file_fig_name;
    QFile file_fig;
    QFileDialog* fd;
    QTextStream ts_fig;

    if(!world) {
//        QMessageBox::information(nullptr, tr("World export"),
//                                 tr("No world loaded, nothing to export"), tr("&Ok"));
        QMessageBox::information(nullptr, tr("World export"),
                                 tr("No world loaded, nothing to export"), QMessageBox::Ok);
        statusBar()->showMessage(tr("Aborted."), 2000);
        return;
    }

    statusBar()->showMessage(tr("Exporting world..."));

//    int resp = QMessageBox::information(this, tr("Export to Fig"),
//                                        tr("Change coordinates?"),
//                                        tr("&No"), tr("&Yes"));
    bool resp = QMessageBox::information(this, tr("Export to Fig"),
                                tr("Change coordinates?"),
                                QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes;
    fd = new QFileDialog(this);
    fd->setDirectory(prog_settings->getWorldDirectory());
    QStringList filters;
    filters << tr("Xfig") + " (*.fig)" << tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd->setFileMode(QFileDialog::AnyFile);
    fd->setAcceptMode(QFileDialog::AcceptSave);
    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();
        delete fd;
        if(fileNames.size() == 0) {
            return;
        }
        file_fig_name = fileNames.at(0);
        if(file_fig_name.section(QDir::toNativeSeparators("/"), -1).indexOf(".") == -1)
            file_fig_name += ".fig";
        file_fig.setFileName(file_fig_name);

        QDir base_fig_dir("src/imagenes");
        QFile file_base(QDir::toNativeSeparators(Settings::instance()->getAppDirectory()) + QDir::separator() + base_fig_dir.path() + QDir::separator() + "base_fig.fig");
        if (file_base.open(QIODevice::ReadOnly)) {
            QTextStream is(&file_base);
            preambulo = is.readAll();
            file_base.close();
        }
        else {
//            QMessageBox::critical(nullptr, tr("World export"),
//                                  tr("No base information for exporting"), tr("&Cancel"));
            QMessageBox::critical(nullptr, tr("World export"),
                                  tr("No base information for exporting"), QMessageBox::Cancel);
            statusBar()->showMessage(tr("Aborted."), 2000);
            return;
        }
        if(file_fig.open(QIODevice::WriteOnly)) {
            ts_fig.setDevice(&file_fig);
            ts_fig << preambulo;
//            world->exportToFig(ts_fig, (bool) resp);
            world->exportToFig(ts_fig, resp);
            file_fig.close();
        }
        else {
//            QMessageBox::critical(nullptr, tr("Export path"),
//                                  tr("Couldn't open Xfig file for exporting"), tr("&Cancel"));
            QMessageBox::critical(nullptr, tr("Export path"),
                                  tr("Couldn't open Xfig file for exporting"), QMessageBox::Cancel);
            statusBar()->showMessage(tr("Aborted."), 2000);
            return;
        }
    }
    else
        delete fd;
    statusBar()->showMessage(tr("Ready."));

}

void NeuralDisApp::slotFixWorld() // para aumentar el grosor de las líneas y que se vean
{
    statusBar()->showMessage(tr("Loading world..."));

    QString worldName;
    QFileDialog* fd;
    int object;
    bool ok;
    QFileInfo fileInfo;
    QDir dir;

    fd = new QFileDialog(this);
    fd->setDirectory(prog_settings->getWorldDirectory());
    QStringList filters;
    filters << tr("Simulated worlds") + " (" + prog_settings->getWorldExtension() + ")"
            << tr("Any files") + " (*)";
    fd->setNameFilters(filters);

    fd->setFileMode(QFileDialog::ExistingFile);
    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();
        if(fileNames.size() == 0) {
            delete fd;
            return;
        }
        worldName = fileNames.at(0);
    }
    delete fd;

    QFile file( worldName );
    QString line;
    if ( !file.open( QIODevice::ReadOnly ) ) {
//        QMessageBox::critical(nullptr, Settings::tr("Load world"),
//                            Settings::tr("World file doesn't exist"), Settings::tr("&Cancel"));
        QMessageBox::critical(nullptr, Settings::tr("Load world"),
                              Settings::tr("World file doesn't exist"), QMessageBox::Cancel);
        return;
    }

    QString tmp_name = worldName + "-tmp";
    QFile file_tmp(tmp_name);
    if ( !file_tmp.open( QIODevice::WriteOnly)) {
//        QMessageBox::critical(nullptr, Settings::tr("Load world"),
//                              Settings::tr("Cannot create tmp file"), Settings::tr("&Cancel"));
        QMessageBox::critical(nullptr, Settings::tr("Load world"),
                              Settings::tr("Cannot create tmp file"), QMessageBox::Cancel);
        return;
    }

    fileInfo.setFile(file);
    dir.setPath(fileInfo.path());

    QTextStream ts(&file);
    QTextStream tmp(&file_tmp);
    line = ts.readLine();
    tmp << line << Qt::endl;
    if(!line.contains("#FIG 3.2")) {
        QMessageBox::critical(nullptr, tr("Load world"),
                              tr("Isn't a valid world file"),
                              QMessageBox::Cancel);
        file.close();
        file_tmp.close();
        dir.remove(tmp_name);
        return;
    }
    for(int i = 1;i < 8; i++) {
        line = ts.readLine();
        tmp << line << Qt::endl;
    }

    while(!ts.atEnd()) {
        line = ts.readLine();
        if(line.isEmpty()) continue;
        if(line.at(0).isSpace()) {
            tmp << line << Qt::endl;
            continue;
        }
        QStringList dat = line.split(" ");
        object = dat[0].toInt(&ok);
        if(!ok) {
//            QMessageBox::critical(nullptr, tr("Load world"),
//                                  tr("Isn't a valid world file"), tr("&Cancel"));
            QMessageBox::critical(nullptr, tr("Load world"),
                                  tr("Isn't a valid world file"), QMessageBox::Cancel);
            file.close();
            file_tmp.close();
            dir.remove(tmp_name);
            return;
        }
        if(object == POLYGON ) {
            uint subtype = dat[1].toUInt();
            if(subtype ==  LINE) {
                uint n_points = dat[15].toUInt();
                if(n_points == 2)
                    dat[3] = QString::number(45*3); // thickness, 45 NDAriaWorld::toFigUnits()
            }
            tmp << dat.join(" ") << Qt::endl;
        }
        else
            tmp << line << Qt::endl;
    }

    file.close();
    file_tmp.close();
    dir.remove(worldName);
    dir.rename(tmp_name, worldName);

    statusBar()->showMessage(tr("Ready."));

}

void NeuralDisApp::slotScilabExport()
{

    QString file_dat_name;
    QFile file_dat, file_new, tmp_file;
    QFileDialog* fd;
    QTextStream ts_dat, ts_new;
    QString baseName, ext, tmp_name;
    QFileInfo fileInfo;

    Exports2Fig* exports2fig = Exports2Fig::instance();

    statusBar()->showMessage(tr("Exporting Scilab..."));

    fd = new QFileDialog(this);
    fd->setDirectory(prog_settings->getSimsDirectory());
    QStringList filters;
    filters << tr("Data") + " (*.dat)" << tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd->setFileMode(QFileDialog::AnyFile);

    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();
        delete fd;
        if(fileNames.size() == 0) {
            return;
        }
        file_dat_name = fileNames.at(0);
        file_dat.setFileName(file_dat_name);
        if(file_dat.open(QIODevice::ReadOnly)) {
            ts_dat.setDevice(&file_dat);
        }
        else {
//            QMessageBox::critical(nullptr, tr("Export Scilab"),
//                                  tr("Couldn't open data file for exporting"), tr("&Cancel"));
            QMessageBox::critical(nullptr, tr("Export Scilab"),
                                  tr("Couldn't open data file for exporting"), QMessageBox::Cancel);
            statusBar()->showMessage(tr("Aborted."), 2000);
            return;
        }

    }
    else {
        delete fd;
        statusBar()->showMessage(tr("Aborted."), 2000);
        return;
    }

    fileInfo.setFile(file_dat);
    baseName = fileInfo.path() + QDir::toNativeSeparators("/") + fileInfo.baseName();
    ext = fileInfo.completeSuffix();

    int cont = 1;
    bool startNewFile = false;
    QString line;

    QMessageBox::StandardButton ret = QMessageBox::information(this, tr("Scilab Export"), tr("Change coordinates in source ?"),
                                       QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::No);

    if(ret == QMessageBox::Cancel) {
        file_dat.close();
        return;
    }

    tmp_name = baseName + "-copy" + "." + ext;
    tmp_file.setFileName(tmp_name);
    if(tmp_file.open(QIODevice::WriteOnly))
        ts_new.setDevice(&tmp_file);
    else {
//        QMessageBox::critical(nullptr, tr("Export Scilab"),
//                              tr("Couldn't open new file for exporting"), tr("&Cancel"));
        QMessageBox::critical(nullptr, tr("Export Scilab"),
                              tr("Couldn't open new file for exporting"), QMessageBox::Cancel);
        file_dat.close();
        statusBar()->showMessage(tr("Aborted."), 2000);
        return;
    }

    exports2fig->exportPath(ts_dat, ts_new, Exports2Fig::ToAscii);
    file_dat.close();
    tmp_file.flush();
    tmp_file.close();

    if(tmp_file.open(QIODevice::ReadOnly))
        ts_dat.setDevice(&tmp_file);
    else {
//        QMessageBox::critical(nullptr, tr("Export Scilab"),
//                              tr("Couldn't open new file for exporting"), tr("&Cancel"));
        QMessageBox::critical(nullptr, tr("Export Scilab"),
                              tr("Couldn't open new file for exporting"), QMessageBox::Cancel);
        file_dat.close();
        statusBar()->showMessage(tr("Aborted."), 2000);
        return;
    }

    while(!ts_dat.atEnd()) {
        QString file_new_name = baseName + "-" +  QString::number(cont).
                                    rightJustified(3, '0') + "." + ext;
        file_new.setFileName(file_new_name);
        if(file_new.open(QIODevice::WriteOnly)) {
            ts_new.setDevice(&file_new);
            startNewFile = true;
        }
        else {
//            QMessageBox::critical(nullptr, tr("Export Scilab"),
//                                  tr("Couldn't open new file for exporting"), tr("&Cancel"));
            QMessageBox::critical(nullptr, tr("Export Scilab"),
                                  tr("Couldn't open new file for exporting"), QMessageBox::Cancel);
            statusBar()->showMessage(tr("Aborted."), 2000);
            return;
        }

        while(!ts_dat.atEnd()) {
            line = ts_dat.readLine();
            if(line.trimmed().isEmpty()) continue;

            if(!line.trimmed().at(0).isDigit()) {
                if(startNewFile)
                    continue;
                else
                    break;
            }
            else {
                startNewFile = false;
                ts_new << line << Qt::endl;
            }
        }

        file_new.close();
        ++cont;

    }

    QDir dir(fileInfo.path());
    if(ret == QMessageBox::Yes) {
        dir.remove(file_dat_name);
        dir.rename(tmp_name, file_dat_name);
    }

    statusBar()->showMessage(tr("Ready."));

}

void NeuralDisApp::slotTestXML( void )
{
    doc->testXML();
}

void NeuralDisApp::slotFileQuit()
{

    if(queryExit()) {
        statusBar()->showMessage(tr("Exiting application..."));
        if(queryClose("File quit")) {
            if(help)
                delete help;
            if(sim)
                delete sim;
            if(browser_prog) {
                browser_prog->terminate();
                QTimer::singleShot( 5000, browser_prog, SLOT( kill() ) );
            }
            if(browser_api) {
                browser_api->terminate();
                QTimer::singleShot( 5000, browser_api, SLOT( kill() ) );
            }
            prog_settings->deleteNet();
            prog_settings->deleteNetView();
            if(doc)
                delete doc;
            qApp->quit();
        }
    }
    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::closeEvent( QCloseEvent* e)
{

    if(sim) {
        if(sim->isRunning()) {
            e->ignore();
            return;
        }
    }

    e->ignore();
    slotFileQuit();
}

void NeuralDisApp::slotEditCut()
{
    statusBar()->showMessage(tr("Cutting selection..."));
    if(net)
        net->cut();
    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::slotEditCopy()
{
    statusBar()->showMessage(tr("Copying selection to clipboard..."));
    if(net)
        net->copy();
    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::slotEditPaste()
{
    statusBar()->showMessage(tr("Inserting clipboard contents..."));

    if(view)
        view->paste();

    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::slotEditSettings()
{
    statusBar()->showMessage(tr("Editing configuration file..."));
    prog_settings->editSettings();
    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::slotSaveSettings()
{
    statusBar()->showMessage(tr("Saving configuration file..."));
    prog_settings->saveSettings();
    statusBar()->showMessage(tr("Ready."));
}


/* funciones nuevas implementadas para undo y redo */


void NeuralDisApp::slotEditUndo()
{
    statusBar()->showMessage(tr("Undo last action"));
    if(net) {
        net->undo();
        view->update();
    }
    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::slotEditRedo()
{
    statusBar()->showMessage(tr("Redo last action..."));
    if(net) {
        net->redo();
        view->update();
    }
    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::slotViewToolBar(bool toggle)
{
    statusBar()->showMessage(tr("Toggle toolbar..."));
    if (toggle== false) {
        fileToolbar->hide();
    }
    else {
        fileToolbar->show();
    }

    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::slotViewDesignerBar(bool toggle)
{
    statusBar()->showMessage(tr("Toggle designerbar..."));
    if (toggle== false) {
        designerBar->hide();
    }
    else {
        designerBar->show();
    }
    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::slotViewStatusBar(bool toggle)
{
    statusBar()->showMessage(tr("Toggle statusbar..."));
    if (toggle == false) {
        statusBar()->hide();
    }
    else {
        statusBar()->show();
    }
    statusBar()->showMessage(tr("Ready."));
}


void NeuralDisApp::windowMenuAboutToShow()
{
    QList<QMdiSubWindow *> windows = pWorkspace->subWindowList(QMdiArea::CreationOrder);
    int w_c = int(windows.count());
    pWindowMenu->clear();
    pWindowMenu->addAction(windowCascade);
    pWindowMenu->addAction(windowTile);

    if ( w_c == 0 ) {
        windowAction->setEnabled(false);
    }
    else {
        windowAction->setEnabled(true);
    }

    pWindowMenu->addSeparator();
    for ( int i = 0; i < w_c; ++i ) {
        QAction* action = pWindowMenu->addAction(QString("&%1 ").arg(i+1)+
                                            windows.at(i)->windowTitle());
        action->setData(QVariant(i));
        connect(pWindowMenu, SIGNAL(triggered(QAction*)),this, SLOT(windowMenuActivated(QAction*)));
        action->setChecked(pWorkspace->activeSubWindow() == windows.at(i));
    }
}

void NeuralDisApp::windowMenuActivated(QAction* action)
{
    QWidget* w = pWorkspace->subWindowList(QMdiArea::CreationOrder).at(action->data().toInt());
    if ( w )
        w->setFocus();
}

void NeuralDisApp::slotHelpAbout()
{
    QMessageBox::about(this,tr("About..."),
                       tr("CyBerSim") + "\n" + tr("Version") + " 1.0.0" + "\n" + "(c) 2003 by Jose M. Cuadra Troncoso");
}

void NeuralDisApp::slotHelpManual()
{
    statusBar()->showMessage(tr("User guide..."));
//    if(prog_settings->getHelpCommand() == "CyBeRSim") {
//        help->setWindowTitle(tr("CyBerSim user guide"));
//        help->setActiveWindow();
//        help->raise();
//        help->show();
//    }
//    else {
        if(!browser_prog)
            browser_prog = new QProcess(this);
//        if(!browser_prog->state() != QProcess::Running) {
        if(browser_prog->state() != QProcess::Running) {
            QStringList arguments;
            arguments << manual_dir + "index.html";
            connect(browser_prog, SIGNAL(errorOccurred(QProcess::ProcessError)), this,
                    SLOT(slotLaunchHelp()));
            browser_prog->start(prog_settings->getHelpCommand(), arguments);
        }
//    }
    statusBar()->showMessage(tr("Ready."));
}

void NeuralDisApp::slotLaunchHelp(void)
{

//    QMessageBox::warning(this,tr("Help system"),
//                         tr("No program available,") + "\n" + tr("using built-in browser"),tr("&Ok"));
    QMessageBox::warning(this,tr("Help system"),
                         tr("No program available,") + "\n" + tr("using built-in browser"), QMessageBox::Ok);
    prog_settings->setHelpCommand("CyBeRSim");
    prog_settings->setHelpBrowser(0);
    if(!help)
        initHelp();
    help->setWindowTitle(tr("CyBeRSim user guide"));
    help->activateWindow();
    help->raise();
    help->show();

}

void NeuralDisApp::slotLaunchApi()
{
//    QMessageBox::warning(this,tr("Help system"),
//                         tr("No program available,") + "\n" + tr("using built-in browser"),tr("&Ok"));
    QMessageBox::warning(this,tr("Help system"),
                         tr("No program available,") + "\n" + tr("using built-in browser"), QMessageBox::Ok);
    prog_settings->setHelpCommand("CyBeRSim");
    prog_settings->setHelpBrowser(0);
    if(!helpAPI)
        initApi();
    helpAPI->setWindowTitle(tr("CyBeRSim API guide"));
    helpAPI->activateWindow();
    helpAPI->raise();
    helpAPI->show();
}

void NeuralDisApp::slotProcessError(QProcess::ProcessError error)
{
    if(error == QProcess::FailedToStart)
        QMessageBox::critical(nullptr, processLaunched, processLaunched + tr(" can't start"),
                              QMessageBox::Cancel);
}

void NeuralDisApp::showPointCoordinates(QMouseEvent *event)
{
    statusBar()->showMessage(tr("Cursor position (") + QString::number(event->pos().x()) +
                             ", " + QString::number(event->pos().y()) + ")");
}

void NeuralDisApp::slotApiManual()
{
    statusBar()->showMessage(tr("User guide..."));

    if(!browser_api)
        browser_api = new QProcess(this);
//    if(!browser_api->state() != QProcess::Running) {
    if(browser_api->state() != QProcess::Running) {
        QStringList api_file;
        api_file << prog_settings->getAppDirectory() + "/docs/api/doxygen/html/index.html";

        connect(browser_api, SIGNAL(errorOccurred(QProcess::ProcessError)), this,
                SLOT(slotLaunchApi(void)));
        browser_api->start(prog_settings->getHelpCommand(), api_file);
    }

    statusBar()->showMessage(tr("Ready."));
}

//* Designer implememtation */

void NeuralDisApp::slotNewNet()
{
    if(net) {
        if(!net->notEmpty())
            slotFileNew();
//        else if(QMessageBox::warning(nullptr,tr("New net"),
//                                     tr("This operation will clear the net, continue ?"),
//                                     tr("&Ok"), tr("&Cancel")) == 0) {
        else if(QMessageBox::warning(nullptr,tr("New net"),
                                  tr("This operation will clear the net, continue ?"),
                                  QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {
            prog_settings->deleteNet();
            NetBuilder* netBuilder = new NetBuilder();
            net = netBuilder->newNet();
            if(!net) {
                statusBar()->showMessage(tr("Aborted."));
                return;
            }
        }
    }
    else
        slotFileNew();

    toolPointer->setChecked(true);
    if(view)
        view->setTool(0);
    statusBar()->showMessage(tr("Ready."), 2000);

}

void NeuralDisApp::slotClearNet()
{
    if(net) {
        if(net->notEmpty()) {
//            if(QMessageBox::warning(nullptr,tr("Clear net"),
//                                    (tr("Are you sure about clear the net ?")),
//                                    tr("&Ok"), tr("&Cancel")) == 0) {
            if(QMessageBox::warning(nullptr,tr("Clear net"),
                                 (tr("Are you sure about clear the net ?")),
                                 QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {
                net->clearNet(true);
                doc->setModified(false);
            }
        }
    }
    toolPointer->setChecked(true);
    view->setTool(0);
}

void NeuralDisApp::slotAddNeuron()
{
    if(net) {
        if(!net->notEmpty())
            slotFileNew();
        else
            net->addNeuron();
    }
    else
        slotFileNew();
    toolPointer->setChecked(true);
    view->setTool(0);

}

void NeuralDisApp::slotDelNeuron()
{
    toolPointer->setChecked(true);
    view->setTool(0);
    net->delNeuron();
}

void NeuralDisApp::slotEditNeuron(void)
{
    toolPointer->setChecked(true);
    view->setTool(0);
    net->editNeuron();
}

void NeuralDisApp::slotAddSynapse()
{
    toolPointer->setChecked(true);
    view->setTool(0);
    net->addSynapse();
}

void NeuralDisApp::slotDelSynapse()
{
    toolPointer->setChecked(true);
    view->setTool(0);
    net->delSynapse();
}

void NeuralDisApp::slotEditSynapse(void)
{
    toolPointer->setChecked(true);
    view->setTool(0);
    net->editSynapse();
}

void NeuralDisApp::setTool(QAction *tool)
{

    if(tool == toolPointer) {
        statusBar()->showMessage(tr("Deselects tools"));
        if(view)
            view->setTool(0);
    }
    else if(tool == toolAddNeuron)
        slotToolAddNeuron();
    else if(tool == toolDelNeuron)
        slotToolDelNeuron();
    else if(tool == toolEditNeuron)
        slotToolEditNeuron();
    else if(tool == toolAddSynapse)
        slotToolAddSynapse();
    else if(tool == toolDelSynapse)
        slotToolDelSynapse();
    else if(tool == toolEditSynapse)
        slotToolEditSynapse();
    else if(tool == toolAddNetwork)
        slotToolAddNetwork();
    else if(tool == toolDelNetwork)
        slotToolDelNetwork();
    else if(tool == toolEditNetwork)
        slotToolEditNetwork();
    else if(tool == toolNetInfo)
        slotToolNetInfo();
}

void NeuralDisApp::slotToolAddNeuron(void)
{

    statusBar()->showMessage(tr("Add neuron"));
    if(net) {
        if(!net->notEmpty()) {
            statusBar()->showMessage(tr("Creating new file..."));
            slotFileNew();
        }
        else
            statusBar()->showMessage(tr("Adding neuron"));
    }
    else {
        statusBar()->showMessage(tr("Creating new file..."));
        slotFileNew();
    }
    if(view)
        view->setTool(ADDNEURON);

}

void NeuralDisApp::slotToolDelNeuron(void)
{
    if(net) {
        if(net->notEmpty()) {
            statusBar()->showMessage(tr("Deleting neuron"));
            view->setTool(DELNEURON);
        }
    }
}

void NeuralDisApp::slotToolEditNeuron(void)
{

    if(net) {
        if(net->notEmpty()) {
            statusBar()->showMessage(tr("Editing neuron"));
            view->setTool(EDITNEURON);
        }
    }
}

void NeuralDisApp::slotToolAddSynapse(void)
{

    if(net) {
        if(net->notEmpty()) {
            statusBar()->showMessage(tr("Adding synapse"));
            view->setTool(ADDSYNAPSE);
        }
    }
}

void NeuralDisApp::slotToolDelSynapse(void)
{

    if(net) {
        if(net->notEmpty()) {
            statusBar()->showMessage(tr("Deleting synapse"));
            view->setTool(DELSYNAPSE);
        }
    }
}

void NeuralDisApp::slotToolEditSynapse(void)
{

    if(net->notEmpty()) {
        statusBar()->showMessage(tr("Editing synapse"));
        view->setTool(EDITSYNAPSE);
    }
}

void NeuralDisApp::slotToolAddNetwork(void)
{

    if(net) {
        if(net->notEmpty()) {
            statusBar()->showMessage(tr("Adding subnet"));
            view->setTool(ADDNETWORK);
        }
    }
}

void NeuralDisApp::slotToolDelNetwork(void)
{

    if(net) {
        if(net->notEmpty()) {
            statusBar()->showMessage(tr("Deleting subnet"));
            view->setTool(DELNETWORK);
        }
    }
}

void NeuralDisApp::slotToolEditNetwork(void)
{

    if(net->notEmpty()) {
        statusBar()->showMessage(tr("Editing subnet"));
        view->setTool(EDITNETWORK);
    }
}


void NeuralDisApp::slotViewLeyend(void)
{
    statusBar()->showMessage(tr("View Legend"));
    if(view->isViewLeyend()) {
        view->viewLeyend(false);
        viewLeyend->setText(tr("Show weights legend"));
    }
    else {
        view->viewLeyend(true);
        viewLeyend->setText(tr("Hide weights legend"));
    }
}

void NeuralDisApp::slotShowAllSensors(void)
{
    for(int i = 0; i < sensorsActionsList.size(); i++)
        sensorsActionsList[i]->setChecked(true);
}

void NeuralDisApp::slotHideAllSensors(void)
{
    for(int i = 0; i < sensorsActionsList.size(); i++)
        sensorsActionsList[i]->setChecked(false);
}

void NeuralDisApp::slotViewSensors(bool checked, QVariant data)
{
    statusBar()->showMessage(tr("View sensors"));
    QString s_data = data.toString();
    sim->showSensors(checked, s_data);
}

void NeuralDisApp::slotViewRawText(void)
{
    statusBar()->showMessage(tr("View raw text"));
    if(view->viewText()) {
        view->viewRawText(false);
        viewViewRawText->setText(tr("Show raw text"));
    }
    else {
        view->viewRawText(true);
        viewViewRawText->setText(tr("Hide raw text"));
    }
}

void NeuralDisApp::slotCreateWorld(void)
{
    QProcess* proc = new QProcess( this );
    QStringList arguments;
    QDir dir_imag = QDir("src/world/imagenes/walls.fig");
    arguments << Settings::instance()->getAppDirectory() + QDir::separator() + dir_imag.path();
    connect(proc, SIGNAL(errorOccurred(QProcess::ProcessError)),
                    this, SLOT(slotProcessError(QProcess::ProcessError)));
    connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
    processLaunched = "xfig";
    proc->start(processLaunched, arguments);
}

void NeuralDisApp::slotEditWorld(void)
{

    if(world) {
        QProcess* proc = new QProcess( this );
//        if(world->metaObject()->className() == "World") {
        if(strcmp(world->metaObject()->className(), "World") == 0) {
            QStringList arguments;
            arguments << world->getFileName();
            connect(proc, SIGNAL(errorOccurred(QProcess::ProcessError)),
                            this, SLOT(slotProcessError(QProcess::ProcessError)));
            connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
            processLaunched = "xfig";
            proc->start(processLaunched, arguments);
        }
//        else if(world->metaObject()->className() == "NDAriaWorld") {
        else if(strcmp(world->metaObject()->className(), "NDAriaWorld") ==0) {
            QStringList arguments;
            arguments << world->getFileName();
            connect(proc, SIGNAL(errorOccurred(QProcess::ProcessError)),
                            this, SLOT(slotProcessError(QProcess::ProcessError)));
            connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
            processLaunched = "Mapper3";
            proc->start(processLaunched, arguments);
        }
        else
//            QMessageBox::critical(nullptr, tr("Edit world"),
//                                  tr("Unknown type of world"), tr("&Cancel"));
            QMessageBox::critical(nullptr, tr("Edit world"),
                                  tr("Unknown type of world"), QMessageBox::Cancel);
    }
    else
//        QMessageBox::critical(nullptr, tr("Edit world"),
//                              tr("No world loaded"), tr("&Cancel"));
        QMessageBox::critical(nullptr, tr("Edit world"),
                              tr("No world loaded"), QMessageBox::Cancel);
}

void NeuralDisApp::slotLoadWorld(void)
{

    statusBar()->showMessage(tr("Loading world..."));

    QString worldName;
    QFileDialog* fd;
    fd = new QFileDialog(this);
    fd->setWindowTitle(tr("World load"));
    fd->setDirectory(prog_settings->getWorldDirectory());
    QStringList filters;
    filters << tr("CyberSim worlds") + " (" + prog_settings->getWorldExtension() + ")" <<
               tr("Aria worlds") + " (*.map)" <<
               tr("Xfig worlds") + " (*.fig)" <<
               tr("Any files") + " (*)";
    fd->setNameFilters(filters);

    fd->setFileMode(QFileDialog::ExistingFile);
    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();
        if(fileNames.size() != 0)
            worldName = fileNames.at(0);
    }
    delete fd;
    if (!worldName.isEmpty()) {
        WorldFactory* worldFactory = new WorldFactory();
        AbstractWorld* new_world = worldFactory->createWorld(worldName);
        if(new_world) {
            slotCloseWorld();
            world = new_world;
            prog_settings->setWorld(world);
            //      pWindowList->append(world->view());
            QString message=tr("Loaded world:")+ " " + worldName;
            statusBar()->showMessage(message, 2000);

            connect(actionShow_quadtree, SIGNAL(toggled(bool)), world, SLOT(showQuadtree(bool)));
            connect(world->view(), SIGNAL(closeWorld(void)),
                    this, SLOT(slotCloseWorld(void)));
            world->view()->show();
            world->view()->getMdiSubWindow()->showNormal();
            slotEnableSimMenu();
        }
        else
            statusBar()->showMessage(tr("Loading aborted"), 2000);
        delete worldFactory;
    }
    else {
        statusBar()->showMessage(tr("Opening aborted"), 2000);
    }
}

void NeuralDisApp::slotCloseWorld(void)
{

    statusBar()->showMessage(tr("Closing world..."), 2000);

    if(world) {
        slotSimClose();
        worldCenterInRobotAction->setChecked(false);
        delete world;
        world = 0;

        prog_settings->setWorld(world);
    }

}

void NeuralDisApp::slotZoomOut(void)
{
    world->zoomOut();
}

void NeuralDisApp::slotZoomIn(void)
{
    world->zoomIn();
}

void NeuralDisApp::slotSimEdit(void)
{

    statusBar()->showMessage(tr("Editing simulation"));
    if(!sim) {
        QMessageBox::information(this, tr("Sim edit"),
                                 tr("No simulation loaded"), QMessageBox::Ok);
        return;
    }
    if(sim->isRecording())
        sim->edit(2);
    else
        sim->edit(1);

    statusBar()->showMessage(tr("Ready"));

}

bool NeuralDisApp::slotSimLoad2(void)
{
    return slotSimLoad(false);
}

bool NeuralDisApp::slotSimLoad(bool nuevo)
{

    statusBar()->showMessage(tr("Loading simulation"));

    QString file_sim_name;
    QFileDialog* fd;
    if(!nuevo) {
        fd = new QFileDialog(this);
        fd->setWindowTitle(tr("Sim load"));
        fd->setDirectory(prog_settings->getSimsDirectory());
        QStringList filters;
        filters << tr("Simulations") + " (*.sim)" << tr("Any files") + " (*)";
        fd->setNameFilters(filters);
        fd->setFileMode(QFileDialog::ExistingFile);

        if ( fd->exec() == QDialog::Accepted ) {
            QStringList fileNames = fd->selectedFiles();
            delete fd;
            if(fileNames.size() != 0)
                file_sim_name = fileNames.at(0);
            if (!file_sim_name.isEmpty()) {
                if(!doc->loadSim(file_sim_name)) {
//                    QMessageBox::critical(nullptr, tr("Sim load"),
//                                          tr("Couldn't open data file"), tr("&Cancel"));
                    QMessageBox::critical(nullptr, tr("Sim load"),
                                          tr("Couldn't open data file"), QMessageBox::Cancel);
                    statusBar()->showMessage(tr("Aborted."), 2000);
                    return false;
                }
            }
        }
        else {
            delete fd;
            statusBar()->showMessage(tr("Aborted."), 2000);
            return false;
        }
    }
    else {
        if(sim)
            file_sim_name = sim->getFileName();
        doc->loadSim(file_sim_name);
    }

    if(sim)
        delete sim;
    else {
        prog_settings->deleteNet();   // estan en el control
        prog_settings->deleteNetView();
    }
    sim = 0;

    SimulationBuilder* simBuilder = new SimulationBuilder();
    sim = simBuilder->construct(doc->getXMLSim(), file_sim_name);
    if(simBuilder->needWorld()) {
//        QMessageBox::information(this, tr("Sim load"),
//                                 tr("Need load a world"), tr("&Ok"));
        QMessageBox::information(this, tr("Sim load"),
                                 tr("Need load a world"), QMessageBox::Ok);
        slotLoadWorld();
        if(world)
            sim = simBuilder->construct(doc->getXMLSim(), file_sim_name);
        else {
            delete simBuilder;
            slotSimClose();
            return false;
        }
    }
    delete simBuilder;

    view = prog_settings->getNetView();
    if(view)
        connectNetViewElements();
    net = prog_settings->getNet();

    if(sim) {
        if(!nuevo)
            sim->init();
        else
            slotSimEdit();
        if(world) {
            world->view()->setFocus();
            connect(world->view(), SIGNAL(mouseEventToRobot(QMouseEvent*)), this, SLOT(showPointCoordinates(QMouseEvent*)));
        }

        menuShow_sensors->setEnabled(true);
        for(int i = 0; i < sensorsActionsList.size(); i++) {
            menuShow_sensors->removeAction(sensorsActionsList[i]);
            delete sensorsActionsList[i];
        }
        sensorsActionsList.clear();


        QList<QString> sensorGroupsNames = sim->getSensorGroupsNames();
        for(int i = 0; i < sensorGroupsNames.size(); i++) {
            MyAction* action = new MyAction(this);
            QString actionName = sensorGroupsNames[i].left(sensorGroupsNames[i].lastIndexOf('_'));
            action->setObjectName(actionName + "Action");
            action->setCheckable(true);
            action->setText(actionName);
            action->setData(sensorGroupsNames[i]);
            sensorsActionsList.append(action);
            menuShow_sensors->addAction(action);
            connect(action, SIGNAL(toggled(bool, QVariant)), this, SLOT(slotViewSensors(bool, QVariant)));
        }

        fileNew->setEnabled(false);
        disCreateNetwork->setEnabled(false);
        disClearNetwork->setEnabled(false);

        slotEnableSimMenu();
        statusBar()->showMessage(tr("Ready"));
    }
    else
        statusBar()->showMessage(tr("Aborted"));

    return true;

}

void NeuralDisApp::slotSimNew(void)
{

    statusBar()->showMessage(tr("New simulation"));

    SimulationBuilder* simBuilder = new SimulationBuilder();
    QDomDocument doc;
    sim = simBuilder->construct(doc);

    if(simBuilder->needWorld()) {
//        QMessageBox::information(this, tr("Sim load"),
//                                 tr("Need load a world"), tr("&Ok"));
        QMessageBox::information(this, tr("Sim load"),
                                 tr("Need load a world"), QMessageBox::Ok);
        slotLoadWorld();
        if(world)
            sim = simBuilder->construct(doc);
    }

    delete simBuilder;
    if(sim) {
        if(slotSimSave())
            slotSimLoad(true);
        else
            slotSimClose();
    }

    statusBar()->showMessage(tr("Ready"));

}

bool NeuralDisApp::slotSimSave(void)
{

    bool ret = false;
    statusBar()->showMessage(tr("Saving simulation"));
    if(!sim)
    {
//        QMessageBox::information(this, tr("Sim save"),
//                                 tr("No simulation loaded"), tr("&Ok"));
        QMessageBox::information(this, tr("Sim save"),
                                 tr("No simulation loaded"), QMessageBox::Ok);
        return ret;
    }
    QString full_name = prog_settings->fillPath(sim->getFileName(),
                                                prog_settings->getSimsDirectory());
    QString extens = prog_settings->getSimsExtension();
    extens = extens.contains("*.") ? extens.right(extens.length() - 2) : extens;
    QString ret_name = doc->saveDocument(full_name, sim->save(), tr("Simulations"),
                                         extens, tr("Saving simulation"), true);
    if(!ret_name.isEmpty()) {
        sim->setFileName(ret_name);
        ret = true;
    }

    statusBar()->showMessage(tr("Ready"));
    return ret;

}

bool NeuralDisApp::slotSimClose(void)
{

    bool ret = false;
    if(sim)
    {
        delete sim;
        sim = 0;

        world->simClosed();
        prog_settings->deleteNet();
        prog_settings->deleteNetView();
        view = prog_settings->getNetView();
        net = prog_settings->getNet();

        menuShow_sensors->setEnabled(false);
        fileNew->setEnabled(true);
        disCreateNetwork->setEnabled(true);
        disClearNetwork->setEnabled(true);

        slotEnableSimMenu();

        ret = true;
    }
    return ret;
}

void NeuralDisApp::slotSimGo(void)
{

    if(!sim) {
//        QMessageBox::information(this, tr("Sim go"),
//                                 tr("No simulation loaded"), tr("&Ok"));
        QMessageBox::information(this, tr("Sim go"),
                                 tr("No simulation loaded"), QMessageBox::Ok);
        return;
    }

    enableRunningSim(false);
    sim->go();

}

void NeuralDisApp::slotSimStep(void)
{

    if(!sim) {
//        QMessageBox::information(this, tr("Sim step"),
//                                 tr("No simulation loaded"), tr("&Ok"));
        QMessageBox::information(this, tr("Sim step"),
                                 tr("No simulation loaded"), QMessageBox::Ok);
        return;
    }
    sim->step();

}

void NeuralDisApp::slotSimPause(void)
{

    if(!sim) {
//        QMessageBox::information(this, tr("Sim pause"),
//                                 tr("No simulation loaded"), tr("&Ok"));
        QMessageBox::information(this, tr("Sim pause"),
                                 tr("No simulation loaded"), QMessageBox::Ok);
        return;
    }

    sim->pause();
    simStep->setEnabled(true);

}

void NeuralDisApp::slotSimStop(void)
{

    if(!sim) {
//        QMessageBox::information(this, tr("Sim stop"),
//                                 tr("No simulation loaded"), tr("&Ok"));
        QMessageBox::information(this, tr("Sim stop"),
                                 tr("No simulation loaded"), QMessageBox::Ok);
        return;
    }

//    if(sim->isRecording())
//        simRecord->setEnabled(true);
//    else
//        simRecord->setEnabled(true);
    simStep->setEnabled(true);
    sim->stop();

    enableRunningSim(true);

}

void NeuralDisApp::slotSimRecord(bool on)
{
    if(!sim) {
        QMessageBox::information(this, tr("Sim record"),
                                 tr("No simulation loaded"), QMessageBox::Ok);
        return;
    }
    sim->setRecording(on);

}

void NeuralDisApp::slotDriveRobot(bool on)
{
    if(!sim) {
//        QMessageBox::information(this, tr("Drive robot"),
//                                 tr("No simulation loaded"), tr("&Ok"));
        QMessageBox::information(this, tr("Drive robot"),
                                 tr("No simulation loaded"), QMessageBox::Ok);
        return;
    }
    sim->setDriving(on);
}

void NeuralDisApp::setToolSim(QAction *simtool)
{

    if(simtool == simGo) {
        slotSimGo();
    }
    else if(simtool == simStop) {
        slotSimStop();
    }
    else if(simtool == simPause) {
        slotSimPause();
    }

}

void NeuralDisApp::slotSimFilm(bool on)
{
    if (!sim) {
//        QMessageBox::information(this, tr("Film simulation"),
//                                 tr("No simulation loaded"), tr("&Ok"));
        QMessageBox::information(this, tr("Film simulation"),
                                 tr("No simulation loaded"), QMessageBox::Ok);
        return;
    }
    if (world) {
        statusBar()->showMessage(tr("Film simulation"));
        world->setFilmed(on);
    }
}

void NeuralDisApp::slotRecordSensors( bool on )
{
    if (!sim) {
//        QMessageBox::information(this, tr("Film simulation"),
//                                 tr("No simulation loaded"), tr("&Ok"));
        QMessageBox::information(this, tr("Film simulation"),
                                 tr("No simulation loaded"), QMessageBox::Ok);
        return;
    }
    else {
        statusBar()->showMessage(tr("Film simulation"));
        sim->recordSensorsDialog(on);
    }
}

void NeuralDisApp::enableRunningSim(bool on)
{
    fileMenu->setEnabled(on);
    editMenu->setEnabled(on);
    disMenu->setEnabled(on);
    simMenu->setEnabled(on);
    // new 06/04/2021
    if(view) {
        viewLeyend->setEnabled(on);
        viewViewRawText->setEnabled(on);
        viewShowNetView->setEnabled(on);
    }
    else {
        viewLeyend->setEnabled(false);
        viewViewRawText->setEnabled(false);
        viewShowNetView->setEnabled(false);
    }
    createWorld->setEnabled(on);
    editWorld->setEnabled(on);
    loadWorld->setEnabled(on);
    closeWorld->setEnabled(on);
    windowCascade->setEnabled(on);
    windowTile->setEnabled(on);
    fileToolbar->setEnabled(on);
    designerBar->setEnabled(on);
    simStep->setEnabled(false);
    simPause->setEnabled(!on);

}

void NeuralDisApp::slotDelNetwork()
{
    /* copiado de borrar neurona */
    toolPointer->setChecked(true);
    view->setTool(0);
    net->delNetwork();
}

void NeuralDisApp::slotAddNetwork()
{
    /* esto lo tengo que modificar creando los cuadros de diálogo nuevos para añadir vista de red, ahora esta copiado de addneuron */
    QString filename;

    if(net) {
        if(!net->notEmpty())
            slotFileNew();
        else
            /* añadir una nueva red, debe poder permitirnos abrir el fichero de red correspondiente y luego a�adir su icono */
            filename=slotNetworkOpen();
        net->addNetwork(filename);
    }
    else
        toolPointer->setChecked(true);
    view->setTool(0);

}


void NeuralDisApp::slotEditNetwork()
{
    /* copiado de editar neurona, debe poder abrirse una ventana nueva que nos permita editar la red */
    QString filename="";
    toolPointer->setChecked(true);
    view->setTool(0);
    filename=net->editNetwork();

    if (filename!=""){
        slotSubNetOpen(filename);}

}

QString NeuralDisApp::slotNetworkOpen()
{
    statusBar()->showMessage(tr("Opening file..."));

    QFileDialog* fd;
    QString fileName;
    fd = new QFileDialog(this);
    fd->setWindowTitle(tr("Subnet load"));
    fd->setDirectory(prog_settings->getNetDirectory());
    QStringList filters;
    filters << tr("Nets") + " (" + prog_settings->getNetExtension() + ")" <<
               tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd->setFileMode(QFileDialog::ExistingFile);

    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();
        if(fileNames.size() != 0)
            fileName = fileNames.at(0);
    }

    delete fd;
    return fileName;
}

void NeuralDisApp::slotToolNetView(void)
{

    if(net->notEmpty()) {
        statusBar()->showMessage(tr("net View"));
        view->setTool(NETVIEW);
    }
}

void NeuralDisApp::slotViewShowNetView()
{
    // para generar una nueva vista de la red actual
    view->setTool(NETVIEW);
}

void NeuralDisApp::slotToolNetInfo( void )
{
    if(net->notEmpty()) {
        statusBar()->showMessage(tr("Net information"));
        view->setTool(NETINFO);
    }
}


void NeuralDisApp::slotWorldRestoreZoom(void)
{
    if(world) {
        statusBar()->showMessage(tr("Restore world zoom"));
        world->restoreInitialZoom();
    }
}


void NeuralDisApp::slotCenterInRobot( bool on )
{
    if(sim) {
        statusBar()->showMessage(tr("Center view in robot"));
        sim->centerInRobot(on);
    }
}

void NeuralDisApp::slotWorldScale(void)
{
    if(world) {
        statusBar()->showMessage(tr("Scaling world"));
        world->scaleByUser();
    }
}

void NeuralDisApp::slotWorldRotate(void)
{
    if(world) {
        statusBar()->showMessage(tr("Rotating world"));
        world->rotateView();
    }
}

void NeuralDisApp::slotSaveWorld(void)
{
    if(world) {
        statusBar()->showMessage(tr("Saving XML world"));
        world->writeXML();
    }
}

void NeuralDisApp::slotDefaultMaterials(void)
{
    if(world) {
        statusBar()->showMessage(tr("Creating default materials"));
        world->getDefaultMaterials();
        world->changeMaterials();
    }
}
