#include "responseframe.h"
#include "../../neuraldis/ndmath.h"

#include <QPainter>
#include <QPen>

ResponseFrame::ResponseFrame(QWidget *parent, Qt::WindowFlags f) : QFrame(parent, f)
{

}

void ResponseFrame::paintEvent(QPaintEvent *event)
{
    int i, i_fmax, n_max;
    double yn,  yn_1,  yn_2,  r,  th, norm;
    yn = 0.0;
    yn_1 = 0.0;
    yn_2 = 0.0;
    r = exp(-damp*samplingPeriod);
    th =  3.1415926535/hper;
    n_max = int(1/th*atan((th/samplingPeriod)/damp)-0.5);
    norm = pow(r, n_max)*sin((n_max+1)*th)/sin(th);
    i_fmax = n_max + 1;
    QVector<double> xn;
    xn.resize(i_fmax*10+1);
    xn.fill(0.0);
    xn[i_fmax+1] = 1.0;
    double x_pixel = width()/(i_fmax*10.0);
    double y_pixel = (height() - 20.0)*0.8;
    changeXLabels(i_fmax);
    QPainter p;
    p.begin(this);
    p.eraseRect(1, 1, width()-2, height()-2);
    p.save();
    p.setPen(QPen(QColor(Qt::gray), 1));
    for(i = 1; i < 10; i++) {
        p.drawLine(NDMath::roundInt(i*(i_fmax)*x_pixel), 1,
                   NDMath::roundInt(i*i_fmax*x_pixel), height()-1);
    }
    for(i = 0; i <11 ; i++) {
        p.drawLine(1, 10+NDMath::roundInt(i*(height() - 20.0)/10.0),
                   width()-1,
                   10+NDMath::roundInt(i*(height() - 20.0)/10.0));
    }
    p.restore();
    p.save();
    p.setPen(QPen(QColor(Qt::black), 2));
    p.setBrush(QBrush(Qt::black));

    for(i=0; i <= i_fmax*10; i++) {
        yn = (xn[i] + 2*r*cos(th)*yn_1 - r*r*yn_2);
        yn_2 = yn_1;
        yn_1 = yn;
        p.drawLine(NDMath::roundInt(x_pixel*i), 10 + NDMath::roundInt((height() - 20)*0.8),
                   NDMath::roundInt(x_pixel*i), 10 + NDMath::roundInt((height() -20 )*0.8 - y_pixel*yn/norm));
        p.drawEllipse(NDMath::roundInt(x_pixel*i)-2,
                      10 + NDMath::roundInt((height() - 20)*0.8 - y_pixel*yn/norm)-2, 4, 4);
    }
    p.restore();
    p.end();
}

void ResponseFrame::setDamp(double damp)
{
    this->damp = damp;
}

void ResponseFrame::setSamplingPeriod(double samplingPeriod)
{
    this->samplingPeriod = samplingPeriod;
}

void ResponseFrame::setHper(double hper)
{
    this->hper = hper;
}
