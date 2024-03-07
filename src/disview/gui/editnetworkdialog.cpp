#include "editnetworkdialog.h"

#include <QVariant>
#include <QMessageBox>

/*
 *  Constructs a editNetworkDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
editNetworkDialog::editNetworkDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    QObject::connect(buttonAddNetworkCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(SpinBox1, SIGNAL(valueChanged(QString)), this, SLOT(getText(QString)));
    QObject::connect(buttonAddNetworkOK, SIGNAL(clicked()), this, SLOT(accept()));

}

/*
 *  Destroys the object and frees any allocated resources
 */
editNetworkDialog::~editNetworkDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void editNetworkDialog::languageChange()
{
    retranslateUi(this);
}

int editNetworkDialog::getNum_IdNetwork(void)
{
    return num_IdNetwork = SpinBox1->cleanText().toInt();
}

bool editNetworkDialog::setNum_IdNetwork( int n_i )
{
    bool ret = false;

  if(n_i > 0) {
         num_IdNetwork  = n_i;
         ret = true;
     }  
 
     return ret;
}


void editNetworkDialog::getText( const QString& texto )
{  
    if(!setNum_IdNetwork(texto.toInt()))
//          QMessageBox::information(nullptr,tr("editSubNet"), tr("Network out of range"),tr("&Ok"));
              QMessageBox::information(nullptr,tr("editSubNet"), tr("Network out of range"), QMessageBox::Ok);
}


void editNetworkDialog::accept( void )
{
    QString texto = SpinBox1->text();
    if(setNum_IdNetwork(texto.toInt()))
        QDialog::accept();
    else
//         QMessageBox::information(nullptr,tr("editSubNet"), tr("Network out of range"),tr("&Ok"));
             QMessageBox::information(nullptr,tr("editSubNet"), tr("Network out of range"), QMessageBox::Ok);
}
