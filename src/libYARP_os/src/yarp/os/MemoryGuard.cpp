/*
 * SPDX-FileCopyrightText: Generative Bionics S.R.L.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/MemoryGuard.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>

#if defined(__linux__)
#    include <errno.h>
#    include <fcntl.h>
#    include <malloc.h>
#    include <sys/mman.h>
#    include <sys/prctl.h>
#    include <unistd.h>
#elif defined(__APPLE__)
#    include <errno.h>
#    include <sys/mman.h>
#    include <unistd.h>
#elif defined(_WIN32)
#    include <malloc.h> // for _alloca
#    include <windows.h>
#endif

using yarp::os::MemoryGuard;

namespace {

#if !defined(_WIN32)
// Touch stack memory by recursing through real stack frames instead of growing a single
// frame with alloca(). A single large alloca() is undone as soon as the function returns
// (so it does not actually keep any pages resident for later use) and, for larger sizes or
// threads with a smaller stack, it can blow straight through the stack guard page with no
// safety margin. Recursing through small, fixed-size frames touches the same number of pages
// while never growing any single frame by more than a page.
constexpr std::size_t kStackTouchFrameSize = 4096;

void touch_stack_frames(std::size_t remaining_bytes) noexcept
{
    volatile std::uint8_t frame[kStackTouchFrameSize];
    std::memset(const_cast<std::uint8_t*>(frame), 0, sizeof(frame));
    if (remaining_bytes > kStackTouchFrameSize) {
        touch_stack_frames(remaining_bytes - kStackTouchFrameSize);
    }
}
#endif

} // namespace

MemoryGuard::MemoryGuard(const std::size_t stack_prefault_bytes,
                         const std::size_t heap_reserve_bytes)
{
    // Touch the stack/heap memory we want resident *before* calling lock_process_memory()
    // below, so that this already-touched footprint is what actually gets locked: MCL_CURRENT
    // only locks pages that are already mapped/resident at the moment it is called, so
    // anything touched *after* the mlockall() call would not be locked at all.
    //
    // We deliberately never use MCL_FUTURE (nor MCL_CURRENT | MCL_FUTURE). mlockall() flags
    // are process-wide, not per-device/per-thread: MCL_FUTURE requires every mapping created
    // anywhere in the process afterwards - including the stack of every thread later spawned
    // by other devices/ports that yarprobotinterface still has to open (control-board
    // wrappers, YARP ports, ...) - to also be lockable. Once enough of those threads pile up,
    // pthread_create() starts failing with EAGAIN.
    //
    // This is not theoretical: it was reproduced on a real robot config and confirmed with a
    // gdb backtrace. With MCL_CURRENT | MCL_FUTURE, yarprobotinterface reliably aborted with
    // "terminate called after throwing an instance of 'std::system_error' / what(): Resource
    // temporarily unavailable". The backtrace showed the abort originating in
    // std::thread::_M_start_thread() -> std::__throw_system_error(11) (errno 11 = EAGAIN from
    // pthread_create()), reached via yarp::os::impl::ThreadImpl::start() ->
    // yarp::os::impl::PortCore::start() -> yarp::os::Port::open(), while opening a
    // ControlBoard_nws_yarp device late in startup with ~57 threads already alive. Using
    // MCL_CURRENT only (never opting into MCL_FUTURE) fixes this: threads created later by
    // device/port wrappers remain free to map their stacks normally.
    disable_malloc_page_faults();
    disable_transparent_huge_pages();
    request_zero_latency();

    if (stack_prefault_bytes > 0) {
        prefault_stack(stack_prefault_bytes);
    }

    if (heap_reserve_bytes > 0) {
        heap_reserved_ = reserve_heap(heap_reserve_bytes);
    }

    locked_ = lock_process_memory();

    yInfo() << "MemoryGuard: Process memory locked:" << (locked_ ? "yes" : "no");
    yInfo() << "MemoryGuard: Stack prefaulted:" << stack_prefault_bytes << "bytes";
    yInfo() << "MemoryGuard: Heap reserved:" << heap_reserve_bytes << "bytes";
    yInfo() << "MemoryGuard: Heap reservation successful:" << (heap_reserved_ ? "yes" : "no");
    if (last_error_) {
        yError() << "MemoryGuard: Last error:" << last_error_->c_str();
    }
}

MemoryGuard::~MemoryGuard()
{
    unlock();
}

MemoryGuard::MemoryGuard(MemoryGuard&& other) noexcept
        : locked_(other.locked_),
          heap_reserved_(other.heap_reserved_),
          last_error_(std::move(other.last_error_))
{
#if defined(__linux__)
    dma_latency_fd_ = other.dma_latency_fd_;
    other.dma_latency_fd_ = -1;
#endif
    other.locked_ = false;
    other.heap_reserved_ = false;
}

MemoryGuard& MemoryGuard::operator=(MemoryGuard&& other) noexcept
{
    if (this != &other) {
        unlock();
        locked_ = other.locked_;
        heap_reserved_ = other.heap_reserved_;
        last_error_ = std::move(other.last_error_);
#if defined(__linux__)
        dma_latency_fd_ = other.dma_latency_fd_;
        other.dma_latency_fd_ = -1;
#endif
        other.locked_ = false;
        other.heap_reserved_ = false;
    }
    return *this;
}

bool MemoryGuard::locked() const noexcept
{
    return locked_;
}

bool MemoryGuard::heap_reserved() const noexcept
{
    return heap_reserved_;
}

const std::optional<std::string>& MemoryGuard::last_error() const noexcept
{
    return last_error_;
}

void MemoryGuard::set_error(const char* what, int err) noexcept
{
    last_error_ = std::string(what) + ": " + std::strerror(err);
}

bool MemoryGuard::lock_process_memory() noexcept
{
#if defined(__linux__)
    // MCL_FUTURE requires the kernel to keep locking every mapping created after this call
    // too, including the stack of every thread created later (e.g. one per device/port when
    // yarprobotinterface loads a large config). In practice this is unsafe even with a
    // generous RLIMIT_MEMLOCK: it was confirmed experimentally that yarprobotinterface
    // reliably aborts with std::system_error ("Resource temporarily unavailable", i.e.
    // pthread_create() failing with EAGAIN) when MCL_FUTURE is used, works reliably with no
    // mlockall() at all, and is still occasionally unstable with MCL_CURRENT alone. So: only
    // lock the memory that is already mapped (current heap reservation, current stack) and
    // never opt into MCL_FUTURE, so that threads created later by device/port wrappers are
    // free to map their stacks normally.
    if (mlockall(MCL_CURRENT) != 0) {
        set_error("mlockall(MCL_CURRENT)", errno);
        printf("MemoryGuard: Failed to lock process memory: %s\n", last_error_->c_str());
        return false;
    }
    return true;

#elif defined(__APPLE__) || defined(_WIN32)
    return true;
#else
    return true;
#endif
}

void MemoryGuard::disable_malloc_page_faults() noexcept
{
#if defined(__linux__)
    if (mallopt(M_MMAP_MAX, 0) != 1) {
        set_error("mallopt(M_MMAP_MAX)", errno);
        printf("MemoryGuard: Failed to set M_MMAP_MAX: %s\n", last_error_->c_str());
    }
    if (mallopt(M_TRIM_THRESHOLD, -1) != 1) {
        set_error("mallopt(M_TRIM_THRESHOLD)", errno);
        printf("MemoryGuard: Failed to set M_TRIM_THRESHOLD: %s\n", last_error_->c_str());
    }
#endif
}

void MemoryGuard::disable_transparent_huge_pages() noexcept
{
#if defined(__linux__) && defined(PR_SET_THP_DISABLE)
    if (prctl(PR_SET_THP_DISABLE, 1, 0, 0, 0) != 0) {
        set_error("prctl(PR_SET_THP_DISABLE)", errno);
        printf("MemoryGuard: Failed to disable transparent huge pages: %s\n", last_error_->c_str());
    }
#endif
}

void MemoryGuard::request_zero_latency() noexcept
{
#if defined(__linux__)
    dma_latency_fd_ = open("/dev/cpu_dma_latency", O_RDWR);
    if (dma_latency_fd_ >= 0) {
        const int32_t latency_target = 0;
        if (write(dma_latency_fd_, &latency_target, sizeof(latency_target)) < 0) {
            set_error("write(/dev/cpu_dma_latency)", errno);
            printf("MemoryGuard: Failed to write to /dev/cpu_dma_latency: %s\n", last_error_->c_str());
        }
    } else {
        set_error("open(/dev/cpu_dma_latency)", errno);
        printf("MemoryGuard: Failed to open /dev/cpu_dma_latency: %s\n", last_error_->c_str());
    }
#endif
}

void MemoryGuard::prefault_stack(std::size_t bytes) noexcept
{
#if defined(_WIN32)
    volatile std::uint8_t* dummy = static_cast<volatile std::uint8_t*>(_alloca(bytes));
    const std::size_t page_size = query_page_size();
    for (std::size_t i = 0; i < bytes; i += page_size) {
        dummy[i] = 0;
    }
#else
    // Touch pages by recursing through real stack frames (see touch_stack_frames() above)
    // instead of a single alloca(bytes) call, which is reclaimed the instant this function
    // returns and offers no protection against blowing through the stack guard page.
    touch_stack_frames(bytes);
#endif
}

std::size_t MemoryGuard::query_page_size() noexcept
{
#if defined(__linux__) || defined(__APPLE__)
    const long p = sysconf(_SC_PAGESIZE);
    return p > 0 ? static_cast<std::size_t>(p) : 4096;
#elif defined(_WIN32)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
#else
    return 4096;
#endif
}

bool MemoryGuard::reserve_heap(std::size_t bytes) noexcept
{
    // IMPROVEMENT 3: Do not keep the pointer. Allocate, touch, and free immediately.
    const std::size_t page_size = query_page_size();
    std::size_t heap_size = ((bytes + page_size - 1) / page_size) * page_size;
    void* ptr = nullptr;

#if defined(_WIN32)
    ptr = VirtualAlloc(nullptr, heap_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!ptr) {
        set_error("VirtualAlloc", static_cast<int>(GetLastError()));
        return false;
    }
#else
    if (posix_memalign(&ptr, page_size, heap_size) != 0) {
        set_error("posix_memalign", errno);
        printf("MemoryGuard: Failed to allocate heap memory: %s\n", last_error_->c_str());
        return false;
    }
#endif

    volatile std::uint8_t* p = static_cast<volatile std::uint8_t*>(ptr);
    for (std::size_t i = 0; i < heap_size; i += page_size) {
        p[i] = 0;
    }

#if defined(_WIN32)
    if (!VirtualLock(ptr, heap_size)) {
        set_error("VirtualLock", static_cast<int>(GetLastError()));
    }
    VirtualUnlock(ptr, heap_size);
    VirtualFree(ptr, 0, MEM_RELEASE);
#elif defined(__APPLE__)
    if (mlock(ptr, heap_size) != 0) {
        set_error("mlock", errno);
    }
    munlock(ptr, heap_size);
    std::free(ptr);
#else
    // Linux returns the pre-faulted RAM to glibc's arena because of mallopt settings.
    std::free(ptr);
#endif

    return true;
}

void MemoryGuard::unlock() noexcept
{
#if defined(__linux__) || defined(__APPLE__)
    if (locked_) {
        munlockall();
    }
#endif

#if defined(__linux__)
    if (dma_latency_fd_ >= 0) {
        close(dma_latency_fd_);
        dma_latency_fd_ = -1;
    }
#endif

    locked_ = false;
    heap_reserved_ = false;
}