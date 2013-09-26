#include "../Components/Body.h"
void Body::fisioCabezaIzquierdaDerecha()
{
    AL::ALValue names, times, keys;
    #include "fisioCabezaIzquierdaDerecha.txt"
    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
