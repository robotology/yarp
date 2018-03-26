/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/LockGuard.h>
#include <yarp/os/ThreadStats.h>

using namespace yarp::os;

// ===========
// ThreadStats
// ===========

ThreadStats::ThreadStats(const double& period,
                         const double& window_length,
                         const unsigned& bins_pitch_ms,
                         const ThreadId& id)
    : m_window_length(window_length)
    , m_period(period)
    , m_cycles_per_window(static_cast<unsigned>(m_window_length / m_period))
    , m_maxJitter_ms(MAX_JITTER_MS_DEFAULT)
    , m_tick_time(0)
    , m_tock_time(0)
    , m_bins_pitch_ms(bins_pitch_ms)
    , m_id(id)
    , m_advanced_statistics_enabled(false)
    , m_reset_stats(false)
{
    assert(yarp::os::Time::isValid());
    if (not Time::isValid()) {
        yFatal() << _messagePrefix() << "The clock is not valid!";
    }
    _resetStat();
}

ThreadStats::ThreadStats(const ThreadStats& ts)
    : m_window_length(ts.m_window_length)
    , m_period(ts.m_period)
    , m_cycles_per_window(static_cast<unsigned>(ts.m_window_length / ts.m_period))
    , m_maxJitter_ms(MAX_JITTER_MS_DEFAULT)
    , m_tick_time(0)
    , m_tock_time(0)
    , m_bins_pitch_ms(ts.m_bins_pitch_ms)
    , m_id(ts.m_id)
    , m_advanced_statistics_enabled(false)
    , m_reset_stats(false)
{
    assert(yarp::os::Time::isValid());
    if (not Time::isValid()) {
        yFatal() << _messagePrefix() << "The clock is not valid!";
    }
    _resetStat();
}

// Overloaded operators
// ====================

ThreadStats& ThreadStats::operator=(const ThreadStats& ts)
{
    if (&ts == this) {
        return *this;
    }

    this->m_window_length     = ts.m_window_length;
    this->m_period            = ts.m_period;
    this->m_cycles_per_window = static_cast<unsigned>(this->m_window_length / this->m_period);
    this->m_tick_time         = 0;
    this->m_tock_time         = 0;
    this->m_bins_pitch_ms     = ts.m_bins_pitch_ms;
    this->m_id                = ts.m_id;
    this->m_advanced_statistics_enabled = false;
    this->m_reset_stats                 = 0;
    return *this;
}

// Private methods
// ===============

void ThreadStats::_resetStat()
{
    m_cycle_count   = 0;
    m_time_total    = 0;
    m_elapsed_total = 0;
    m_elapsed_mu    = 0;
    m_elapsed_M2    = 0;
    m_dT_mu         = 0;
    m_dT_M2         = 0;
    _initializeJitterPDF();
    m_reset_stats = false;
}

double ThreadStats::_calculateM2(const double sample, double& mean, double& M2, const double n)
{
    double delta = sample - mean;
    mean += delta / n;
    double delta2 = sample - mean;
    return M2 += delta * delta2;
}

void ThreadStats::_initializeJitterPDF()
{
    for (unsigned i = 0; i < m_maxJitter_ms; i += m_bins_pitch_ms) {
        m_jitter_pdf[i] = 0;
    }
}

void ThreadStats::_insertJitter(double jitter)
{
    unsigned jitter_ms     = static_cast<unsigned>(jitter * 1000);
    unsigned quantized_bin = jitter_ms - (jitter_ms % m_bins_pitch_ms);
    // Get the current value. If no key is defined, it gets initialized to 0
    // This workaroud allows avoiding to use exceptions
    unsigned value              = m_jitter_pdf[quantized_bin];
    m_jitter_pdf[quantized_bin] = value + 1;
}

ConstString ThreadStats::_messagePrefix() const
{
    ConstString s("yarp::os::ThreadStats");
    if (m_id != -1) {
        s += " (id=" + std::to_string(m_id) + ")";
    }
    s += ": ";
    return s;
}

void ThreadStats::_reportAdvancedStatistics()
{
    {
        // Report the pdf of cycle time
        LockGuard lock(m_mutex);
        unsigned samples_num = 0;
        for (const auto& bin : m_jitter_pdf) {
            samples_num += bin.second;
        }
        if (samples_num != 0) {
            ConstString string_pdf;
            for (const auto& bin : m_jitter_pdf) {
                if (bin.first == 0) {
                    string_pdf += "p(d<" + std::to_string(m_bins_pitch_ms) + ")=";
                    string_pdf += std::to_string(static_cast<float>(bin.second)
                                                 / static_cast<float>(samples_num));
                }
                else if (bin.first < m_maxJitter_ms) {
                    string_pdf += " p(" + std::to_string(bin.first) + "<=d<";
                    string_pdf += std::to_string(bin.first + m_bins_pitch_ms) + ")=";
                    string_pdf += std::to_string(static_cast<float>(bin.second)
                                                 / static_cast<float>(samples_num));
                }
                else {
                    string_pdf += " p(d>=" + std::to_string(bin.first) + ")=";
                    string_pdf += std::to_string(static_cast<float>(bin.second)
                                                 / static_cast<float>(samples_num));
                }
            }
            yDebug() << _messagePrefix() << string_pdf;
        }
        else {
            yWarning() << _messagePrefix()
                       << "The statistic has been asked but no data is available";
        }
    }

    // Report the estimated period
    double muP = 0, avP = 0;
    getEstPeriod(muP, avP);
    yDebug() << _messagePrefix() << "Estimated period: " << muP << "+-" << avP;

    // Report the estimated used time
    double muU = 0, avU = 0;
    getEstUsed(muU, avU);
    yDebug() << _messagePrefix() << "Estimated used time: " << muU << "+-" << avU;
}

// Public API
// ==========

void ThreadStats::tick()
{
    double tick_time_old;
    {
        LockGuard lock(m_mutex);
        tick_time_old = m_tick_time;
        m_tick_time   = Time::now();
    }

    if ((m_cycle_count >= m_cycles_per_window) || m_reset_stats) {
        if (m_advanced_statistics_enabled) {
            _reportAdvancedStatistics();
        }
        _resetStat();
    }

    {
        LockGuard lock(m_mutex);
        m_cycle_count++;

        // Skip first period for dT calculation
        if (m_cycle_count > 1) {
            m_dT = m_tick_time - tick_time_old;
            _insertJitter(m_dT); // TODO change name? is is not a Jitter
            _calculateM2(m_dT, m_dT_mu, m_dT_M2, m_cycle_count - 1);
            m_time_total += m_dT;
        }
    }
}

void ThreadStats::tock()
{
    LockGuard lock(m_mutex);
    m_tock_time = Time::now();

    m_elapsed = m_tock_time - m_tick_time;
    _calculateM2(m_elapsed, m_elapsed_mu, m_elapsed_M2, m_cycle_count);
    m_elapsed_total += m_elapsed;
}

void ThreadStats::sleepRemainingTime() const
{
    assert(m_period != 0.0);
    assert((m_period - m_elapsed) >= 0);
    if ((m_period - m_elapsed) < 0) {
        yError() << _messagePrefix() << "Computational time was greater than the thread's period!";
        return;
    }
    Time::delay(m_period - m_elapsed);
}

void ThreadStats::resetStat()
{
    LockGuard lock(m_mutex);
    m_reset_stats = true;
}

void ThreadStats::enableAdvancedStatistics()
{
    LockGuard lock(m_mutex);
    m_advanced_statistics_enabled = true;
    _initializeJitterPDF();
}

void ThreadStats::disableAdvancedStatistics()
{
    LockGuard lock(m_mutex);
    m_advanced_statistics_enabled = false;
}

// Get methods
// ===========

int ThreadStats::getThreadId() const
{
    return m_id;
}

// Backward compatibility
// ======================

double ThreadStats::getEstPeriod()
{
    double ret;
    LockGuard lock(m_mutex);

    if (m_cycle_count <= 1) {
        ret = 0;
    }
    else {
        ret = m_time_total / (m_cycle_count - 1);
    }
    return ret;
}

void ThreadStats::getEstPeriod(double& av, double& std)
{
    LockGuard lock(m_mutex);
    if (m_cycle_count <= 1) {
        av  = 0;
        std = 0;
    }
    else {
        unsigned samples_number = m_cycle_count - 1;
        av                      = m_time_total / samples_number;
        std                     = sqrt(1.0 / (samples_number - 1) * m_dT_M2);
    }
}

unsigned ThreadStats::getIterations()
{
    LockGuard lock(m_mutex);
    unsigned ret = m_cycle_count;
    return ret;
}

double ThreadStats::getEstUsed()
{
    LockGuard lock(m_mutex);
    double ret;

    if (m_cycle_count < 1) {
        ret = 0;
    }
    else {
        ret = m_elapsed_total / m_cycle_count;
    }

    return ret;
}

void ThreadStats::getEstUsed(double& av, double& std)
{
    LockGuard lock(m_mutex);

    if (m_cycle_count < 1) {
        av  = 0;
        std = 0;
    }
    else {
        av  = m_elapsed_total / m_cycle_count;
        std = sqrt(1.0 / (m_cycle_count - 1) * m_elapsed_M2);
    }
}

// ===================
// MultipleThreadStats
// ===================

MultipleThreadStats::MultipleThreadStats(const double& period,
                                         const double& window_length,
                                         const unsigned& bins_pitch_ms)
    : m_period(period), m_window_length(window_length), m_bins_pitch_ms(bins_pitch_ms)
{
    assert(m_period != 0.0);
}

// Private methods
// ===============

ConstString MultipleThreadStats::_messagePrefix() const
{
    return ConstString("yarp::os::MultipleThreadStats: ");
}

// Public API
// ==========

bool MultipleThreadStats::insertThreadStats(const yarp::os::ThreadStats& ts)
{
    bool ret;
    ThreadStats::ThreadId id = ts.getThreadId();
    if (m_threads_stats.find(id) == m_threads_stats.end()) {
        ret = true;
    }
    else {
        yWarning() << _messagePrefix() << "ThreadStats with id=" << id << " has been overwritten";
        ret = false;
    }
    m_threads_stats[id] = ts;
    return ret;
}

bool MultipleThreadStats::newThreadStats(const ThreadStats::ThreadId& id,
                                         const double& period,
                                         const double& window_length,
                                         const unsigned& bins_pitch_ms)
{
    bool ret = false;

    // Use values passed in the constructor if not given
    if (period == 0.0) {
        return ret = insertThreadStats({m_period, m_window_length, m_bins_pitch_ms, id});
    }
    else if (window_length == 0.0) {
        return ret = insertThreadStats({period, m_window_length, m_bins_pitch_ms, id});
    }
    else if (bins_pitch_ms == 0) {
        return ret = insertThreadStats({period, window_length, m_bins_pitch_ms, id});
    }
    else {
        return ret = insertThreadStats({period, window_length, bins_pitch_ms, id});
    }
}

bool MultipleThreadStats::removeThreadStats(const ThreadStats::ThreadId& id)
{
    if (m_threads_stats.erase(id) == 0) {
        yWarning() << _messagePrefix() << "Tried to remove unexisting thread with id=" << id;
        return false;
    }
    else {
        return true;
    }
}

// ThreadStats transparent usage
// =============================

void MultipleThreadStats::tick(const ThreadStats::ThreadId& id)
{
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        m_threads_stats[id].tick();
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
}

void MultipleThreadStats::tock(const ThreadStats::ThreadId& id)
{
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        m_threads_stats[id].tock();
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
}

void MultipleThreadStats::resetStat(const ThreadStats::ThreadId& id)
{
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        m_threads_stats[id].resetStat();
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
}

void MultipleThreadStats::sleepRemainingTime(const ThreadStats::ThreadId& id) const
{
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        m_threads_stats.at(id).sleepRemainingTime();
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
}
void MultipleThreadStats::enableAdvancedStatistics(const ThreadStats::ThreadId& id)
{
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        m_threads_stats[id].enableAdvancedStatistics();
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
}
void MultipleThreadStats::disableAdvancedStatistics(const ThreadStats::ThreadId& id)
{
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        m_threads_stats[id].disableAdvancedStatistics();
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
}

double MultipleThreadStats::getEstPeriod(const ThreadStats::ThreadId& id)
{
    double estimated_period = 0;
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        estimated_period = m_threads_stats[id].getEstPeriod();
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
    return estimated_period;
}

void MultipleThreadStats::getEstPeriod(double& av, double& std, const ThreadStats::ThreadId& id)
{
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        m_threads_stats[id].getEstPeriod(av, std);
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
}
unsigned MultipleThreadStats::getIterations(const ThreadStats::ThreadId& id)
{
    unsigned iterations = 0;
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        iterations = m_threads_stats[id].getIterations();
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
    return iterations;
}

double MultipleThreadStats::getEstUsed(const ThreadStats::ThreadId& id)
{
    double estimated_used = 0;
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        estimated_used = m_threads_stats[id].getEstUsed();
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
    return estimated_used;
}

void MultipleThreadStats::getEstUsed(double& av, double& std, const ThreadStats::ThreadId& id)
{
    if (m_threads_stats.find(id) != m_threads_stats.end()) {
        m_threads_stats[id].getEstUsed(av, std);
    }
    else {
        yWarning() << _messagePrefix() << "Thread with id=" << id << " not found";
    }
}
