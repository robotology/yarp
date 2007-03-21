#ifndef __YARP_SIG_RAND__
#define __YARP_SIG_RAND__

#include <yarp/sig/Vector.h>

const int NTAB=32;

namespace yarp
{
    class RandScalar;
    class RandnScalar;
    class RandVector;
    namespace sig
    {
        class Random;
    }
};

/**
* A random number generator, uniform in the range 0-1.
*/
class yarp::RandScalar
{
    int idum;
    int iy;
    int iv[NTAB];
    RandScalar(const RandScalar &l); 
public:
     RandScalar(int seed);
     RandScalar();

    /**
    * Initialize the random generator using
    * current time (time(0)).
    */
     void init();

    /**
    * Initialize the random generator.
    * @param seed the seed.
    */
     void init (int seed);

    /**
    * Get the seed.
    * @return the seed.
    */
     int getSeed ()
     { return idum; }

    /**
    * Generate a random number from a 
    * uniform distribution.
    * @return the random value.
    */
     double get();

    /**
    * Generate a random number from a 
    * uniform distribution within [min, max]
    * @return the random value.
    */
     double get(double min, double max);
};

/**
* A class to generate random vectors, uniform distribution.
*/
class yarp::RandVector
{
    yarp::sig::Vector data;
    yarp::RandScalar rnd;

private:
    RandVector(const RandVector &l);

public:
    RandVector(int s);

    void resize(int s);
    void init();
    void init(int seed);

    const yarp::sig::Vector &get();
    const yarp::sig::Vector &get(const yarp::sig::Vector &min, const yarp::sig::Vector &max);
};

/**
* A random number generator, normal distribution.
*/
class yarp::RandnScalar
{
private:
    inline void boxMuller();
    double y[2];
    int last;

    RandScalar rnd;
    RandnScalar(const RandnScalar &l);
public:
    /**
    * Constructor.
    */
    RandnScalar();
    
    /**
    * Constructor, initialize the generator.
    * @param seed, seed for the rnd generator.
    */
    RandnScalar(int seed);

    /**
    * Inizialize the generator.
    * Uses current time for the seed.
    */
    void init();

    /**
    * Inizialize the generator.
    * Provide a seed.
    * @param seed the seed
    */
    void init (int seed);

    long getSeed ()
    { return rnd.getSeed(); }

    /**
    * Generate a randomly generated number, drawn from
    * a normal distribution.
    * @return the random number.
    */
    double get();
};

/**
* A class to generate random vectors, normal distribution.
*/
class RandnVector
{
    yarp::sig::Vector data;
    yarp::RandnScalar rnd;
    RandnVector(const RandnVector &l){};

public:
    RandnVector(int s);

    void resize(int s);
    void init();
    void init(int seed);

    const yarp::sig::Vector &get();
};

/**
* A static class grouping function for random number 
* generator. Thread safe.
*/
class yarp::sig::Random
{
public:
    /**
    * Get a random number from a uniform distribution
    * in the range [0,1].
    */
    static double scalar();

    /**
    * Get a random number from a uniform distribution
    * in the range [min,max].
    * @param min lower bound
    * @param max upper bound
    * @return return value
    */
    static double scalar(double min, double max);

    /**
    * Get a vector of random numbers from a uniform distribution,
    * values are in the range [0, 1].
    * @param s the size of the vector
    * @return the random vector
    */
    static yarp::sig::Vector vector(int s);

    /**
    * Get a vector of random numbers from a uniform distribution,
    * values are in the range [min, max], for each components. The
    * function determines the size of the vector from the parameters.
    * @param min a vector whose components specify the lower bound of 
    * the distribution.
    * @param max a vector whose components specify the upper bound of
    * the distribution
    * @return the random vector
    */
    static yarp::sig::Vector vector(const yarp::sig::Vector &min,
                                    const yarp::sig::Vector &max);

    /**
    * Initialize the random number generator, with
    * current time (time(0)).
    */
    static void init();
    
    /**
    * Initialize the random number generator, provide
    * a seed.
    * @param seed: a seed.
    */
    static void init(int seed);
};

#endif
