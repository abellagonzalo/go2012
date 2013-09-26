#include "../Components/Body.h"

void Body::obliqueKickToRight()
{
  AL::ALValue names, times, keys;

  #include "obliqueKickToRight.txt"

  try
  {
	  movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
  }
  catch(ALError& e)
  {
      std::cout << "ERROR!!!!" << std::endl;
  }
}

void Body::obliqueKickToLeft()
{

  AL::ALValue names, times, keys;

  #include "obliqueKickToLeft.txt"

  try
  {
	  movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
  }
  catch(ALError& e)
  {
      std::cout << "ERROR!!!!" << std::endl;
  }
}
