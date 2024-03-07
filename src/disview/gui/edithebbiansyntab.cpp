#include "edithebbiansyntab.h"

#include <QVariant>
#include "../../net/hebbiansynapse.h"

/*
 *  Constructs a EditHebbianSynTab which is a child of 'parent', with the
 *  name 'name'.'
 */
EditHebbianSynTab::EditHebbianSynTab(QWidget* parent, const char* name)
    : QewExtensibleDialog(parent, name)
{
    setupUi(this);

    QObject::connect(learnFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setLRate(double)));

}

/*
 *  Destroys the object and frees any allocated resources
 */
EditHebbianSynTab::~EditHebbianSynTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void EditHebbianSynTab::languageChange()
{
    retranslateUi(this);
}

void EditHebbianSynTab::init( HebbianSynapse * hebb_syn)
{
    hebb_synapse = hebb_syn;
    setLRate(hebb_synapse->getLearnRate());
}


double EditHebbianSynTab::getLRate( void )
{
    return learn_rate = learnFloatSpinBox->value();
}

void EditHebbianSynTab::setLRate( double d)
{
    if(d >= 0.0 &&  d <= 1.0)
        learn_rate = d;
    learnFloatSpinBox->setValue(d);
}


void EditHebbianSynTab::saveThis(void)
{
    hebb_synapse->setLearnRate(learnFloatSpinBox->value());
}
