#include "irsmonitor.h"
#include "../../neuraldis/settings.h"
#include <QVariant>


/*
 *  Constructs a IRSMonitor as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
IRSMonitor::IRSMonitor(QWidget* parent, const char* name, Qt::WindowFlags fl)
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
    QObject::connect(this, SIGNAL(lVel(QString)), vLeftLCD, SLOT(display(QString)));
    QObject::connect(this, SIGNAL(rVel(QString)), vRightLCD, SLOT(display(QString)));
    
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(QPixmap(Settings::instance()->getAppDirectory() + "src/robot/imagenes/robot_big.xpm")));
    setPalette(palette);
}

/*
 *  Destroys the object and frees any allocated resources
 */
IRSMonitor::~IRSMonitor()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void IRSMonitor::languageChange()
{
    retranslateUi(this);
}

void IRSMonitor::setLCDColorBackGround(QLCDNumber *lcd, QColor color)
{
    QPalette pal(lcd->palette());
    pal.setColor(QPalette::Window, color);
    lcd->setAutoFillBackground(true);
    lcd->setPalette(pal);
}

/*! \class IRSMonitor
 *   \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */

void IRSMonitor::init(void) {

    setAttribute(Qt::WA_DeleteOnClose);
    ident_offset = 0;

}

void IRSMonitor::setLCD(int val, int ident)
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
void IRSMonitor::setRVel( double rv )
{
    QString n = QString::number(rv, 'f', 1);
    emit rVel(n);
}

void IRSMonitor::setLVel( double lv )
{
    emit lVel(QString::number(lv, 'f', 1));
}


void IRSMonitor::setIdOffset( int id_offset )
{
    ident_offset = id_offset - 1;
}

void IRSMonitor::setSensorLabel( const QString & label )
{
    sensorLabel->setText(label);
}

void IRSMonitor::startLCD(int ident)
{
    int pos = ident - ident_offset;
    pos = pos > 8 ? pos%8 : pos;
    pos = !pos ? 8 : pos;
    pos = pos < 0 ? -pos : pos;

    switch(pos) {
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


