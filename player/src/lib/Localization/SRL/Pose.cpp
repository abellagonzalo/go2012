#include "Pose.h"
#include "num_util.h"

// Define the logging constants
#define COMPONENT LOCALIZATION
//#define CLASS_LOG_LEVEL LOG_LEVEL_INFO

Pose::Pose(const Vector2D & _position,
           const float _angle,
           const float _confidence,
           const bool _lost){
  position = _position;
  angle = _angle;
  confidence = _confidence;
  lost = _lost;
}

Pose::Pose(const Pose & other){
  position = other.position;
  angle = other.angle;
  confidence = other.confidence;
  lost = other.lost;
}

Pose::~Pose() {
}

const Vector2D Pose::getPosition() const{
  return position;
}

void Pose::setPosition(const Vector2D & _position){
  position = _position;
}

void Pose::setPosition(const float x, const float y) {
  position.x = x;
  position.y = y;
}

float Pose:: getAngle() const{
  return angle;
}

void Pose::setAngle(const float _angle){
  angle = _angle;
}

float Pose::getConfidence() const{
  return confidence;
}

void Pose::setConfidence(const float _confidence){
  confidence = _confidence;
}

bool Pose::isLost() const{
  return lost;
}

void Pose::setLost(const bool _lost){
  lost = _lost;
}

Vector2D Pose::convertRelativeToGlobal(const Vector2D & relativeCoords) const {
  float s = sin(angle);
  float c = cos(angle);
  float x = relativeCoords.x;
  float y = relativeCoords.y;

  Vector2D globalCoords = position;
  globalCoords.x += c * x - s * y;
  globalCoords.y += s * x + c * y;

  return globalCoords;
}

Vector2D Pose::convertGlobalToRelative(const Vector2D & globalCoords) const {
  float s = sin(-angle);
  float c = cos(-angle);
  float x = globalCoords.x - position.x;
  float y = globalCoords.y - position.y;

  return Vector2D(c * x - s * y, s * x + c * y);
}

float Pose::convertRelativeAngleToGlobal(float relativeAngle) const {
  return norm_angle(angle + relativeAngle);
}

float Pose::convertGlobalAngleToRelative(float globalAngle) const {
  return norm_angle(globalAngle - angle);
}

Pose & Pose::operator=(const Pose & other) {
  position = other.position;
  angle = other.angle;
  confidence = other.confidence;
  lost = other.lost;

  return *this;
}

void Pose::setGuess(int num, float x, float y, float theta)
{
  if (num < 0 || num >= POSE_NUM_RANDOM_SAMPLES)
	  return;
  guesses[num].x = x;
  guesses[num].y = y;
  guesses[num].theta = theta;
}

void Pose::getGuess(int num, float* x, float* y, float* theta) const
{
  if (num < 0 || num >= POSE_NUM_RANDOM_SAMPLES)
	  return;
  *x = guesses[num].x;
  *y = guesses[num].y;
  *theta = guesses[num].theta;
}

