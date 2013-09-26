#include "../Components/Body.h"
void Body::fisioBrazosTocarHombro()
{
    AL::ALValue names, times, keys;
    #include "fisioBrazosTocarHombro.txt"
    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
