#include "buildnetdialog.h"

#include <QVariant>
#include <QMessageBox>

/*
 *  Constructs a buildNetDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
buildNetDialog::buildNetDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    QObject::connect(SpinBoxInput, SIGNAL(valueChanged(int)), this, SLOT(setNumInput(int)));
    QObject::connect(SpinBoxHidden, SIGNAL(valueChanged(int)), this, SLOT(setNumHidden(int)));
    QObject::connect(SpinBoxOutput, SIGNAL(valueChanged(int)), this, SLOT(setNumOutput(int)));
    QObject::connect(buttonBuildCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(buttonBuildOk, SIGNAL(clicked()), this, SLOT(accept()));

}

/*
 *  Destroys the object and frees any allocated resources
 */
buildNetDialog::~buildNetDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void buildNetDialog::languageChange()
{
    retranslateUi(this);
}

int buildNetDialog::getNumInput(void)
{
    return numInput = SpinBoxInput->cleanText().toInt();
}

void buildNetDialog::setNumInput( int n )
{
    numInput = n;
    SpinBoxInput->setValue(n);
}

int buildNetDialog::getNumHidden( void )
{
    return numHidden = SpinBoxHidden->cleanText().toInt();
}

int buildNetDialog::getNumOutput( void )
{
    return numOutput = SpinBoxOutput->cleanText().toInt();
}

void buildNetDialog::setNumOutput( int n )
{
    numOutput = n;
    SpinBoxOutput->setValue(n);
}

void buildNetDialog::setNumHidden( int n )
{
   numHidden = n;
   SpinBoxHidden->setValue(n);
}



void buildNetDialog::accept()
{
    if(numInput+numHidden+numOutput > 0) // quitar = es prueba
        QDialog::accept();
    else
//        QMessageBox::information(nullptr,tr("buildNet"), tr("Empyt net"),tr("&Ok"));
    QMessageBox::information(nullptr,tr("buildNet"), tr("Empyt net"), QMessageBox::Ok);

}


