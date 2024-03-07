#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QMap>

namespace Ui {
class HelpWindow;
}

class HelpWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HelpWindow(const QString& home_, QWidget *parent = 0);
    ~HelpWindow();

public slots:
    void destroy(void);

protected:
    void changeEvent(QEvent *e);

private slots:
    void setBackwardAvailable(bool available);
    void setForwardAvailable(bool available);

    void about(void);

    void textChanged(void);
    void openFile(void);
    void newWindow(void);
    void print(void);

    void histChosen(QAction* action);
    void bookmChosen(QAction* action);
    void addBookmark(void);

private:
    void readHistory();
    void readBookmarks();
    void setSource(const QString& source);

private:
    int backwardId, forwardId;
    QString selectedURL;
    QStringList searchPath;
    QStringList history, bookmarks;
    QMap<QAction*, QString> mHistory, mBookmarks;
    // TODO relate file names with doc titles in history and bookmarks
    QMap<QString, QString> docTitlesHistory, docTitlesBookmarks;

private:
    Ui::HelpWindow *ui;
};

#endif // HELPWINDOW_H
