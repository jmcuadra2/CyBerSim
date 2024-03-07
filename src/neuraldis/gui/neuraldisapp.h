#ifndef NEURALDISAPP_H
#define NEURALDISAPP_H

#include "ui_neuraldisapp.h"
#include "../../disview/neuraldisview.h"
#include "../neuraldisdoc.h"
#include "../../net/neuralnetwork.h"
#include "../dictionarymanager.h"
#include "../helpwindow.h"
#include "../settings.h"

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QProcess>

class AbstractWorld;
class BaseSimulation;
class MyAction;

class NeuralDisApp : public QMainWindow, public Ui::NeuralDisApp 
{
    Q_OBJECT

public:
    NeuralDisApp(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::Window );
    ~NeuralDisApp();

    void init( Settings * prog_set );

signals:
    void isInitView(void);

protected slots:
    void languageChange();

private:
    void destroy();
    void initMenuBar();
    void initToolBar();
    void initDesignerBar();
    void initSimulatorBar();
    void initStatusBar();
    void initHelp();
    void initApi();
    void initDoc();
    void initView();
    bool queryExit( void );
    bool queryClose( const QString & func );    
    void connectNetViewElements();

private slots:
    void slotEnableDisMenu( bool enabl );
    void slotEnableSimMenu( void );
    void slotEnableSimCommonMenu( bool on );
    void slotFileNew();
    void slotFileOpen();
    void slotSubNetOpen( QString file );
    bool slotFileSave();
    bool slotFileSaveAs();
    void slotFileClose();
    void slotFilePrint();
    void slotNetExport();
    void slotPathExport();
    void slotWorldExport();
    void slotScilabExport();
    void slotFixWorld();
    void slotTestXML( void );
    void slotFileQuit();
    
    void closeEvent( QCloseEvent * e );
    
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotEditSettings();
    void slotSaveSettings();
    void slotEditUndo();
    void slotEditRedo();
    
    void slotViewToolBar( bool toggle );
    void slotViewDesignerBar( bool toggle );
    void slotViewStatusBar( bool toggle );
    
    void windowMenuAboutToShow();
    void windowMenuActivated(QAction *action );
    void slotHelpAbout();
    void slotHelpManual();
    void slotApiManual();
    
    void slotNewNet();
    void slotClearNet();
    void slotAddNeuron();
    void slotDelNeuron();
    void slotEditNeuron( void );
    void slotAddSynapse();
    void slotDelSynapse();
    void slotEditSynapse( void );
    void setTool( QAction * tool );
    void slotToolAddNeuron( void );
    void slotToolDelNeuron( void );
    void slotToolEditNeuron( void );
    void slotToolAddSynapse( void );
    void slotToolDelSynapse( void );
    void slotToolEditSynapse( void );
    void slotToolAddNetwork( void );
    void slotToolDelNetwork( void );
    void slotToolEditNetwork( void );
    
    void slotViewLeyend( void );

    void slotShowAllSensors(void);
    void slotHideAllSensors(void);
    void slotViewSensors(bool checked, QVariant data);
    void slotViewRawText( void );
    
    void slotCreateWorld( void );
    void slotEditWorld( void );
    void slotLoadWorld( void );
    void slotCloseWorld( void );
    void slotZoomOut( void );
    void slotZoomIn( void );
    void slotWorldRestoreZoom( void );
    void slotCenterInRobot( bool on );
    void slotWorldScale( void );
    void slotWorldRotate( void );
    void slotDefaultMaterials( void );

    void slotSaveWorld( void );
    
    void slotSimEdit( void );
    bool slotSimLoad2( void );
    bool slotSimLoad( bool nuevo );
    void slotSimNew( void );
    bool slotSimSave( void );
    bool slotSimClose( void );
    void slotSimGo( void );
    void slotSimStep( void );
    void slotSimPause( void );
    void slotSimStop( void );
    void slotSimRecord( bool on );
    void slotDriveRobot(bool on);
    void setToolSim( QAction * simtool );
    void enableRunningSim( bool on );
    void slotSimFilm( bool on );
    void slotRecordSensors( bool on );
    
    void slotDelNetwork();
    void slotAddNetwork();
    void slotEditNetwork();
    QString slotNetworkOpen();
    void slotToolNetView( void );
    void slotViewShowNetView();
    void slotToolNetInfo( void );
    
    void slotLaunchHelp(void);
    void slotLaunchApi(void);

    void slotProcessError(QProcess::ProcessError error);

    void showPointCoordinates(QMouseEvent* event);

private:
    QProcess* browser_api;
    QProcess* browser_prog;
    QList<QWidget *> *pWindowList;
    NeuralDisView* view;
    AbstractWorld* world;
    NeuralDisDoc* doc;
    NeuralNetwork* net;
    Settings* prog_settings;
    QPointer<HelpWindow> help;
    QPointer<HelpWindow> helpAPI;
    QString manual_dir;
    QString api_dir;

    QMdiArea *pWorkspace;
    QGridLayout *view_back;
    BaseSimulation* sim;
    QList<MyAction* > sensorsActionsList;
    QString processLaunched;
};

class MyAction : public QAction
{
    Q_OBJECT
public:
    MyAction(QObject * parent) : QAction(parent)
    {
        connect(this, SIGNAL(toggled(bool )), this, SLOT(getToggled(bool)));
    }
    
public slots:
    void getToggled(bool checked) { emit toggled(checked, data()) ; }
    
signals:
    void toggled(bool /*checked*/, QVariant /*data*/);
};

#endif // NEURALDISAPP_H
