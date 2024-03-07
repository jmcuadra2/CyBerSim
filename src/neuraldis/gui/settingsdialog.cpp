#include "settingsdialog.h"

#include <QVariant>
#include <QMessageBox>
#include <QFileDialog>

/*
 *  Constructs a settingsDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */

settingsDialog::settingsDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    QObject::connect(LineEditExtension, SIGNAL(textChanged(QString)),
                     this, SLOT(setNetExtension(QString)));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(buttonDir, SIGNAL(clicked()), this, SLOT(chooseDirectory()));
    QObject::connect(buttonApp, SIGNAL(clicked()), this, SLOT(chooseDirApp()));
    QObject::connect(buttonWorkingDir, SIGNAL(clicked()), this, SLOT(chooseWorkingDir()));
    QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(LineEditDirectory, SIGNAL(textChanged(QString)),
                     this, SLOT(setNetDirectory(QString)));
    QObject::connect(LineEditDirApp, SIGNAL(textChanged(QString)),
                     this, SLOT(setDirApp(QString)));
    QObject::connect(lineEditWorkingDir, SIGNAL(textChanged(QString)),
                     this, SLOT(setWorkingDir(QString)));
    QObject::connect(LineEditBrowser, SIGNAL(textChanged(QString)),
                     this, SLOT(setCommand(QString)));
    QObject::connect(buttonFileBrowser, SIGNAL(clicked()), this, SLOT(chooseBrowser()));
    QObject::connect(LineEditWorldExtension, SIGNAL(textChanged(QString)),
                     this, SLOT(setWorldExtension(QString)));
    QObject::connect(LineEditWorldDirectory, SIGNAL(textChanged(QString)),
                     this, SLOT(setWorldDirectory(QString)));
    QObject::connect(buttonWorld, SIGNAL(clicked()), this, SLOT(chooseWorldDirectory()));
    QObject::connect(ListBoxBrowser, SIGNAL(currentRowChanged(int)),
                     this, SLOT(setBrowser(int)));
    QObject::connect(LineEditSimsDirectory, SIGNAL(textChanged(QString)),
                     this, SLOT(setSimsDirectory(QString)));
    QObject::connect(LineEditSimsExtension, SIGNAL(textChanged(QString)),
                     this, SLOT(setSimsExtension(QString)));
    QObject::connect(buttonSims, SIGNAL(clicked()), this, SLOT(chooseSimsDirectory()));
    QObject::connect(LineEditRobotsDirectory, SIGNAL(textChanged(QString)),
                     this, SLOT(setRobotDirectory(QString)));
    QObject::connect(LineEditRobotsExtension, SIGNAL(textChanged(QString)),
                     this, SLOT(setRobotExtension(QString)));
    QObject::connect(buttonRobots, SIGNAL(clicked()), this, SLOT(chooseRobotDirectory()));
    QObject::connect(LineEditCtrlsDirectory, SIGNAL(textChanged(QString)),
                     this, SLOT(setControlDirectory(QString)));
    QObject::connect(LineEditCtrlsExtension, SIGNAL(textChanged(QString)),
                    this, SLOT(setControlExtension(QString)));
    QObject::connect(buttonCtrls, SIGNAL(clicked()), this, SLOT(chooseControlDirectory()));

}

/*
 *  Destroys the object and frees any allocated resources
 */
settingsDialog::~settingsDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void settingsDialog::languageChange()
{
    retranslateUi(this);
}


/*! \class settingsDialog
 *   \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */

void settingsDialog::setNetExtension(const QString& ext)
{
    net_extension = ext;
}

void settingsDialog::setNetDirectory(const QString& dir)
{
    net_directory = dir;

}

void settingsDialog::setWorldExtension(const QString& ext)
{
    world_extension = ext;
}

void settingsDialog::setWorldDirectory(const QString& dir)
{
    world_directory = dir;

}

void settingsDialog::setDirApp(const QString& dir)
{
    dirApp = dir;

}

void settingsDialog::setWorkingDir(const QString& dir)
{
    workingDir = dir;

}

void settingsDialog::setCommand(const QString& c)
{
    if (c.isEmpty())  {
        if (browser == commands.count() - 1) {   // Other browser
            browser = 0;
            ListBoxBrowser->setCurrentItem(0);
        }
        setBrowser(browser);
    }
    else {
        command = c;
    }
}

void settingsDialog::setBrowser(int b)
{
    browser = uint(b);
    command = commands[browser];
    LineEditBrowser->setText(command);
    if (browser)  {
        LineEditBrowser->setEnabled(true);
        buttonFileBrowser->setEnabled(true);
    }
    else {
        LineEditBrowser->setEnabled(false);
        buttonFileBrowser->setEnabled(false);
    }
}


void settingsDialog::setSimsExtension(const QString& ext)
{
    sims_extension = ext;
}

void settingsDialog::setSimsDirectory(const QString& dir)
{
    sims_directory = dir;

}


QString settingsDialog::getNetExtension(void)
{
    return net_extension;
}

QString settingsDialog::getNetDirectory(void)
{
    return net_directory;
}


QString settingsDialog::getWorldExtension(void)
{
    return world_extension;
}

QString settingsDialog::getWorldDirectory(void)
{
    return world_directory;
}

QString settingsDialog::getDirApp(void)
{
    return dirApp;
}

QString settingsDialog::getWorkingDir(void)
{
    return workingDir;
}

QString settingsDialog::getCommand(void)
{
    return command;
}

uint settingsDialog::getBrowser(void)
{
    return browser;
}

QString settingsDialog::getSimsExtension(void)
{
    return sims_extension;
}

QString settingsDialog::getSimsDirectory(void)
{
    return sims_directory;
}

void settingsDialog::setValues(QString dirA, QString dirWork, QString dir, QString ext, QString w_dir, QString w_ext, uint brow, QString com, QString s_dir, QString s_ext, QString r_dir, QString r_ext, QString c_dir, QString c_ext)
{
    net_extension = ext;
    LineEditExtension->setText(ext);
    net_directory = dir;
    LineEditDirectory->setText(dir);
    world_extension = w_ext;
    LineEditWorldExtension->setText(w_ext);
    world_directory = w_dir;
    LineEditWorldDirectory->setText(w_dir);
    dirApp = dirA;
    LineEditDirApp->setText(dirA);
    workingDir = dirWork;
    lineEditWorkingDir->setText(dirWork);
    browser = brow;
    command = com;
    sims_directory = s_dir;
    LineEditSimsDirectory->setText(s_dir);
    sims_extension = s_ext;
    LineEditSimsExtension->setText(s_ext);
    robot_directory = r_dir;
    LineEditRobotsDirectory->setText(r_dir);
    robot_extension = r_ext;
    LineEditRobotsExtension->setText(r_ext);
    control_directory = c_dir;
    LineEditCtrlsDirectory->setText(c_dir);
    control_extension = c_ext;
    LineEditCtrlsExtension->setText(c_ext);

    commands.append("CyBeRSim");
    commands.append("netscape");
    commands.append("mozilla");
    commands.append("konqueror");
    commands.append("kdehelp");
    commands.append("nautilus");
    commands.append("galeon");
    commands.append("google-chrome");
    commands.append("?");
    commands[browser] = command;

    for(int i = 0; i < commands.size(); i++) {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(commands.at(i));
        ListBoxBrowser->insertItem(i, newItem);
    }

    ListBoxBrowser->setCurrentRow(browser);
    LineEditBrowser->setText(commands[browser]);

    if (!browser) {
        LineEditBrowser->setEnabled(false);
        buttonFileBrowser->setEnabled(false);
    }
    else {
        LineEditBrowser->setEnabled(true);
        buttonFileBrowser->setEnabled(true);
    }
}

void settingsDialog::chooseDirectory(void)
{
    QString dir;
    dir = QFileDialog::getExistingDirectory(this, tr("Choose a directory for net files"),
                                            fillPath(net_directory));
    if (!dir.isEmpty()) {
        net_directory = dir;
        LineEditDirectory->setText(dir);
    }
    
}

void settingsDialog::chooseWorldDirectory(void)
{
    QString dir;
    dir = QFileDialog::getExistingDirectory(this, tr("Choose a directory for world files"),
                                            fillPath(world_directory));
    if (!dir.isEmpty()) {
        world_directory = dir;
        LineEditWorldDirectory->setText(dir);
    }
}

void settingsDialog::chooseDirApp(void)
{
    QString dir;
    dir = QFileDialog::getExistingDirectory(this, tr("Look for Cybersim directory"),
                                            dirApp);
    if (!dir.isEmpty()) {
        dirApp = dir;
        LineEditDirApp->setText(dir);
    }
}

void settingsDialog::chooseWorkingDir(void)
{
    QString dir;
    dir = QFileDialog::getExistingDirectory(this, tr("Look for working directory"),
                                            workingDir);
    if (!dir.isEmpty()) {
        workingDir = dir;
        lineEditWorkingDir->setText(dir);
    }
}

void settingsDialog::chooseBrowser(void)
{
    QString file;
    file = QFileDialog::getOpenFileName(this, tr("Look for help browser"), QDir::homePath());
    if (!file.isEmpty()) {
        command = file;
        //commands[browser] = command;
        LineEditBrowser->setText(command);
    }
}

void settingsDialog::chooseSimsDirectory(void)
{
    QString dir;
    dir = QFileDialog::getExistingDirectory(this, tr("Choose a directory for simulations files"),
                                            fillPath(sims_directory));
    if (!dir.isEmpty()) {
        sims_directory = dir;
        LineEditSimsDirectory->setText(dir);
    }
}

void settingsDialog::accept(void)
{
    if (command == "?" && command.trimmed().isEmpty()) {
        browser = 0;
        command = commands[0];
    }
    QDialog::accept();
}

void settingsDialog::setRobotExtension(const QString& ext)
{
    robot_extension = ext;
}

void settingsDialog::setRobotDirectory(const QString& dir)
{
    robot_directory = dir;

}

QString settingsDialog::getRobotExtension(void)
{
    return robot_extension;
}

QString settingsDialog::getRobotDirectory(void)
{
    return robot_directory;
}


void settingsDialog::chooseRobotDirectory(void)
{
    QString dir;
    dir = QFileDialog::getExistingDirectory(this, tr("Choose a directory for robot files"),
                                            fillPath(robot_directory));
    if (!dir.isEmpty()) {
        robot_directory = dir;
        LineEditRobotsDirectory->setText(dir);
    }
    else {
        robot_directory = workingDir;
        LineEditRobotsDirectory->setText(dir);
    }
}

void settingsDialog::setControlExtension(const QString& ext)
{
    control_extension = ext;
}

void settingsDialog::setControlDirectory(const QString& dir)
{
    control_directory = dir;

}

QString settingsDialog::getControlExtension(void)
{
    return control_extension;
}

QString settingsDialog::getControlDirectory(void)
{
    return control_directory;
}


void settingsDialog::chooseControlDirectory(void)
{
    QString dir;
    dir = QFileDialog::getExistingDirectory(this, tr("Choose a directory for control files"),
                                            fillPath(control_directory));
    if (!dir.isEmpty()) {
        control_directory = dir;
        LineEditCtrlsDirectory->setText(dir);
    }
}

QString settingsDialog::fillPath(const QString& path)
{
    QString fullpath = path;
    QString separator;
    bool is_relative = false;
#ifdef Q_OS_UNIX
    separator = "/";
    is_relative = (path.left(1) != separator);
#endif
#ifdef Q_OS_WIN
    separator = "\\";
    is_relative = !(path.contains(":\\") || path.contains(":/"));
#endif
    if(is_relative) {
        fullpath = QDir::cleanPath(workingDir + QDir::separator() + fullpath);
//        fullpath = workingDir + separator + fullpath;
    }
    return fullpath;
}
