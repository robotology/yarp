/**
 * C++ Mersenne Twister wrapper class written by
 * Jason R. Blevins <jrblevin@sdf.lonestar.org> on July 24, 2006.
 * Based on the original MT19937 C code by
 * Takuji Nishimura and Makoto Matsumoto.
 */

/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <iostream>
#include <cassert>

#include "yarp/impl/mt.h"

/**
 * Constructor
 */
MersenneTwister::MersenneTwister(void):
    mt_(new unsigned long[N]), mti_(N+1),
    init_key_(NULL), key_length_(0), s_(0),
    seeded_by_array_(false), seeded_by_int_(false)
{
    unsigned long init[4] = { 0x123, 0x234, 0x345, 0x456 };
    unsigned long length = 4;
    init_by_array(init, length);
}

/**
 * Destructor
 */
MersenneTwister::~MersenneTwister(void)
{
    assert(mt_ != NULL);
    delete[] mt_;
    mt_ = NULL;

    assert(init_key_ != NULL);
    delete[] init_key_;
    init_key_ = NULL;
}

/**
 * Initializes the Mersenne Twister with a seed.
 *
 * \param s seed
 */
void MersenneTwister::init_genrand(unsigned long s)
{
    mt_[0]= s & 0xffffffffUL;
    for (mti_=1; mti_<N; mti_++) {
        mt_[mti_] = 
	    (1812433253UL * (mt_[mti_-1] ^ (mt_[mti_-1] >> 30)) + mti_); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt_[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt_[mti_] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
    // Store the seed
    s_ = s;
    seeded_by_array_ = false;
    seeded_by_int_ = true;
}

/**
 * Seed the Mersenne Twister using an array.
 *
 * \param init_key an array for initializing keys
 * \param key_length the length of \a init_key
 */
void MersenneTwister::init_by_array(unsigned long* init_key, int key_length)
{
    // Store the key array
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--) {
        mt_[i] = (mt_[i] ^ ((mt_[i-1] ^ (mt_[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt_[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt_[0] = mt_[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt_[i] = (mt_[i] ^ ((mt_[i-1] ^ (mt_[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt_[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt_[0] = mt_[N-1]; i=1; }
    }

    mt_[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 

    // Store the seed
    if (init_key_ != NULL) {
        delete[] init_key_;
    }
    init_key_ = new unsigned long[key_length];
    for (int k = 0; k < key_length; k++) {
        init_key_[k] = init_key[k];
    }
    key_length_ = key_length;
    seeded_by_int_ = false;
    seeded_by_array_ = true;
}

/**
 * Generates a random number on [0,0xffffffff]-interval
 *
 * \return random number on [0, 0xffffffff]
 */
unsigned long MersenneTwister::genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti_ >= N) { /* generate N words at one time */
        int kk;

        if (mti_ == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt_[kk]&UPPER_MASK)|(mt_[kk+1]&LOWER_MASK);
            mt_[kk] = mt_[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt_[kk]&UPPER_MASK)|(mt_[kk+1]&LOWER_MASK);
            mt_[kk] = mt_[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt_[N-1]&UPPER_MASK)|(mt_[0]&LOWER_MASK);
        mt_[N-1] = mt_[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti_ = 0;
    }
  
    y = mt_[mti_++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/**
 * Generates a random integer on [0,0x7fffffff].
 *
 * \return a random integer on [0,0x7fffffff]
 */
long MersenneTwister::genrand_int31(void)
{
    return (long)(genrand_int32()>>1);
}

/**
 * Generates a random real number on [0,1].
 *
 * \return a random real number on [0,1]
 */
double MersenneTwister::genrand_real1(void)
{
    return genrand_int32()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/**
 * Generates a random real number on [0,1).
 *
 * \return a random real number on [0,1)
 */
double MersenneTwister::genrand_real2(void)
{
    return genrand_int32()*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/**
 * Generates a random real number on (0,1).
 *
 * \return a random real number on (0,1)
 */
double MersenneTwister::genrand_real3(void)
{
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/**
 * Generates a random real number on [0,1) with 53-bit precision.
 *
 * \return a random 53-bit real number on [0,1)
 */
double MersenneTwister::genrand_res53(void)
{ 
    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
/* These real versions are due to Isaku Wada, 2002/01/09 added */

/**
 * Print interesting information about the Mersenne Twister.
 *
 */
void MersenneTwister::print(void)
{
    std::cout << "MersenneTwister (M. Matsumoto and T. Nishimura), seed = ";
    if (seeded_by_int_) {
        std::cout << s_ << std::endl;
    } else {
        std::cout << "[ ";
        for (int k = 0; k < key_length_; k++) {
            std::cout << init_key_[k] << " ";
        }
        std::cout << "]" << std::endl;
    }
}
