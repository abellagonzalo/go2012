#include "../Components/Body.h"


void Body::suicide()
{
    AL::ALValue names, times, keys;

    #include "suicide.txt"

    try
    {
//      std::cout << "WEAK RIGHT" << std::endl;
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
