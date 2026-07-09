/*
 * SPDX-FileCopyrightText: Generative Bionics S.R.L.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_MEMORYGUARD_H
#define YARP_OS_MEMORYGUARD_H

#include <yarp/os/api.h>

#include <cstddef>
#include <optional>
#include <string>

namespace yarp::os {

/**
 * @brief RAII guard that locks process memory and prefaults stack/heap to
 * eliminate page-fault-induced latency spikes in the RT control loop.
 *
 * Intended for use once at startup, before entering the cyclic EtherCAT task.
 */
class YARP_os_API MemoryGuard
{
public:
    // Default stack reduced to a safe 512KB to prevent OS hard-limit crashes.
    // Default heap set to 128MB to pre-warm the glibc arena.
    explicit MemoryGuard(std::size_t stack_prefault_bytes = 512 * 1024,
                         std::size_t heap_reserve_bytes = 128 * 1024 * 1024);

    ~MemoryGuard();

    MemoryGuard(const MemoryGuard&) = delete;
    MemoryGuard& operator=(const MemoryGuard&) = delete;

    MemoryGuard(MemoryGuard&& other) noexcept;
    MemoryGuard& operator=(MemoryGuard&& other) noexcept;

    bool locked() const noexcept;
    bool heap_reserved() const noexcept;
    const std::optional<std::string>& last_error() const noexcept;

private:
    bool locked_ {false};
    bool heap_reserved_ {false}; // Replaced raw pointer with a boolean flag
    std::optional<std::string> last_error_;
#if defined(__linux__)
    int dma_latency_fd_ {-1};
#endif

    void set_error(const char* what, int err) noexcept;
    bool lock_process_memory() noexcept;
    void disable_malloc_page_faults() noexcept;
    void disable_transparent_huge_pages() noexcept;
    void request_zero_latency() noexcept;
    void prefault_stack(std::size_t bytes) noexcept;
    static std::size_t query_page_size() noexcept;
    bool reserve_heap(std::size_t bytes) noexcept;
    void unlock() noexcept;
};

} // namespace yarp::os

#endif // YARP_OS_MEMORYGUARD_H