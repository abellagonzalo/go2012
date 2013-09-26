#include "../Components/Body.h"


void Body::sideMiniToRightIn()
{
    AL::ALValue names, times, keys;

    #include "sideMiniToRightIn.txt"

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

void Body::sideMiniToLeftIn()
{

    AL::ALValue names, times, keys;

    #include "sideMiniToLeftIn.txt"

    try
    {
//      std::cout << "WEAK LEFT" << std::endl;
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
