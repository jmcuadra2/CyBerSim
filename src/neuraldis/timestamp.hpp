/***************************************************************************
 *   Copyright (C) 2008 by Jose Manuel Cuadra Troncoso   *
 *   jose@portatil-jose   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <utility>
// #include <QDateTime>
#include <sys/timeb.h>
#include <sys/time.h>
#include <QtGlobal>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ariaUtil.h>

#define UNITS_PER_SECOND 1000000
 
using namespace std;

/**
  @author Jose Manuel Cuadra Troncoso <jose@portatil-jose>
*/
class TimeStamp : public std::pair<int, int>
{
  public:
    TimeStamp(int seconds, int microseconds = 0, bool normalize = true) : std::pair<int, int>(seconds, microseconds) 
    {
      if(normalize) {
        if(microseconds > -UNITS_PER_SECOND && microseconds < UNITS_PER_SECOND) {
          normalizeTS(first, second);
          valid = true;
        }
        else {
          cerr << "Error TimeStamp constructor: invalid microseconds" << endl;
          first = 0;
          second = 0;
        }
      } 
      else
        valid = true;
//       setValid() ; 
    };
    
    
    TimeStamp() : std::pair<int, int>(0, 0), valid(false) {}; // inicializado invalido
    
    TimeStamp(ArTime time) {
      TimeStamp(time.getSec(), time.getMSec()*1000, false);
    }
    
    void fromArTime(ArTime time) {
      first = time.getSec();
      second = time.getMSec()*1000;
    }
    
    ArTime toArTime() { 
      ArTime time;
      time.setSec(first);
      time.setMSec(second/1000);
      return time;
    }
    
    int seconds(void) const { return first ; }
    int microseconds(void) const { return second ; }
    bool isValid(void) { return valid ; };
    
    friend ostream &operator<<(ostream &stream, TimeStamp timestamp)
    {
      if(!timestamp.first) {
        if(timestamp.second < 0)
          stream << "-";
      }
      stream << timestamp.first << ".";
      stream.width(6);
      stream.fill('0');
      stream << abs(timestamp.second)/* << " secs."*/;
      return stream;
    }
    
    TimeStamp operator -(const TimeStamp& timestamp)
    {
      int secs;
      int microsecs, microsecs_this, microsecs_ts;
      
      secs = first - timestamp.first;
      if(first < 0)
        microsecs_this = -second; // microsecs debe ser <= 0 para ser compatible con first
      else
        microsecs_this = second;
      if(timestamp.first < 0)
        microsecs_ts = -timestamp.second;
      else
        microsecs_ts = timestamp.second;       
      microsecs = microsecs_this - microsecs_ts;
      
      normalizeTS(secs, microsecs);   
        
      return(TimeStamp(secs, microsecs, false));
    } 
       
    TimeStamp operator +(const TimeStamp& timestamp)
    {
      int secs;
      int microsecs, microsecs_this, microsecs_ts;
      
      secs = first + timestamp.first;
      if(first < 0)
        microsecs_this = -second; // microsecs debe ser <= 0 para ser compatible con first
      else
        microsecs_this = second;
      if(timestamp.first < 0)
        microsecs_ts = -timestamp.second;
      else
        microsecs_ts = timestamp.second;       
      microsecs = microsecs_this + microsecs_ts;
      
      normalizeTS(secs, microsecs);
        
      return(TimeStamp(secs, microsecs, false));
    }   
        
    TimeStamp operator +=(const TimeStamp& timestamp)
    {
      *this = *this + timestamp;
      return *this;
    }               
    
    static TimeStamp nowMicro(void)  
    {
      struct timeval tv;
      gettimeofday(&tv, 0);
      return TimeStamp(tv.tv_sec, tv.tv_usec, false);
    }
     
    char* toChar(void) {
      static char s[30];
      sprintf(s, "%d.%06d", first, second);
      return s;
    }
    
//    static TimeStamp now(void)
//    {
//      #ifndef Q_OS_WIN
//        struct timeb tp;
//        ftime(&tp);
//      #endif
//      #ifdef Q_OS_WIN
//        struct _timeb tp;
//        _ftime_s(&tp);
//      #endif
//        return TimeStamp(tp.time, tp.millitm*UNITS_PER_SECOND/1000,false);
//    }

  protected:
    // emplear solo en el constructor o en operaciones
    // NO emplear sobre una t_s válida, si es < 0
    void normalizeTS(int& secs, int& microsecs) {
      if(secs > 0) {  // microsecs debe ser >= 0 para ser compatible
        if (microsecs < 0) {  // al menos un t_s es > 0 --> su ms >= 0 --> microsecs > -UNITS_PER_SECOND
          secs -= 1;
          microsecs += UNITS_PER_SECOND;
        }
        else if(microsecs >= UNITS_PER_SECOND) {
          secs += 1;
          microsecs -= UNITS_PER_SECOND;
        }        
      }
      else if(secs < 0) {  // microsecs debe ser <= 0 para ser compatible con secs
        if(microsecs <= -UNITS_PER_SECOND) {
          secs -= 1;
          microsecs += UNITS_PER_SECOND;
        }
        else if(microsecs > 0) {
          secs += 1;
          microsecs -= UNITS_PER_SECOND;
        }
        if(secs)
          microsecs = -microsecs; // se escriben positivos en el t_s
      }
      else {
        if(microsecs >= UNITS_PER_SECOND) {
          secs += 1;
          microsecs -= UNITS_PER_SECOND;
        }
        else if(microsecs <= -UNITS_PER_SECOND) {
          secs -= 1;
          microsecs += UNITS_PER_SECOND;
        }
      }
    }
//     QDateTime toDateTime(void);

  private:
    void setValid(void) { valid = (first >= 0 && second >= 0 && second < UNITS_PER_SECOND) || (first < 0 && second > -UNITS_PER_SECOND && second <= 0); }

  private:
    bool valid;

};

#endif
