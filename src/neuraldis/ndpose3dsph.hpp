#ifndef NDPOSE3DSPH_HPP
#define NDPOSE3DSPH_HPP

#include "ndpose.hpp"

/**
	@author Jose Manuel Cuadra Troncoso,,913987144,, <jose@portatil-jose>
*/
class NDPose3DSph : public NDPose
{
public:
    NDPose3DSph(NDPose3DSph const& pose) : NDPose(pose) {};

    NDPose3DSph() : NDPose(properSize()) {};

    NDPose3DSph(vector< double > x)
      {
        NDPose3DSph();
        setCoordinates(x);
      };

    NDPose3DSph(vector<double> position, vector<double> orientation)
      {
        NDPose3DSph();
        setCoordinates(position, orientation);
      };

    ~NDPose3DSph(){};

    SizesPose properSize(void) const { return SizesPose(3, 3) ; }

    void operator=(NDPose3DSph const& pose)
      {
        position = pose.position;
        orientation = pose.orientation;
      }

    NDPose* deepCopy(void)
      {
        return new NDPose3DSph(*this);
      }
        
    PoseType getType(void) const { return Pose3DSph ; };
        
  protected:
    string idString() const { return "Pose3DSph" ; }
       
  private:      
//     static SizesPose sizesPose;
//     static PoseType type;
};

// SizesPose NDPose3DSph::sizesPose(3, 3);
// NDPose::PoseType NDPose3DSph::type = Pose3DSph;

#endif
