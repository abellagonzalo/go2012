
#include "../Components/Body.h"


void Body::sideStrongToRightIn()
{
    AL::ALValue names, times, keys;

    #include "sideStrongToRightIn.txt"

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

void Body::sideStrongToLeftIn()
{

    AL::ALValue names, times, keys;

    #include "sideStrongToLeftIn.txt"

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
