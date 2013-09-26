#include "../Components/Body.h"
void Body::fisioMunecaPalmaVuelta()
{
    AL::ALValue names, times, keys;
    #include "fisioMunecaPalmaVuelta.txt"
    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
