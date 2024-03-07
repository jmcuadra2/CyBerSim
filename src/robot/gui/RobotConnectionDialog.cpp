#include "RobotConnectionDialog.h"

#include <QVariant>
#include <QFileDialog>
#include <QDir>
#include <QButtonGroup>

#include "../../neuraldis/settings.h"

/*
 *  Constructs a RobotConnectionDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
RobotConnectionDialog::RobotConnectionDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    server_file = QDir::toNativeSeparators("/usr/local/Arnl/examples/ndguiServer");
    dirRobot = "192.168.1.11";
    QString remotePort = "8101";
    QString host = "localhost";
    modeButtonGroup = new QButtonGroup(modeGroupBox);
    modeButtonGroup->addButton(simulatorRadioButton, 0);
    modeButtonGroup->addButton(offBoardRadioButton, 1);
    modeButtonGroup->addButton(onBoardRadioButton, 2);

    init();

    connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(onBoardRadioButton, SIGNAL(toggled(bool)), this, SLOT(activarServidorPC(bool)));
    connect(userLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setUser(QString)));
    connect(hostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setHost(QString)));
    connect(passwordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setPassword(QString)));
    connect(terminalLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setTerminal(QString)));
    connect(programServerLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setServidor(QString)));
    connect(mapaLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setMapa(QString)));
    connect(userRobotLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setUserRobot(QString)));
    connect(dirRobotLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setDirRobot(QString)));
    connect(nombreServidorToolButton, SIGNAL(clicked()), this, SLOT(setFileServidor()));
    connect(mapaToolButton, SIGNAL(clicked()), this, SLOT(setFileMapa()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
RobotConnectionDialog::~RobotConnectionDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void RobotConnectionDialog::languageChange()
{
    retranslateUi(this);
}

void RobotConnectionDialog::setUser(const QString & user)
{
    if (user != userLineEdit->text())
        userLineEdit->setText(user);
}


void RobotConnectionDialog::setHost(const QString & host)
{
    if (host != hostLineEdit->text())
        hostLineEdit->setText(host);
}


void RobotConnectionDialog::setPassword(const QString & password)
{
    if (password != passwordLineEdit->text())
        passwordLineEdit->setText(password);
}


void RobotConnectionDialog::setTerminal(const QString & terminal)
{
    if (terminal != terminalLineEdit->text())
        terminalLineEdit->setText(terminal);
}


void RobotConnectionDialog::setServidor(const QString & programServer)
{
    if (programServer != programServerLineEdit->text())
        programServerLineEdit->setText(programServer);
}


void RobotConnectionDialog::setMapa(const QString & mapa)
{
    if (mapa != mapaLineEdit->text())
        mapaLineEdit->setText(mapa);
}


void RobotConnectionDialog::setRemotePort(const QString & remotePort)
{
    if (remotePort != remotePortLineEdit->text())
        remotePortLineEdit->setText(remotePort);
}


void RobotConnectionDialog::setUserRobot(const QString & userRobot)
{
    if (userRobot != userRobotLineEdit->text())
        userRobotLineEdit->setText(userRobot);
}

void RobotConnectionDialog::setDirRobot(const QString & dirRobot)
{
    if (dirRobot != dirRobotLineEdit->text())
        dirRobotLineEdit->setText(dirRobot);
}


QString RobotConnectionDialog::getUser()
{
    return userLineEdit->text();
}


QString RobotConnectionDialog::getHost()
{
    return hostLineEdit->text();
}

void RobotConnectionDialog::activarServidorPC(bool on)
{
    //     userRobotLineEdit->setDisabled(on);
    //   dirRobotLineEdit->setDisabled(on);
    if(on)
        setHost("192.168.1.3");
    else
        setHost("localhost");
}

QString RobotConnectionDialog::getPassword()
{
    return passwordLineEdit->text();
}


QString RobotConnectionDialog::getTerminal()
{
    return terminalLineEdit ->text();
}

QString RobotConnectionDialog::getServidor()
{
    return programServerLineEdit ->text();
}

QString RobotConnectionDialog::getMapa()
{
    return mapaLineEdit ->text();
}

QString RobotConnectionDialog::getRemotePort()
{
    return remotePortLineEdit ->text();
}

QString RobotConnectionDialog::getUserRobot()
{
    return userRobotLineEdit ->text();
}

QString RobotConnectionDialog::getDirRobot()
{
    return dirRobotLineEdit ->text();
}

void RobotConnectionDialog::init()
{
    //   simulatorRadioButton->toggle();
    simulatorRadioButton->setChecked(true);
}

bool RobotConnectionDialog::isRealRobot() // Ñapa por que el comando de microcontrolador
{                                         // VEL2 de Aria gira bien en MobileSim
    if (modeButtonGroup->checkedId() > 0)   // y mal con los robots reales
        return true;
    else
        return false;
}

void RobotConnectionDialog::setFileServidor()
{
    QString program;
    program = QFileDialog::getOpenFileName(this, tr("Choose a file for server"),
                                           "/usr/local/Arnl/examples/");
    if (!(program == "")) {
        server_file = program;
        programServerLineEdit->setText(program);
    }
}

void RobotConnectionDialog::setFileMapa()
{
    QString mapa;
    Settings *progSettings = Settings::instance();
    mapa = QFileDialog::getOpenFileName(this, progSettings->getWorldDirectory(),
                                        tr("Choose a file for map"), tr("Aria map") + "(*.map)");
    if (!(mapa == "")) {
        map_file = mapa;
        mapaLineEdit->setText(mapa);
    }

}
