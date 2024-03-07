#ifndef NDPOSE3DQUAT_HPP
#define NDPOSE3DQUAT_HPP

#include "ndpose.hpp"

/**
  @author Jose Manuel Cuadra Troncoso,,913987144,, <jose@portatil-jose>
*/
class NDPose3DQuat : public NDPose
{
public:
    NDPose3DQuat(NDPose3DQuat const& pose) : NDPose(pose) {};
    
    NDPose3DQuat() : NDPose(properSize()) {};

    NDPose3DQuat(vector< double > x)
      {
        NDPose3DQuat();
        setCoordinates(x);
      };

    NDPose3DQuat(vector<double> position, vector<double> orientation)
      {
        NDPose3DQuat();
        setCoordinates(position, orientation);
      };
    
    ~NDPose3DQuat(){};

    SizesPose properSize(void) const { return SizesPose(3, 4) ; }

    void operator=(NDPose3DQuat const& pose)
      {
        position = pose.position;
        orientation = pose.orientation;
      }
        
    NDPose* deepCopy(void)
      {
        return new NDPose3DQuat(*this);
      }

    PoseType getType(void) const { return Pose3DQuat ; };

  protected:
    string idString() const { return "Pose3DQuat" ; }
           
  private:      
//     static SizesPose sizesPose;
//     static PoseType type;
};

// SizesPose NDPose3DQuat::sizesPose(3, 4);
// NDPose::PoseType NDPose3DQuat::type = Pose3DQuat;

#endif
