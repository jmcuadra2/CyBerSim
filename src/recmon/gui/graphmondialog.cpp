#include "graphmondialog.h"
#include "../graphicmonitor.h"

#include <QVariant>
#include <QMessageBox>  

/*
 *  Constructs a graphmonDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
graphmonDialog::graphmonDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    QObject::connect(OkButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(timeupdSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTimeUpd(int)));
    QObject::connect(tLabelsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setTLabels(int)));
    QObject::connect(maxFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMax(double)));
    QObject::connect(minFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMin(double)));
    QObject::connect(vLabelsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setVLabels(int)));
    QObject::connect(captionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setTitulo(QString)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
graphmonDialog::~graphmonDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void graphmonDialog::languageChange()
{
    retranslateUi(this);
}

/*! \class graphmonDialog
 *   \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */

void graphmonDialog::init(GraphicMonitor *gm, int dec, int mant)  {
    gmon = gm;
    setAccurateRestore(gmon->getAccurateRestore());
    minFloatSpinBox->setDecimals(dec);
    maxFloatSpinBox->setDecimals(dec); 
    setMin(gmon->getYMin());
    setMax(gmon->getYMax());
    setTimeUpd(gmon->getXUpd());
    setTLabels(gmon->getNXLabels());
		setTitulo(gmon->getTitulo());
    setVLabels(gmon->getNYLabels());

} 

double graphmonDialog::getMin(void )
{ 
    return min = minFloatSpinBox->value(); 
}

void graphmonDialog::setMin(double d)
{
    min = d;
    minFloatSpinBox->setValue(d);
}

double graphmonDialog::getMax( void )
{
    return max = maxFloatSpinBox->value();
}

void graphmonDialog::setMax( double d)
{
    max = d;
    maxFloatSpinBox->setValue(d);
}

int graphmonDialog::getTimeUpd( void )
{
    return timeupd = timeupdSpinBox->cleanText().toInt();
}

void graphmonDialog::setTimeUpd( int tu)
{
  if(tu > 0)  
    timeupd = tu;
  timeupdSpinBox->setValue(timeupd);
}

int graphmonDialog::getTLabels( void )
{
    return tLabels = tLabelsSpinBox->cleanText().toInt();
}

void graphmonDialog::setTLabels( int tl)
{
  if(tl > 1)  
    tLabels = tl;
  tLabelsSpinBox->setValue(tLabels);
}

int graphmonDialog::getVLabels( void )
{
    return vLabels = vLabelsSpinBox->cleanText().toInt();
}

void graphmonDialog::setVLabels( int vl)
{
  if(vl > 0)  
    vLabels = vl; 
  vLabelsSpinBox->setValue(vLabels);
}

int graphmonDialog::getAccurateRestore( void )
{
    return accurateRestore = accurateRestoreCheckBox->isChecked();
}

void graphmonDialog::setAccurateRestore( bool accres)
{
  if(accurateRestore != accres)  
    accurateRestore = accres; 
  accurateRestoreCheckBox->setChecked(accres);
}

void graphmonDialog::setInfor(const QString in) {
    inforLineEdit->setText(in);    
}
void graphmonDialog::accept(void) {
    if(getMax() <= getMin()) {
// QMessageBox::warning(this, tr("Edit monitoring..."),
//                                    tr("Maximun value must be greater or equal than minimun value"),  tr("&Return")) ;
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(tr("Edit monitoring..."));
    msgBox.setInformativeText(tr("Maximun value must be greater or equal than minimun value"));
    QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
    msgBox.exec();
 return; 
    }
    if(getTimeUpd() < 1) {
//      QMessageBox::warning(this, tr("Edit monitoring..."),
//                                    tr("Time update must be greater or equal than 1"),  tr("&Return")) ;
     QMessageBox msgBox;
     msgBox.setIcon(QMessageBox::Warning);
     msgBox.setText(tr("Edit monitoring..."));
     msgBox.setInformativeText(tr("Time update must be greater or equal than 1"));
     QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
     msgBox.exec();
      return; 
    }
    if(getTLabels() <= 1) {
//      QMessageBox::warning(this, tr("Edit monitoring..."),
//                                    tr("Number of time labels must be greater or equal than 2"),  tr("&Return")) ;
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.setText(tr("Edit monitoring..."));
      msgBox.setInformativeText(tr("Number of time labels must be greater or equal than 2"));
      QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
      msgBox.exec();
      return; 
    }  
    if(getVLabels() <  1) {
//      QMessageBox::warning(this, tr("Edit monitoring..."),
//                                    tr("Number of value labels must be greater or equal than 1"),  tr("&Return")) ;
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.setText(tr("Edit monitoring..."));
      msgBox.setInformativeText(tr("Number of value labels must be greater or equal than 1"));
      QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
      msgBox.exec();
      return; 
    }     
    
    gmon->setYMin(getMin());
    gmon->setYMax(getMax());
    gmon->setXUpd(getTimeUpd());
    gmon->setNXLabels(getTLabels());
    gmon->setNYLabels(getVLabels()); 
    gmon->setTitulo(titulo);
    gmon->setAccurateRestore( getAccurateRestore( ));
    QDialog::accept(); 
} 



void graphmonDialog::setTitulo( const QString & cap )
{
    titulo = cap;
    captionLineEdit->setText(titulo);
	
}


const QString& graphmonDialog::getTitulo( void )
{
	return titulo;
}
