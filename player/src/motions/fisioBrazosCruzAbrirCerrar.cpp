#include "../Components/Body.h"
void Body::fisioBrazosCruzAbrirCerrar()
{
    AL::ALValue names, times, keys;
    #include "fisioBrazosCruzAbrirCerrar.txt"
    try
    {
    	movet.push_back(pmotion->post.angleInterpolationBezier(names, times, keys));
    }
    catch(ALError& e)
    {
        std::cout << "ERROR!!!!" << std::endl;
    }
}
