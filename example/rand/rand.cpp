#include <yarp/sig/Vector.h>
#include <yarp/sig/Rand.h>
#include <stdio.h>

using namespace yarp;
using namespace yarp::sig;

int main(int argc, const char **)
{
    for(int k=0; k<10; k++)
    { 
        double n=Random::scalar();
        fprintf(stderr, "%lf\n", n);
    }

    for(int k=0; k<10; k++)
    { 
        Vector v=Random::vector(3);
        fprintf(stderr, "%s\n", v.toString().c_str());
    }


    return 0;
}  
