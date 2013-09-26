#ifndef POSE_H_
#define POSE_H_

#include "Vector2D.h"

#define POSE_NUM_RANDOM_SAMPLES 10

typedef struct
{
	float x, y, theta;
} PoseGuess;

class Pose {
public:
  Pose(const Vector2D & _position = Vector2D(0, 0),
       const float _angle = 0,
       const float _confidence = 0,
       const bool _lost = true);

  /**
   * Copy constructor.
   */
  Pose(const Pose & other);

  virtual ~Pose();

  const Vector2D getPosition() const;
  void setPosition(const Vector2D & _position);
  void setPosition(const float x, const float y);

  float getAngle() const;
  void setAngle(const float _angle);

  float getConfidence() const;
  void setConfidence(const float _confidence);

  bool isLost() const;
  void setLost(const bool _lost);

  Vector2D convertRelativeToGlobal(const Vector2D & relativeCoords) const;
  float convertRelativeAngleToGlobal(const float relativeAngle) const;

  Vector2D convertGlobalToRelative(const Vector2D & globalCoords) const;
  float convertGlobalAngleToRelative(const float globalAngle) const;

  void setGuess(int num, float x, float y, float theta);
  void getGuess(int num, float* x, float* y, float* theta) const;

  /**
   * Allow assignment.
   */
  Pose & operator=(const Pose & other);

private:
  Vector2D position;
  float    angle;
  float    confidence;
  bool     lost;
  PoseGuess guesses[POSE_NUM_RANDOM_SAMPLES];
};

#endif

