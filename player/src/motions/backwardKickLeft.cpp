#include "../Components/Body.h"
void Body::backwardKickLeft()
{
    AL::ALValue names, times, keys;
    #include "backwardKickLeft.txt"
    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
