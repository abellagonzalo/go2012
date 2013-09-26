#include "../Components/Body.h"


void Body::forwardDribbleRight()
{
    AL::ALValue names, times, keys;

    #include "forwardDribbleRight.txt"

    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}

void Body::forwardDribbleLeft()
{

    AL::ALValue names, times, keys;

    #include "forwardDribbleLeft.txt"

    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
