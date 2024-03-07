/***************************************************************************
                          clocksdispatcher.h  -  description
                             -------------------
    begin                : Tue Apr 5 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CLOCKSDISPATCHER_H
#define CLOCKSDISPATCHER_H

#include <QTimer>
#include <QDateTime>
#include <QHash>

#include <QElapsedTimer>

class TimingResults;

/**
  *@author Jose M. Cuadra Troncoso
  */

//class NDClock : public QTime {

//    friend class ClocksDispatcher;

//public:
//    ~NDClock() {}
//    int getCounter(void) { return counter ; }

//protected:
//    NDClock();
//    void start(void);
//    void elapsed(void);
    
//protected:
//    int counter;
//};

//inline void NDClock::start(void)
//{

//    QTime::start();

//};

//inline void NDClock::elapsed(void)
//{

//    counter += QTime::elapsed();

//}

class NDClock : public QElapsedTimer {

    friend class ClocksDispatcher;

public:
    ~NDClock() {}
    int getCounter(void) { return counter ; }

protected:
    NDClock();
    void start(void);
    void elapsed(void);

protected:
    int counter;
};

inline void NDClock::start(void)
{

    QElapsedTimer::start();

};

inline void NDClock::elapsed(void)
{

    counter += QElapsedTimer::elapsed();

}

/**
  *@author Jose M. Cuadra Troncoso
  */


class ClocksDispatcher : public QTimer  {

    Q_OBJECT

    friend class TimingResults;

protected:
    ClocksDispatcher(QObject* parent = 0, const char* name = 0);
    
public:
    ~ClocksDispatcher();
    static ClocksDispatcher* instance(void);
    /** Anade un nuevo reloj (clock_name) a la lista del usuario
   (user_name) y anade al usuario a la lista de usuarios
   si no estaba antes */
    virtual void addClock(const QString& user_name,
                          const QString& clock_name);
    virtual void pauseClock(const QString& user_name,
                            const QString& clock_name);
    virtual void startClock(const QString& user_name,
                            const QString& clock_name);
    virtual void start(int adv_period);
    virtual void reset(void);
    virtual void startSim(void);
    virtual void stop(void);
    virtual void pauseSim(void);
    virtual void changeInterval (int msec);
    virtual void write(void);
    virtual void setRealAdvPeriod(int real_adv_period);
    virtual void setFileName(const QString& f_name);
    virtual void deleteClocks(void);

protected:
    virtual NDClock* findClock(const QString& user_name,
                               const QString& clock_name);
protected slots:
    virtual void countStep(void) { ++count_step ; }

signals:
    void Crono(int);
    void initCrono(int);
    void CronoReset(bool);
    
protected: // Protected attributes
    /** Relaciona un objeto(1 QString) con su relojes (2 QString) */
    typedef QHash<QString, NDClock*> UserClocks;
    QHash<QString, UserClocks*> users;
    NDClock total_time;
    int advance_period, real_advance_period;
    int count_step;
    QString file_name;
    static ClocksDispatcher* dispatch;
    
};

#endif
