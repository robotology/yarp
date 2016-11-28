/**
 * mt.h: Mersenne Twister header file
 *
 * Jason R. Blevins <jrblevin@sdf.lonestar.org>
 * Durham, March  7, 2007
 */

#ifndef METRICS_MT_H
#define METRICS_MT_H

/**
 * Mersenne Twister.
 *
 * M. Matsumoto and T. Nishimura, "Mersenne Twister: A
 * 623-dimensionally equidistributed uniform pseudorandom number
 * generator", ACM Trans. on Modeling and Computer Simulation Vol. 8,
 * No. 1, January pp.3-30 (1998).
 *
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html.
 */
class MersenneTwister
{
public:
    MersenneTwister(void);
    ~MersenneTwister(void);

    double random(void) { return genrand_real1(); }
    void print(void);

    void init_genrand(unsigned long s);
    void init_by_array(unsigned long* init_key, int key_length);

    unsigned long genrand_int32(void);
    long genrand_int31(void);
    double genrand_real1(void);
    double genrand_real2(void);
    double genrand_real3(void);
    double genrand_res53(void);

private:
    static const int N                    = 624;
    static const int M                    = 397;
    // constant vector a
    static const unsigned long MATRIX_A   = 0x9908b0dfUL;
    // most significant w-r bits
    static const unsigned long UPPER_MASK = 0x80000000UL;
    // least significant r bits
    static const unsigned long LOWER_MASK = 0x7fffffffUL;

    unsigned long* mt_;                  // the state vector
    int mti_;                            // mti == N+1 means mt not initialized

    unsigned long* init_key_;            // Storage for the seed vector
    int key_length_;                     // Seed vector length
    unsigned long s_;                    // Seed integer
    bool seeded_by_array_;               // Seeded by an array
    bool seeded_by_int_;                 // Seeded by an integer
};

#endif // METRICS_MT_H
