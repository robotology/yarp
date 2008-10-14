#include <yarp/sig/Vector.h>
#include <yarp/math/Rand.h>
#include <stdio.h>

using namespace yarp::sig;
using namespace yarp::math;

int main(int argc, const char **)
{
    for(int k=0; k<10; k++)
    { 
        double n=Rand::scalar();
        fprintf(stderr, "%lf\n", n);
    }

    for(int k=0; k<10; k++)
    { 
        Vector v=Rand::vector(3);
        fprintf(stderr, "%s\n", v.toString().c_str());
    }


    return 0;
}  
