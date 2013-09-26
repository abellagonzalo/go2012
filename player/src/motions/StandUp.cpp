#include "../Components/Body.h"


void Body::StandUpBack()
{
    AL::ALValue times, keys;
    std::vector<std::string> names;

    #include "StandUpBack.txt"

    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }

   
}

void Body::StandUpFront()
{

    AL::ALValue times, keys;
    std::vector<std::string> names;

    #include "StandUpFront.txt"

    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
