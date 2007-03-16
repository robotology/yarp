
#include <yarp/sig/Rand.h>
#include <stdio.h>

int main(int argc, const char **)
{
    for(int k=0; k<1000; k++)
    { 
        double n=yarp::Random::rand();
        fprintf(stderr, "%lf\n", n);
    }

    return 0;
}  
