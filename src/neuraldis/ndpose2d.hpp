#ifndef NDPOSE2D_HPP
#define NDPOSE2D_HPP

#include "ndpose.hpp"

/**
  @author Jose Manuel Cuadra Troncoso <jose@portatil-jose>
*/
class NDPose2D : public NDPose
{
  public:
//     NDPose2D(NDPose const& pose);
    NDPose2D(NDPose2D const& pose) : NDPose(pose) {};

    NDPose2D() : NDPose(properSize()) {};
    
    NDPose2D(vector< double > x)
      {
        NDPose2D();
        NDPose::setCoordinates(x);
      };

    NDPose2D(vector<double> position, vector<double> orientation)
      {
        NDPose2D();
        NDPose::setCoordinates(position, orientation);
      };

    ~NDPose2D() {};

    SizesPose properSize(void) const { return SizesPose(2, 1) ; }

    void operator=(NDPose2D const& pose)
      {
        position = pose.position;
        orientation = pose.orientation;
      }

    NDPose* deepCopy(void)
      {
        return new NDPose2D(*this);
      }

    PoseType getType(void) const { return Pose2D ; };

    bool setCoordinates(double x, double y, double orientation)
      {
        position[0] = x;
        position[1] = y;
        while(orientation > 2*M_PI) orientation -= 2*M_PI;
        while(orientation < -2*M_PI) orientation += 2*M_PI;
        int m = (int)(orientation / (2.0 * M_PI));
        orientation = orientation - (double)m * M_PI;
        if (orientation < (-M_PI))
          orientation += 2.0 * M_PI;
        if (orientation >= M_PI)
          orientation -= 2.0 * M_PI;
//        while(orientation > 360.0) orientation -= 360.0;
//        while(orientation < -360.0) orientation += 360.0;
        this->orientation[0] = orientation;
        return true;
      }    

  protected:      
    string idString() const { return "Pose2D" ; }
    
  private:      
//     static SizesPose sizesPose;
//     static PoseType type;
};

// SizesPose NDPose2D::sizesPose(2, 1);
// NDPose::PoseType NDPose2D::type = Pose2D;

#endif
