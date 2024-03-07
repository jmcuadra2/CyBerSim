#include "dictclasschooser.h"

#include <QVariant>
#include <QListWidgetItem>
/*
 *  Constructs a dictClassChooser as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
dictClassChooser::dictClassChooser(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    QObject::connect(classListBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                     this, SLOT(itemChoosed(QListWidgetItem*)));
    QObject::connect(pushButton1, SIGNAL(clicked()), this, SLOT(reject()));

}

/*
 *  Destroys the object and frees any allocated resources
 */
dictClassChooser::~dictClassChooser()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dictClassChooser::languageChange()
{
    retranslateUi(this);
}

void dictClassChooser::itemChoosed(QListWidgetItem *item )
{
    class_id = -1;
    QString s_class_id  = item->text();
    class_id  = names_map[s_class_id];
    accept();
}

void dictClassChooser::setClassMap( NameClass_Map namesmap )
{
    names_map = namesmap;
    class_id = -1;
    NameClass_Map::Iterator it;
    int i = 0;
    for (it = names_map.begin(); it != names_map.end(); ++it) {
        QListWidgetItem* item = new QListWidgetItem(it.key());
        classListBox->insertItem(i, item);
        ++i;
    }
    
}


int dictClassChooser::getClassId( void )
{
    return class_id;
}


void dictClassChooser::setComment( const QString & comment )
{
    commentLextLabel->setText(comment);
}
