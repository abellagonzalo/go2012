#include "../Components/Body.h"

void Body::forwardStrongRight()
{
  AL::ALValue names, times, keys;

  #include "forwardStrongRight.txt"

  try
  {
	  movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
  }
  catch(ALError& e)
  {
      std::cout << "ERROR!!!!" << std::endl;
  }
}

void Body::forwardStrongLeft()
{

  AL::ALValue names, times, keys;

  #include "forwardStrongLeft.txt"

  try
  {
	  movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
  }
  catch(ALError& e)
  {
      std::cout << "ERROR!!!!" << std::endl;
  }
}
