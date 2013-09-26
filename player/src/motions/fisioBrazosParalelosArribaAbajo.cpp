#include "../Components/Body.h"
void Body::fisioBrazosParalelosArribaAbajo()
{
    AL::ALValue names, times, keys;
    #include "fisioBrazosParalelosArribaAbajo.txt"
    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
