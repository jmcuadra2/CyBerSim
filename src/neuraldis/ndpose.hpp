#ifndef NDPOSE_HPP
#define NDPOSE_HPP

#include <vector>
#include <cmath>
#include <iostream>
#include <cstring>
#include <QTextStream>

using namespace std;
// 
// #define vectorArray(x, mx) vectorFromArray<double>(x, mx, sizeof(mx))
// 
// template <typename T>
// void vectorFromArray(vector<T>& vect, T array[], size_t asize)
// {
//   vect.assign(&array[0], &array[asize/sizeof(T)]);
// }

typedef pair<size_t, size_t> SizesPose;

/**
  @author Jose Manuel Cuadra Troncoso <jose@portatil-jose>
*/
class NDPose
{

  public:
    enum PoseType {NoPoseType, Pose2D, Pose3DSph, Pose3DQuat};

  protected:
    NDPose(SizesPose sizesPose = SizesPose(0, 0))
      {
        position.assign(sizesPose.first, 0);
        orientation.assign(sizesPose.second, 0);
      }
  public:    
    virtual ~NDPose() {}
    
    NDPose(NDPose const& pose)
      {
          position = pose.getPosition();
          orientation = pose.getOrientation();
      }
      
    virtual void operator=(NDPose const& pose)
      {
        if((getType() == NoPoseType) || (getType() == pose.getType())) {
          position = pose.getPosition();
          orientation = pose.getOrientation();
        }
      }

    static bool safeCopy(NDPose* poseOrigin, NDPose* & poseDestination, bool copyPose = true)
      {
        bool ret = canSafeCopy(poseOrigin, poseDestination);
        if(ret) {
          if(poseDestination) 
            *poseDestination = *poseOrigin;
          else {
            if(copyPose)
              poseDestination = poseOrigin->deepCopy();
            else
              poseDestination = poseOrigin;
          }
        }
        return ret;
      }
      
    virtual NDPose* deepCopy(void)
      {
        return new NDPose(*this);
      }
      
    static NDPose* createPose(NDPose::PoseType type);
      
    friend ostream &operator<<(ostream &stream, NDPose const& pose)
    {
      stream << pose.idString();
      stream << "(( " ;
      for(unsigned int i = 0; i < pose.position.size(); i++)
        stream << pose.position.at(i) << " ";
      stream << ") ";

      stream << "( " ;
      for(unsigned int i = 0; i < pose.orientation.size(); i++)
        stream << pose.orientation.at(i)*180/M_PI << " ";
      stream << "))";
      return stream; 
    }
     
    virtual PoseType getType(void) const { return NoPoseType ; }

    virtual SizesPose properSize(void) const {return SizesPose() ; }

    bool validSize(size_t poseSize) { return (poseSize == properSize().first + properSize().second) ; }
    bool validPositionSize(size_t positionSize) { return (positionSize == properSize().first) ; }
    bool validOrientationSize(size_t orientationSize) { return (orientationSize == properSize().second) ; }    
    
    virtual bool setCoordinates(const vector< double >& x)
      {
        bool ret = false;
        if(validSize(x.size())) {
          position.assign(x.begin(), x.begin() + properSize().first);
          orientation.assign(x.begin() + properSize().first, x.end());
          ret = true;
        }
        else
          cerr << "Warning: Invalid pose type, coordinates not assigned" << endl;

        return ret;
      }

    virtual bool setCoordinates(const vector< double >& position, const vector< double >& orientation)
      {
        return setPosition(position) && setOrientation(orientation);
      }

    virtual bool setCoordinates(double /*x*/, double /*y*/, double /*orientation*/)
    {
      cout << "NDPose::setCoordinates(d, d, d): not a NDPose2D" << endl;
      return false;
    }
      
    virtual bool setPosition(const vector< double >& position)
    {
      bool ret = false;
      if(validPositionSize(position.size())) {
        this->position = position;
        ret = true;
      }
      else
        cerr << "Warning: Invalid pose type, position not assigned" << endl;

      return ret;
    }
    vector< double > getPosition() const { return position ; }
    
    virtual bool setOrientation(const vector< double >& orientation)
    {
      bool ret = false;
      if(validOrientationSize(orientation.size())) {
        this->orientation = orientation;
        ret = true;
      }
      else
        cerr << "Warning: Invalid pose type, orientation not assigned" << endl;

      return ret;
    }
    vector< double > getOrientation() const { return orientation; }

    void toFile(QTextStream &stream)
    {
      for(unsigned int i = 0; i < position.size(); i++)
        stream << position.at(i) << " ";
      unsigned int i = 0;
      for(; i < orientation.size(); i++)
        stream << orientation.at(i)*180/M_PI << " ";
    }

    static bool canSafeCopy(const NDPose* poseOrigin, const NDPose* poseDestination)
      {
        bool ret = false;
        if(poseOrigin) {
          if(!poseDestination)
            ret = true;
          else
            ret = (poseDestination->getType() == NDPose::NoPoseType) || (poseDestination->getType() ==  poseOrigin->getType());
        }
        return ret;
      }

  protected:
    virtual string idString() const {return "" ; }

  protected:
    vector<double> position;
    vector<double> orientation;    
          
  private:
//     static SizesPose sizesPose;
//     static PoseType type;

};

// SizesPose NDPose::sizesPose(0,0);
// NDPose::PoseType NDPose::type = NoPoseType;

#endif
