#include "resonatordialog.h"

#include <QVariant>
//#include <QPainter>
//#include <QPaintEvent>
#include <QButtonGroup>

#include "../../neuraldis/ndmath.h"
#include "../../net/resonator.h"

/*
 *  Constructs a resonatorDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
resonatorDialog::resonatorDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    stimulButtonGroup = new QButtonGroup(this);
    stimulButtonGroup->addButton(CSRadioButton, 0);
    stimulButtonGroup->addButton(USRadioButton, 1);

    connect(OkButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(dampFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setDamp(double)));
    connect(hperSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setHalfPeriod(int)));
    connect(linkCheckBox, SIGNAL(toggled(bool)), this, SLOT(setLink(bool)));
    connect(stimulButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(setStimulus(int)));
    connect(weightFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setWeight(double)));
    connect(wfixCheckBox, SIGNAL(toggled(bool)), this, SLOT(setWFix(bool)));
    connect(fmaxFloatSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setFirstMax(double)));
    connect(nonNCheckBox, SIGNAL(toggled(bool)), this, SLOT(setNonN(bool)));
    connect(recmonCheckBox, SIGNAL(toggled(bool)), this, SLOT(setRecMonable(bool)));
    connect(graphFrame, SIGNAL(changeXLabels(int)), this, SLOT(changeXLabels(int)));
    
//    graphFrame->setAttribute(Qt::WA_PaintOutsidePaintEvent);
    
}

/*
 *  Destroys the object and frees any allocated resources
 */
resonatorDialog::~resonatorDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void resonatorDialog::languageChange()
{
    retranslateUi(this);
}

void resonatorDialog::changeXLabels( int c )
{
    int i = -1;
    QListIterator<QLabel*> it(xlabs);
    while(it.hasNext()) {
        it.next()->setText(QString::number(i*c));
        ++i;
    }
}

void resonatorDialog::init( Resonator *r , bool mode)
{
    OkButton->setEnabled(mode);
    res = r;
    damp = res->getDamp();
    hper = res->getHalfPeriod();
    weight = res->getWeight();
    wfix = res->getWeightFix();
    nonneg = res->getPosWeight();
    stimul = res->isCStim();
    rec_mon = res->isRecMonable();
    link = false;
    no_init = false;
    double th;
    th = 3.1415926535/hper;
    fmax = 1/th*atan(th/res->getSamplingPeriod()/damp);

    setStimulus(int(stimul));
    hperSpinBox->setValue(hper);
    dampFloatSpinBox->setValue(damp);
    weightFloatSpinBox->setValue(weight);
    fmaxFloatSpinBox->setValue(fmax);
    setRecMonable(rec_mon);
    setWFix(wfix);
    setNonN(nonneg);
//    x_pixel = graphFrame->width()/((int(fmax) + 1)*10.0);
//    y_pixel = (graphFrame->height() - 20.0)*0.8;
    xlabs.append(xLabel0);
    xlabs.append(xLabel1);
    xlabs.append(xLabel2);
    xlabs.append(xLabel3);
    xlabs.append(xLabel4);
    xlabs.append(xLabel5);
    xlabs.append(xLabel6);
    xlabs.append(xLabel7);
    xlabs.append(xLabel8);
    xlabs.append(xLabel9);
    xlabs.append(xLabel10);
    changeXLabels((int(fmax) + 1));

    hperSpinBox->setFocus();

    graphFrame->setDamp(damp);
    graphFrame->setHper(hper);
    graphFrame->setSamplingPeriod(res->getSamplingPeriod());


} 

int resonatorDialog::getHalfPeriod( void)
{
    return hper = hperSpinBox->cleanText().toInt();
}

void  resonatorDialog::setHalfPeriod(int hp)
{
    if(!no_init) {
        hper = hp ;
        hperSpinBox->setValue(hper);
        no_init = true;
        return;
    }
    if(link) {
        if(!first2damp())
            hper = prevHper;
        else {
            hper = hp ;
            hperSpinBox->setValue(hper);
            prevHper = hper;
        }
    }
    else {
         if(!damp2first())
             hper = prevHper;
         else {
             hper = hp ;
             hperSpinBox->setValue(hper);
             prevHper = hper;
         }
    }
    graphFrame->setHper(hper);
}

double resonatorDialog::getWeight( void )
{
    return weight = weightFloatSpinBox->value();
} 

void resonatorDialog::setWeight( double w)
{
    weight = w;
    weightFloatSpinBox->setValue(w);
}


bool resonatorDialog::getWFix(void)
{
    return wfix;
}

void resonatorDialog::setWFix(bool wf)
{
    wfix= wf;
    wfixCheckBox->setChecked(wfix);
}

void resonatorDialog::setNonN(bool nn)
{
    nonneg= nn;
    nonNCheckBox->setChecked(nonneg);
}

double resonatorDialog::getDamp( void )
{
    return damp = dampFloatSpinBox->value();
} 

void resonatorDialog::setDamp( double d)
{
    damp = d;
    dampFloatSpinBox->setValue(d);
    if(!damp2first())
        damp = prevDamp;
    else
        graphFrame->setDamp(damp);
}

void resonatorDialog::setFirstMax( double d)
{
    fmax = d;
    fmaxFloatSpinBox->setValue(d);
    if(!first2damp())
        fmax = prevFMax;
}

bool resonatorDialog::first2damp() {
    double th;    
    th =  3.1415926535/hper;
    damp = th/tan(fmax*th)/res->getSamplingPeriod();
    if(damp < 0) {
        damp = prevDamp;
        return false;
    }
    dampFloatSpinBox->setValue(damp);
    graphFrame->setDamp(damp);
    prevDamp = damp;
    return true;
}

bool resonatorDialog::damp2first() {
    double th;
    th =  3.1415926535/hper;
    fmax = 1/th*atan(th/res->getSamplingPeriod()/damp);
    if(fmax < 0) {
        fmax = prevFMax;
        return false;
    }
    fmaxFloatSpinBox->setValue(fmax);
    prevFMax = fmax;
    return true;
}

void resonatorDialog::setLink(bool l)
{
    link = l;
}

void resonatorDialog::paintEvent(QPaintEvent* e)
{
//    int tipo = e->type();
//    paintGraph();
    graphFrame->update(graphFrame->rect());
}

bool resonatorDialog::isCStimul( void)
{
    return stimul;
} 

void resonatorDialog::setStimulus(int s)
{
    stimul = bool(s);
    stimulButtonGroup->buttons().at(s)->setChecked(true);
}

//void resonatorDialog::paintGraph(void) {

//    int i, i_fmax, n_max;
//    double yn,  yn_1,  yn_2,  r,  th, norm;
//    yn = 0.0;
//    yn_1 = 0.0;
//    yn_2 = 0.0;
//    r = exp(-damp*res->getSamplingPeriod());
//    th =  3.1415926535/hper;
//    n_max = int(1/th*atan((th/res->getSamplingPeriod())/damp)-0.5);
//    norm = pow(r, n_max)*sin((n_max+1)*th)/sin(th);
//    i_fmax = n_max + 1;
//    xn.resize(i_fmax*10+1);
//    xn.fill(0.0);
//    xn[i_fmax+1] = 1.0;
//    x_pixel = graphFrame->width()/(i_fmax*10.0);
//    y_pixel = (graphFrame->height() - 20.0)*0.8;
//    changeXLabels(i_fmax);
//    QPainter p;
//    p.begin(graphFrame);
//    p.eraseRect(1, 1, graphFrame->width()-2, graphFrame->height()-2);
//    p.save();
//    p.setPen(QPen(QColor(Qt::gray), 1));
//    for(i = 1; i < 10; i++) {
//        p.drawLine(NDMath::roundInt(i*(i_fmax)*x_pixel), 1,
//                   NDMath::roundInt(i*i_fmax*x_pixel), graphFrame->height()-1);
//    }
//    for(i = 0; i <11 ; i++) {
//        p.drawLine(1, 10+NDMath::roundInt(i*(graphFrame->height() - 20.0)/10.0),
//                   graphFrame->width()-1,
//                   10+NDMath::roundInt(i*(graphFrame->height() - 20.0)/10.0));
//    }
//    p.restore();
//    p.save();
//    p.setPen(QPen(QColor(Qt::black), 2));
//    p.setBrush(QBrush(Qt::black));

//    for(i=0; i <= i_fmax*10; i++) {
//        yn = (xn[i] + 2*r*cos(th)*yn_1 - r*r*yn_2);
//        yn_2 = yn_1;
//        yn_1 = yn;
//        p.drawLine(NDMath::roundInt(x_pixel*i), 10 + NDMath::roundInt((graphFrame->height() - 20)*0.8),
//                   NDMath::roundInt(x_pixel*i), 10 + NDMath::roundInt((graphFrame->height() -20 )*0.8 - y_pixel*yn/norm));
//        p.drawEllipse(NDMath::roundInt(x_pixel*i)-2,
//                      10 + NDMath::roundInt((graphFrame->height() - 20)*0.8 - y_pixel*yn/norm)-2, 4, 4);
//    }
//    p.restore();
//    p.end();
//}

void resonatorDialog::accept() {
    res->setDamp(getDamp()) ;
    res->setHalfPeriod(getHalfPeriod());
    res->setCStim(isCStimul());
    res->setWeight(getWeight());
    res->setWeightFix(getWFix());
    res->setPosWeight(nonneg);
    QDialog::accept();
    
}

void resonatorDialog::setRecMonable( bool recmon )
{
    rec_mon = recmon;
    recmonCheckBox->setChecked(rec_mon);
}
