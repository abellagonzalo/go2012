#include "../Components/Body.h"

void Body::forwardKickRight()
{

  ALValue names, times, keys;

  #include "forwardKickRight.txt"

  try
  {
	  movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
  }
  catch(ALError& e)
  {
      std::cout << "ERROR!!!!" << std::endl;
  }
}

void Body::forwardKickLeft() {

  ALValue names, times, keys;

  #include "forwardKickLeft.txt"

  try
  {
	  movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
  }
  catch(ALError& e)
  {
      std::cout << "ERROR!!!!" << std::endl;
  }
}
