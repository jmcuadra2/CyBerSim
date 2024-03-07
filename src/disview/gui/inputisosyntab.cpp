#include "inputisosyntab.h"

#include <QVariant>
#include <QButtonGroup>
#include "../../net/isoinputsynapse.h"

/*
 *  Constructs a InputISOSynTab which is a child of 'parent', with the
 *  name 'name'.'
 */
InputISOSynTab::InputISOSynTab(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);

    stimButtonGroup = new QButtonGroup(this);
    stimButtonGroup->addButton(radioButtonU, 0);
    stimButtonGroup->addButton(radioButtonC, 1);

    QObject::connect(stimButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(setIsCS(int)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
InputISOSynTab::~InputISOSynTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void InputISOSynTab::languageChange()
{
    retranslateUi(this);
}

void InputISOSynTab::init(ISOInputSynapse *iso_syn )
{
    iso_synapse = iso_syn;
    setIsCS(int(iso_synapse->getIsCS()));

}

void InputISOSynTab::setIsCS( int is_cs )
{
    if(iso_synapse->validStim(bool(is_cs)))
        isCS = bool(is_cs);
    stimButtonGroup->buttons().at(int(isCS))->setChecked(true);
    return;

}

bool InputISOSynTab::getIsCS( void )
{
    return isCS;
}

void InputISOSynTab::saveThis(void)
{
    iso_synapse->setIsCS(isCS);
}
