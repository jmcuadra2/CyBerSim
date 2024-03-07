#include "lightmonitor.h"
#include "../../neuraldis/settings.h"

#include <QVariant>

/*
 *  Constructs a lightMonitor as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
lightMonitor::lightMonitor(QWidget* parent, const char* name, Qt::WindowFlags fl)
    : QWidget(parent, fl)
{
    setupUi(this);

    setObjectName(name);

    QObject::connect(this, SIGNAL(lcd8(int)), LCD8, SLOT(display(int)));
    QObject::connect(this, SIGNAL(lcd1(int)), LCD1, SLOT(display(int)));
    QObject::connect(this, SIGNAL(lcd2(int)), LCD2, SLOT(display(int)));
    QObject::connect(this, SIGNAL(lcd3(int)), LCD3, SLOT(display(int)));
    QObject::connect(this, SIGNAL(lcd4(int)), LCD4, SLOT(display(int)));
    QObject::connect(this, SIGNAL(lcd5(int)), LCD5, SLOT(display(int)));
    QObject::connect(this, SIGNAL(lcd6(int)), LCD6, SLOT(display(int)));
    QObject::connect(this, SIGNAL(lcd7(int)), LCD7, SLOT(display(int)));
    QObject::connect(this, SIGNAL(lcdbumpDetect(int)), LCDBumpDetect, SLOT(display(int)));
    QObject::connect(this, SIGNAL(lcdbumpNoDetect(int)), LCDBumpNoDetect, SLOT(display(int)));
    
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(QPixmap(Settings::instance()->getAppDirectory() + "src/robot/imagenes/robot_big.xpm")));
    setPalette(palette);

}

/*
 *  Destroys the object and frees any allocated resources
 */
lightMonitor::~lightMonitor()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void lightMonitor::languageChange()
{
    retranslateUi(this);
}

/*! \class lightMonitor
 *   \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */

void lightMonitor::init(void) {

    ident_offset = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    is_already_bump_no_detect = false;
    n_bump_no_detect = 0;
    is_already_bump_detect = false;
    n_bump_detect = 0;
}

void lightMonitor::setLCD(int val, int ident)
{
    int pos = ident - ident_offset;
    pos = pos > 8 ? pos%8 : pos;
    pos = !pos ? 8 : pos;
    pos = pos < 0 ? -pos : pos;
    switch(pos)  {
    case 1:
        emit lcd1(val);
        break;
    case 2:
        emit lcd2(val);
        break;
    case 3:
        emit lcd3(val);
        break;
    case 4:
        emit lcd4(val);
        break;
    case 5:
        emit lcd5(val);
        break;
    case 6:
        emit lcd6(val);
        break;
    case 7:
        emit lcd7(val);
        break;
    case 8:
        emit lcd8(val);
        break;
    }
}

void lightMonitor::resetBump( void )
{
    n_bump_detect  = 0;
    is_already_bump_detect  = 0;
    emit lcdbumpDetect (0);
    n_bump_no_detect = 0;
    is_already_bump_no_detect = 0;
    emit lcdbumpNoDetect (0);
}

void lightMonitor::addBumpDetect(int /*index*/, int val)		
{
    if(val) {
        if(!is_already_bump_detect) {
            is_already_bump_detect = 2;
            ++n_bump_detect;
            emit lcdbumpDetect (n_bump_detect);
        }
    }
    else
        is_already_bump_no_detect = 0;
}

void lightMonitor::initBumpDetect( void )
{
    if(is_already_bump_detect) --is_already_bump_detect;
}

void lightMonitor::addBumpNoDetect( void )		
{
    if(!is_already_bump_no_detect) {
        is_already_bump_no_detect = 2;
        ++n_bump_no_detect;
        emit lcdbumpNoDetect(n_bump_no_detect);
    }
}

void lightMonitor::initBumpNoDetect( void )
{
    if(is_already_bump_no_detect) --is_already_bump_no_detect;
}


void lightMonitor::setIdOffset( int id_off )
{
    ident_offset = id_off - 1;
}


void lightMonitor::setSensorLabel( const QString & label )
{
    sensorLabel->setText(label);
}

void lightMonitor::setLCDColorBackGround(QLCDNumber *lcd, QColor color)
{
    QPalette pal(lcd->palette());
    pal.setColor(QPalette::Window, color);
    lcd->setAutoFillBackground(true);
    lcd->setPalette(pal);
}

void lightMonitor::startLCD(int ident)
{
    int pos = ident - ident_offset;
    pos = pos > 8 ? pos%8 : pos;
    pos = !pos ? 8 : pos;
    pos = pos < 0 ? -pos : pos;
    switch(pos)  {
    case 1:
        setLCDColorBackGround(LCD1, QColor(255, 105, 105));
        break;
    case 2:
        setLCDColorBackGround(LCD2, QColor(255, 105, 105));
        break;
    case 3:
        setLCDColorBackGround(LCD3, QColor(255, 105, 105));
        break;
    case 4:
        setLCDColorBackGround(LCD4, QColor(255, 105, 105));
        break;
    case 5:
        setLCDColorBackGround(LCD5, QColor(255, 105, 105));
        break;
    case 6:
        setLCDColorBackGround(LCD6, QColor(255, 105, 105));
        break;
    case 7:
        setLCDColorBackGround(LCD7, QColor(255, 105, 105));
        break;
    case 8:
        setLCDColorBackGround(LCD8, QColor(255, 105, 105));
        break;
    }
}
