#include "textviewer.h"

#include <QVariant>

/*
 *  Constructs a TextViewer as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
TextViewer::TextViewer(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

}

/*
 *  Destroys the object and frees any allocated resources
 */
TextViewer::~TextViewer()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void TextViewer::languageChange()
{
    retranslateUi(this);
}

void TextViewer::initTextViewer(bool read_only)
{
    visorTexto->setReadOnly(read_only);
}

void TextViewer::slotTexto(QString texto_)
{
    visorTexto->setText(texto_);
}

void TextViewer::closeEvent( QCloseEvent* ) {
    emit closeViewer();
}

void TextViewer::destroy(void) {
    emit closeViewer();
}
