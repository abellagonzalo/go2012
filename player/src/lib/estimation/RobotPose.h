/**
 * @file RobotPose.h
 *
 * The file contains the definition of the class RobotPose.
 *
 * @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas R�fer</A>
 */

#ifndef __RobotPose_h_
#define __RobotPose_h_

#include "../Math/Pose2D.h"

/**
* @class RobotPose
* A Pose2D with validity.
*/
class RobotPose : public Pose2D
{

public:
  double validity; /**< The validity of the robot pose. */
 
  /** Constructor */
  RobotPose():validity(0.0) {}

  /** Assignment operator
  * @param other Another RobotPose
  * @return A reference to the object after the assignment
  */
  const RobotPose& operator=(const RobotPose& other)
  {
    (Pose2D&) *this = (const Pose2D&) other;
    validity = other.validity;
    return *this;
  }

  /** Cast Contructor.
  * @param otherPose A Pose2D object
  */
  RobotPose (const Pose2D& otherPose) 
  {
    (Pose2D&) *this = otherPose;
    validity = 0;
  }

  /** Assignment operator for Pose2D objects
  * @param other A Pose2D object
  * @return A reference to the object after the assignment
  */
  const RobotPose& operator=(const Pose2D& other)
  {
    (Pose2D&) *this = other;
    //validity is not set
    return *this;
  }
};

#endif //__RobotPose_h_
