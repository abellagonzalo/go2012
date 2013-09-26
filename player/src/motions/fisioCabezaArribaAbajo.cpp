#include "../Components/Body.h"
void Body::fisioCabezaArribaAbajo()
{
    AL::ALValue names, times, keys;
    #include "fisioCabezaArribaAbajo.txt"
    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
