#ifndef DICTCLASSCHOOSER_H
#define DICTCLASSCHOOSER_H

#include "ui_dictclasschooser.h"
#include "../dictionarymanager.h"

class QListWidgetItem;

class dictClassChooser : public QDialog, public Ui::dictClassChooser
{
    Q_OBJECT

public:
    dictClassChooser(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~dictClassChooser();

    virtual void setClassMap( NameClass_Map namesmap );
    virtual int getClassId( void );

public slots:
    virtual void setComment( const QString & comment );

protected:
    NameClass_Map names_map;
    int class_id;

protected slots:
    virtual void languageChange();

    virtual void itemChoosed(QListWidgetItem * item);


};

#endif // DICTCLASSCHOOSER_H
