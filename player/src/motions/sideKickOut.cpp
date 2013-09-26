#include "../Components/Body.h"

void Body::sideKickToRightOut()
{

  AL::ALValue names, times, keys;

  #include "sideKickToRightOut.txt"

  try
  {
  	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
 }
  catch(ALError& e)
  {
      std::cout << "ERROR!!!!" << std::endl;
  }
}

void Body::sideKickToLeftOut()
{

  AL::ALValue names, times, keys;

  #include "sideKickToLeftOut.txt"

  try
  {
  	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
  }
  catch(ALError& e)
  {
      std::cout << "ERROR!!!!" << std::endl;
  }
}
