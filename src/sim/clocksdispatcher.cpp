/***************************************************************************
                          clocksdispatcher.cpp  -  description
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

#include "clocksdispatcher.h"
#include "gui/timingresults.h"

ClocksDispatcher* ClocksDispatcher::dispatch = 0;

//NDClock::NDClock() : QTime(){
    
//    counter = 0;
    
//}
NDClock::NDClock() : QElapsedTimer(){

    counter = 0;

}

ClocksDispatcher::ClocksDispatcher(QObject* parent, const char* name)
    : QTimer(parent)
{
    
    real_advance_period = 20;
    advance_period = real_advance_period;
    count_step = 0;
    setObjectName(name);
    connect(this, SIGNAL(timeout(void)), this, SLOT(countStep(void)));
    
}

ClocksDispatcher::~ClocksDispatcher(){
    
    deleteClocks();
    
}

ClocksDispatcher* ClocksDispatcher::instance(void)
{
    
    if(dispatch == 0)
        dispatch = new ClocksDispatcher();
    return dispatch;
    
}

/** Añade un nuevo reloj (clock_name) a la lista del usuario (user_name)
   y añade al usuario a la lista de usuarios si no estaba antes */
void ClocksDispatcher::addClock(const QString& user_name, const QString& clock_name)
{
    
    UserClocks* userClocks;
    if(users.contains(user_name))
        userClocks = users[user_name];
    else {
        userClocks = new UserClocks();
        users.insert(user_name, userClocks);
        NDClock* sim_time = new NDClock();
        userClocks->insert(tr("Simulation"), sim_time);
    }
    NDClock* clock = new NDClock();
    userClocks->insert(clock_name, clock);
    
}

void ClocksDispatcher::pauseClock(const QString& user_name,
                                  const QString& clock_name)
{
    
    NDClock* clock = findClock(user_name, clock_name);
    if(clock)
        clock->elapsed();
    
}

void ClocksDispatcher::startClock(const QString& user_name,
                                  const QString& clock_name)
{
    
    NDClock* clock = findClock(user_name, clock_name);
    if(clock)
        clock->start();
    
}

NDClock* ClocksDispatcher::findClock(const QString& user_name,
                                     const QString& clock_name)
{
    
    NDClock* clock = 0;
    UserClocks userClocks;
    if(users.contains(user_name)) {
        userClocks = *(users[user_name]);
        if(userClocks.contains(clock_name))
            clock = userClocks[clock_name];
    }
    
    return clock;
    
}

void ClocksDispatcher::start(int adv_period)
{
    
    advance_period = adv_period;
    QTimer::start(advance_period);
    total_time.start();
    
}

void ClocksDispatcher::reset(void)
{
    
    QHashIterator<QString, UserClocks*> it(users);
    while(it.hasNext()) {
        UserClocks userClocks = *(it.next().value());
        QHashIterator<QString, NDClock*> itc(userClocks);
        while(itc.hasNext())
            itc.next().value()->counter = 0;
    }
    count_step = 0;
    total_time.counter = 0;
    
}

void ClocksDispatcher::startSim(void)
{
    
    QHashIterator<QString, UserClocks*> it(users);
    while(it.hasNext()) {
        UserClocks userClocks = *(it.next().value());
        userClocks["Simulation"]->start();
    }
}

void ClocksDispatcher::stop(void)
{
    
    QTimer::stop();
    total_time.elapsed();
    
}

void ClocksDispatcher::pauseSim(void)                                                     
{
    
    QHashIterator<QString, UserClocks*> it(users);
    while(it.hasNext()) {
        UserClocks userClocks = *(it.next().value());
        userClocks["Simulation"]->elapsed();
    }
    
}

void ClocksDispatcher::write(void)
{
    
    if(!count_step)
        return;
    
    TimingResults* viewer= new TimingResults();
    viewer->init(this);
    viewer->write();
    viewer->exec();

    delete viewer;
    
}

void ClocksDispatcher::changeInterval(int msec)
{
    
    advance_period = msec;
    setInterval(msec);
    
}

void ClocksDispatcher::setRealAdvPeriod(int real_adv_period)
{
    
    real_advance_period = real_adv_period ;
    advance_period = real_advance_period;
    
}

void ClocksDispatcher::setFileName(const QString& f_name)
{
    
    file_name = f_name;
    file_name.replace(".sim", ".log");
    
}

void ClocksDispatcher::deleteClocks(void)
{
    
    QHashIterator<QString, UserClocks*> it(users);
    while(it.hasNext()) {
        UserClocks* userClocks = it.next().value();
        QHashIterator<QString, NDClock*> itc(*userClocks);
        while(itc.hasNext())
            delete itc.next().value();
        userClocks->clear();
        delete userClocks;
    }
    users.clear();
    count_step = 0;
    total_time.counter = 0;
    
}
