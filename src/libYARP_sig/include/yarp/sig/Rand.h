#ifndef __YARP_SIG_RAND__
#define __YARP_SIG_RAND__

const int NTAB=32;

namespace yarp
{
    class RandScalar;
    class RandnScalar;
    class Random;
};

/**
* A random number generator, uniform in the range 0-1.
*/
class yarp::RandScalar
{
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

private:
    int idum;
    int iy;
    int iv[NTAB];
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
* A static class grouping function for random number 
* generator. Thread safe,
*/
class yarp::Random
{
public:
    /**
    * Get a random number from a uniform distribution
    * in the range [0,1].
    */
    static double rand();
    
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
