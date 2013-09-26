#include "../Components/Body.h"
void Body::fisioBrazosAlternosArribaAbajo()
{
    AL::ALValue names, times, keys;
    #include "fisioBrazosAlternosArribaAbajo.txt"
    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
