#ifndef RESPONSEFRAME_H
#define RESPONSEFRAME_H

#include <QFrame>

class ResponseFrame : public QFrame
{

    Q_OBJECT

public:
    ResponseFrame(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    void setHper(double hper);
    void setSamplingPeriod(double samplingPeriod);
    void setDamp(double damp);

//protected:
    void paintEvent(QPaintEvent *event);

signals:
    void changeXLabels(int);

private:
    double hper;
    double samplingPeriod;
    double damp;
};

#endif // RESPONSEFRAME_H
