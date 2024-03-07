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
#ifndef MEASURE_H
#define MEASURE_H

#include "timestamp.hpp"
#include "ndpose2d.hpp"
#include "ndpose3dquat.hpp"
#include "ndpose3dsph.hpp"
#include <sstream>

// #include <vector>
// #include <utility>
// #include <iostream>
using namespace std;

/**
  @author Jose Manuel Cuadra Troncoso <jose@portatil-jose>
*/
template<typename type_y = double>
class Measure
{

//   public:
//     enum NDPose::PoseType {NoType, Measure2D, Measure3DSph, Measure3DQuat};
    
  protected:
/*!
    \fn Measure::Measure(char* entity, char* unit, TimeStamp timeStamp, pair<size_t, size_t> xSizes, vector<double> x, TimeStamp timeStamp, vector<size_t> yDimensions, vector<type_y> y, vector<double> yUncertainity)
 */    
      
    Measure<type_y>() : entity(""), unit(""), pose(0), timeStamp(TimeStamp()), yDimensions(), y(), yUncertainity() {}; // este constructor necesita asignar pose

    Measure<type_y>(NDPose* pose, bool copyPose = true) : entity(""), unit(""), timeStamp(TimeStamp()), yDimensions(), y(), yUncertainity()
      {
        if(copyPose)
          this->pose = pose->deepCopy();
        else
          this->pose = pose;
      };     
    
  public:
    Measure<type_y>(const char* entity, const char* unit, NDPose* pose, TimeStamp timeStamp, vector<size_t> yDimensions, vector<type_y> y, vector<double> yUncertainity, bool copyPose = true) : entity(entity), unit(unit), timeStamp(timeStamp), yDimensions(yDimensions), y(y), yUncertainity(yUncertainity)
      {
        if(copyPose)
          this->pose = pose->deepCopy();
        else
          this->pose = pose;
      };

    Measure<type_y>(const char* entity, const char* unit, NDPose* pose, vector<size_t> yDimensions, vector<type_y> y, vector<double> yUncertainity, bool copyPose = true) : entity(entity), unit(unit), timeStamp(TimeStamp::nowMicro()), yDimensions(yDimensions), y(y), yUncertainity(yUncertainity)
      {
        if(copyPose)
          this->pose = pose->deepCopy();
        else
          this->pose = pose;
      };
  
    Measure<type_y>(Measure<type_y> const& measure) : entity(measure.entity), unit(measure.unit), pose(measure.pose->deepCopy()), timeStamp(measure.timeStamp), yDimensions(measure.yDimensions), y(measure.y), yUncertainity(measure.yUncertainity){ };

    void operator=(Measure<type_y> const& measure);

    ~Measure()
      {
        if(pose)
          delete pose;
      };

    static Measure<type_y> createMeasure(NDPose::PoseType type = NDPose::NoPoseType)
      {
        Measure<type_y> measure(NDPose::createPose(type), false);
        return measure;
      }

    void initMeasure(NDPose::PoseType type, const char* entity, const char* unit,
                     vector<size_t> yDimensions)
      {
        if(type == NDPose::NoPoseType)
          cerr << "Warning: Measure initialization must have a nonabstract pose type" << endl;
        else if(pose) 
          cerr << "Warning: Measure already initialized" << endl;
        else {
          setEntity(entity);
          setUnit(unit);
          setPose(NDPose::createPose(type), false);
          vector<double> y(properYSize(yDimensions), 0);
          setYDimensions(yDimensions);
          setY(y);
	  setTimeStamp();
        }
      }      

//     static NDPose* createPose(NDPose::PoseType type)
//       {
//         NDPose* pose = 0;
//         switch(type) {
//           case NDPose::Pose2D:
//             pose = new NDPose2D();
//             break;
//           case NDPose::Pose3DSph:
//             pose = new NDPose3DSph();
//             break;
//           case NDPose::Pose3DQuat:
//             pose = new NDPose3DQuat();
//             break;
//           default:
//             break;                      
//         }
//         return pose;
//       }

    void printMeasure(void);
    
    void setEntity(const char* entity) { this->entity = entity ; }
    const char* getEntity() const { return entity ; }
    void setUnit(const char* unit) { this->unit = unit ; }
    const char* getUnit() const { return unit ; }
    
    bool setPose(NDPose* poseOrigin, bool copyPose = true)
      {
        bool ret = NDPose::safeCopy(poseOrigin, pose, copyPose);
        if(!ret)
          cerr << "Warning: Measure can't copy pose" << endl;
        return ret;
      };
      
    NDPose* getPose(void) const { return pose ; }
    
    void setTimeStamp(const TimeStamp& timeStamp = TimeStamp::nowMicro()) { this->timeStamp = timeStamp; }
    TimeStamp getTimeStamp() const { return timeStamp ; }
    
    void setYDimensions(const vector< size_t >& yDimensions) { this->yDimensions = yDimensions ; }
    vector< size_t > getYDimensions() const { return yDimensions ; }
    
    void setY(const vector< type_y >& y)
      {
        if(validYSize(y.size()))
          this->y = y;
        else
          cerr << "Warning: Measure with invalid y size, y not changed " << y.size() << endl;
      }
    void setY(type_y const& y)
      {
          this->y[0] = y;
      }      
    vector< type_y > getY() const { return y ; }
    
    void setYUncertainity(const vector< double >& yUncertainity)
      {
        if(!yUncertainity.size() || yUncertainity.size() == y.size())
          this->yUncertainity = yUncertainity;
        else
          cerr << "Warning: Measure with invalid yUncertainity size, yUncertainity not changed" << endl;
      }
    vector< double > getYUncertainity() const { return yUncertainity; }

    bool validYSize(size_t ySize) { return validYSize(ySize, yDimensions) ; }
    static bool validYSize(size_t ySize, vector<size_t> yDimensions)
      {
        return (ySize == properYSize(yDimensions));
      }

    static size_t properYSize(vector<size_t> yDimensions)
      {
        size_t size = yDimensions.size();
        size_t prod = size ? 1 : 0;
        for(unsigned int i = 0; i < size; i++)
          prod *= yDimensions.at(i);      
        return prod;
      }

  protected:
    const char* entity;
    const char* unit;
    NDPose* pose;
    TimeStamp timeStamp;
    vector<size_t> yDimensions;
    vector<type_y> y;
    vector<double> yUncertainity; // ¿ o tiene las dimensiones yDimensions o esta vacia ?

};

template<typename type_y>
  void Measure<type_y>::printMeasure(void)
  {
    cout << "Entity: " << entity << endl;
    cout << "Unit: " << unit << endl;
    cout << "TimeStamp: " << timeStamp << endl;
    cout << *pose << endl;
    
    cout << "yDimension[ ";
    for(unsigned int i = 0; i < yDimensions.size(); i++)
      cout << yDimensions.at(i) << " ";
    cout << "]" << endl;

    cout << "y[";
    for(unsigned int i = 0; i < y.size(); i++)
      cout << y.at(i) << " ";
    cout << "]" << endl;

    cout << "yUncertainity[";
    for(unsigned int i = 0; i < yUncertainity.size(); i++)
      cout << yUncertainity.at(i) << " ";
    cout << "]" << endl;
  }

template<typename type_y>
  void Measure<type_y>::operator=(Measure<type_y> const& measure)
  {
    if(setPose(measure.pose)) {
      entity = measure.entity;
      unit = measure.unit;
      timeStamp = measure.timeStamp;
      yDimensions = measure.yDimensions;
      y = measure.y;
      yUncertainity = measure.yUncertainity;
    }
  }

#endif
