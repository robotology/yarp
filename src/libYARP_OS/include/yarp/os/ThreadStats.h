/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_THREAD_STATS_H
#define YARP_THREAD_STATS_H

#include <cassert>
#include <cmath>
#include <map>
#include <yarp/os/ConstString.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/Time.h>

/*
    Consider:
    * |---------| Period of the Thread or the Device Under Test
    * x           Time unit taken by the real computation
    * -           Time unit not taken by the real computation

    tick  tock
    v     v
    |xxxxxx---|xxxx-----|xxxxx----|xxxxxx---|
     <--------><--------><--------><-------->   dT
     <---->    <-->      <--->     <---->       elapsed
*/

namespace yarp {
namespace os {
class ThreadStats;
class MultipleThreadStats;
const unsigned WINDOW_LENGTH_DEFAULT = 60;
const unsigned BINS_PITCH_MS_DEFAULT = 5;
const unsigned MAX_JITTER_MS_DEFAULT = 100;
} // namespace os
} // namespace yarp

class YARP_OS_API yarp::os::ThreadStats {
public:
    typedef int ThreadId;

private:
    // All times are in seconds and must be positive and non-zero

    // Constant values
    // ===============
    double m_window_length;       // Length of the window for calculating statistic features
    double m_period;              // Ideal period
    unsigned m_cycles_per_window; // Number of cycles that fit in a selected window
    unsigned m_maxJitter_ms;      // Maximum jitter allowed

    // Instantaneous values
    // ====================
    double m_dT;        // Real period
    double m_tick_time; // Time acquired during the tick phase
    double m_tock_time; // Time acquired during the tock phase
    double m_elapsed;   // Effective computational time taken by the thread

    // Variables that keep data among cycles into the window_length
    // ============================================================
    unsigned m_cycle_count; // Counter of elapsed cycles
    double m_time_total;    // Time passed from the last reset (sum of dT until window_length)
    double m_elapsed_total; // Effective time passed from the last reset (sum of elapsed)
    double m_elapsed_mu;    // Mean of the elapsed time over cycles inside the window_length
    double m_elapsed_M2; // Variable used for the online calculation of the STD of the elapsed time
    double m_dT_mu;      // Mean of the real period
    double m_dT_M2;      // Variable used for the online calculation of the STD of the real period

    // Advanced statistics: histogram
    // ==============================
    unsigned m_bins_pitch_ms;
    typedef unsigned Bin;
    typedef unsigned BinOccurrence;
    std::map<Bin, BinOccurrence> m_jitter_pdf;

    // Other variables
    // ===============
    ThreadId m_id;
    bool m_advanced_statistics_enabled;
    bool m_reset_stats;
    yarp::os::Mutex m_mutex;

    // Private methods
    // ===============
    void _resetStat();
    void _insertJitter(double jitter);
    void _initializeJitterPDF();
    yarp::os::ConstString _messagePrefix() const;
    void _reportAdvancedStatistics();
    // Welford algorithm
    double _calculateM2(const double sample, double& mean, double& M2, const double n);

public:
    ThreadStats(const double& period          = 0,
                const double& window_length   = WINDOW_LENGTH_DEFAULT,
                const unsigned& bins_pitch_ms = BINS_PITCH_MS_DEFAULT,
                const ThreadId& id            = -1);
    ThreadStats(const ThreadStats& ts);
    ~ThreadStats() = default;

    // Overloaded operators
    // ====================
    ThreadStats& operator=(const ThreadStats& ts);

    // Public API
    // ==========

    /**
     * Marks the beginning of the period where the statistic is calculated
     * @see tock()
     */
    void tick();

    /**
     * Marks the end of the period where the statistic is calculated
     * @see tick()
     */
    void tock();

    /**
     * Cleans the interal status of the class
     */
    void resetStat();

    /**
     * If the period is set in the object constructor, it gives the possibility of sleeping
     * for the time `period - elapsed`
     */
    void sleepRemainingTime() const;

    /**
     * Enables the calculation of the advanced stastistic. For the time being it publishes
     * to yDebug() the histogram of the jitter in the configured `windows_length`
     */
    void enableAdvancedStatistics();

    /**
     * Disables the calculation of the advanced statistic
     *
     * @see enableAdvancedStatistics()
     */
    void disableAdvancedStatistics();

    // Set methods
    // ===========

    // Get methods
    // ===========

    /**
     * Returns the object id
     * @return The ThreadId value
     */
    ThreadId getThreadId() const;

    // Backward compatibility
    // ======================

    /**
     * Documented in RateThread::getEstPeriod()
     */
    double getEstPeriod();

    /**
     * Documented in RateThread::getEstPeriod(double&, double&)
     */
    void getEstPeriod(double& av, double& std);

    /**
     * Documented in RateThread::getIterations()
     */
    unsigned getIterations();

    /**
     * Documented in RateThread::getEstUsed()
     */
    double getEstUsed();

    /**
     * Documented in RateThread::getEstUsed(double&, double&)
     */
    void getEstUsed(double& av, double& std);
};
