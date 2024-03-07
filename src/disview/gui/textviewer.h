#ifndef TEXTVIEWER_H
#define TEXTVIEWER_H

#include "ui_textviewer.h"

class TextViewer : public QDialog, public Ui::TextViewer
{
    Q_OBJECT

public:
    TextViewer(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~TextViewer();

    virtual void initTextViewer( bool read_only );

public slots:
    virtual void slotTexto( QString texto_ );

signals:
    void closeViewer(void);

protected slots:
    virtual void languageChange();

    virtual void closeEvent( QCloseEvent * ce );
    virtual void destroy( void );


};

#endif // TEXTVIEWER_H
