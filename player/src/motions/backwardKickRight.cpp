#include "../Components/Body.h"
void Body::backwardKickRight()
{
    AL::ALValue names, times, keys;
    #include "backwardKickRight.txt"
    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
