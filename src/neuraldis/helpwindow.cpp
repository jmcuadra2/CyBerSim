#include "helpwindow.h"
#include "ui_helpwindow.h"

#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>

HelpWindow::HelpWindow(const QString &home_, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::HelpWindow), selectedURL()
{
    ui->setupUi(this);

    ui->statusHelp->setFixedHeight(ui->statusHelp->sizeHint().height());
    readHistory();
    readBookmarks();

    ui->browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    connect(ui->browser, SIGNAL(textChanged()), this, SLOT(textChanged()));
    connect(ui->browser, SIGNAL(backwardAvailable(bool)),
                        this, SLOT(setBackwardAvailable(bool)));
    connect(ui->browser, SIGNAL(forwardAvailable(bool)),
                        this, SLOT(setForwardAvailable(bool)));

    setSource(home_);

    connect(ui->browser, SIGNAL(highlighted(const QString&)),
                    ui->statusHelp, SLOT(showMessage(const QString&)));

    connect(ui->actionNew_window, SIGNAL(triggered(bool)), this, SLOT(newWindow()));
    connect(ui->actionOpen_file, SIGNAL(triggered(bool)), this, SLOT(openFile()));
    connect(ui->actionPrint, SIGNAL(triggered(bool)), this, SLOT(print()));
    connect(ui->actionClose, SIGNAL(triggered(bool)), this, SLOT(close()));

    ui->actionBackwards->setEnabled(false);
    ui->actionForwards->setEnabled(false);
    connect(ui->actionBackwards, SIGNAL(triggered(bool)), ui->browser, SLOT(backward()));
    connect(ui->actionForwards, SIGNAL(triggered(bool)), ui->browser, SLOT(forward()));
    connect(ui->actionHome, SIGNAL(triggered(bool)), ui->browser, SLOT(home()));
    connect(ui->menuHistory, SIGNAL( triggered(QAction*)),
             this, SLOT(histChosen(QAction*)));

    QListIterator<QString> ith(history);
    while (ith.hasNext()) {
        QString hist = ith.next();
        QAction* action = ui->menuHistory->addAction(hist);
        mHistory[action] = hist;
    }

    connect(ui->actionAdd_bookmark, SIGNAL(triggered(bool)), this, SLOT(addBookmark()));
    QListIterator<QString> itb(bookmarks);
    while (itb.hasNext()) {
        QString book = itb.next();
        QAction* action = ui->menuBookmarks->addAction(book);
        mBookmarks[action] = book;
    }
    connect(ui->menuBookmarks, SIGNAL( triggered(QAction*)),
             this, SLOT(bookmChosen(QAction*)));

    connect(ui->actionAbout, SIGNAL( triggered(bool)), this, SLOT(about()));

    ui->statusHelp->showMessage(tr("HandBook ready."));
    ui->browser->setFocus();
    resize(940, 720);
}

HelpWindow::~HelpWindow()
{
    delete ui;
    destroy();
}

void HelpWindow::destroy()
{
    history.clear();
    QMap<QAction*, QString>::Iterator it = mHistory.begin();
    for ( ; it != mHistory.end(); ++it)
        history.append( *it );

    QFile f(QDir::currentPath() + QDir::toNativeSeparators("/.history"));
    f.open(QIODevice::WriteOnly);
    QDataStream s(&f);
    s << history;
    f.close();

    bookmarks.clear();
    QMap<QAction*, QString>::Iterator it2 = mBookmarks.begin();
    for ( ; it2 != mBookmarks.end(); ++it2)
        bookmarks.append(*it2);

    QFile f2(QDir::currentPath() + QDir::toNativeSeparators("/.bookmarks"));
    f2.open(QIODevice::WriteOnly);
    QDataStream s2(&f2);
    s2 << bookmarks;
    f2.close();
}

void HelpWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void HelpWindow::setBackwardAvailable(bool available)
{
    ui->actionBackwards->setEnabled(available);
}

void HelpWindow::setForwardAvailable(bool available)
{
    ui->actionForwards->setEnabled(available);
}

void HelpWindow::about()
{
    QMessageBox::about( this, tr("Neuraldis HelpViewer"),
             tr("Adapted from an example from Qt's documentation, simple HTML and TXT help viewer."));
}

void HelpWindow::textChanged()
{
    if (ui->browser->documentTitle().isNull())
        setWindowTitle(ui->browser->source().toString());
    else
        setWindowTitle(ui->browser->documentTitle()) ;

    selectedURL = ui->browser->source().toString();

    if (!selectedURL.isEmpty()) {
        bool exists = false;
        if(mHistory.key(selectedURL)) {
            exists = true;
        }

        if(!exists) {
            QAction* action = ui->menuHistory->addAction(selectedURL);
            mHistory[action] = selectedURL;
        }

        selectedURL = QString();
    }
}

void HelpWindow::openFile()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::homePath(),
                                               "HTML *.html\n TXT *.txt");
    setSource(fn);
}

void HelpWindow::newWindow()
{
    (new HelpWindow(ui->browser->source().toString()))->show();
}

void HelpWindow::print()
{
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize(QPageSize::A4));
    QFileInfo fileInfo(QDir::homePath(), ui->browser->documentTitle() + ".pdf");
    QString filename = fileInfo.absoluteFilePath();
    printer.setOutputFileName(filename);

    QPrintDialog printDialog(&printer);
    if (printDialog.exec() == QDialog::Accepted) {
        ui->browser->print(&printer);
    }
}

void HelpWindow::histChosen(QAction* action)
{
    if(mHistory.contains(action))
        setSource(mHistory[action]);
}

void HelpWindow::bookmChosen(QAction *action)
{
    if(mBookmarks.contains(action))
        setSource(mBookmarks[action]);
}

void HelpWindow::addBookmark()
{
    QString book = ui->browser->source().toString();
    QString title = ui->browser->documentTitle();
    if(!mBookmarks.key(book)) {
        QAction* action = ui->menuBookmarks->addAction(book);
        mBookmarks[action] = book;
    }
}

void HelpWindow::readHistory()
{
    if ( QFile::exists( QDir::currentPath() +
                        QDir::toNativeSeparators("/.history" ))) {
        QFile f(QDir::currentPath() + QDir::toNativeSeparators("/.history"));
        f.open(QIODevice::ReadOnly);
        QDataStream s(&f);
        s >> history;
        f.close();
        while (history.count() > 20)
            history.removeAt(0);
    }
}

void HelpWindow::readBookmarks()
{
    if ( QFile::exists(QDir::currentPath() +
                       QDir::toNativeSeparators("/.bookmarks" ))) {
        QFile f(QDir::currentPath() + QDir::toNativeSeparators("/.bookmarks"));
        f.open(QIODevice::ReadOnly);
        QDataStream s(&f);
        s >> bookmarks;
        f.close();
        while (bookmarks.count() > 20)
            bookmarks.removeAt(0);
    }
}

void HelpWindow::setSource(const QString &source)
{
    if (!source.isEmpty()) {
        QString fileName = source.section('#', 0, 0);
        QFileInfo fileInfo(fileName);
        ui->browser->setSource(QUrl(source));
        if(fileInfo.exists() && !searchPath.contains(fileInfo.absolutePath())) {
            searchPath << fileInfo.absolutePath();
            ui->browser->setSearchPaths(searchPath);
        }
    }
}
