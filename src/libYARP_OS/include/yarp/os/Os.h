/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_OS_H
#define YARP_OS_OS_H

#include <yarp/os/api.h>

#include <cstddef>


namespace yarp {
namespace os {
#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.70
enum
{
    YARP_SIGTERM,
    YARP_SIGINT
};

typedef void (*YarpSignalHandler)(int);
/**
 * @brief Portable wrapper for the signal() function.
 *
 * Attach a function handler to a signal.
 *
 * @param signum signal number (e.g. SIGTERM/SIGINT) to handle
 * @param sighandler handler function
 * @deprecated Since YARP 2.3.70. Use std::signal().
 */
YARP_DEPRECATED_MSG("Use std::signal")
YARP_OS_API YarpSignalHandler signal(int signum, YarpSignalHandler sighandler);
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.70
/**
 * @brief Portable wrapper for the exit() function.
 *
 * @deprecated Since YARP 2.3.70. Use std::exit().
 */
YARP_OS_DEPRECATED_API_MSG("Use std::exit()")
void exit(int exit_code); // FIXME noreturn
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.70
/**
 * @brief Portable wrapper for the abort() function.
 *
 * @deprecated Since YARP 2.3.70. Use std::abort().
 */
YARP_OS_DEPRECATED_API_MSG("Use std::abort()")
void abort(bool verbose = false);
#endif // YARP_NO_DEPRECATED

/**
 * @brief Portable wrapper for the getenv() function.
 *
 * Get an environment variable.
 *
 * @param[in] var string that containt the environment variable name
 * @return the value corresponding to the envarionment variable v
 */
YARP_OS_API const char* getenv(const char* var);

/**
 * @brief Portable wrapper for the getppid() function.
 *
 * Get process identification.
 *
 * @return the process id (pid)
 */
YARP_OS_API int getpid();

/**
 * @brief Portable wrapper for the setprogname() function.
 *
 * Set the program name.
 *
 * @param[in] progname the program name
 */
YARP_OS_API void setprogname(const char* progname);

/**
 * @brief Portable wrapper for the getprogname() function.
 *
 * Get the program name.
 *
 * @param[out] progname the program name
 * @param size The size of the @c progname array
 */
YARP_OS_API void getprogname(char* progname, size_t size);

/**
 * @brief Portable wrapper for the gethostname() function.
 *
 * Returns the null-terminated hostname in the character array
 * @c hostname, which has a length of @c size bytes.
 *
 * @param[out] hostname the system hostname
 * @param size The size of the @c hostname array
 */
YARP_OS_API void gethostname(char* hostname, size_t size);

/**
 * @brief Portable wrapper for the mkdir() function.
 *
 * Create a directory.
 *
 * @param[in] p name of the new directory
 * @return 0 on success
 *
 * @sa mkdir_p
 */
YARP_OS_API int mkdir(const char* p);

/**
 * @brief Create a directory and all parent directories needed.
 *
 * @param[in] p desired path
 * @param ignoreLevels components of name to ignore. Set to 1 if
 * last element of path is a filename, for example
 * @return 0 on success
 *
 * @sa mkdir
 */
YARP_OS_API int mkdir_p(const char* p, int ignoreLevels = 0);

/**
 * @brief Portable wrapper for the rmdir() function.
 *
 * Remove an empty directory.
 *
 * @param[in] p name of the directory
 * @return 0 on success
 */
YARP_OS_API int rmdir(const char* p);

/**
 * @brief Portable wrapper for the rename() function.
 *
 * Changes the name of the file or directory specified by @c oldname to
 * @c newname.
 *
 * @param[in] oldname old name of the file/directory
 * @param[in] newname new name of the file/directory
 * @return 0 on success
 */
YARP_OS_API int rename(const char* oldname, const char* newname);

/**
 * @brief Portable wrapper for the stat() function.
 *
 * Get file status.
 *
 * @param[in]
 * @return 0 on success (i.e. the file exists)
 */
YARP_OS_API int stat(const char* path);

/**
 * @brief Portable wrapper for the getcwd() function.
 *
 * Get current working directory.
 * The getcwd() function copies an absolute pathname of the current
 * working directory to the array pointed to by @c buf, which is of
 * length @c size.
 *
 * @param[out] buf The buffer where the path is copied
 * @param size The size of the buffer
 * @return a pointer to @c buf or NULL on failure
 *
 * @since YARP 2.3.70
 */
YARP_OS_API char* getcwd(char* buf, size_t size);

/**
 * @brief Portable wrapper for the fork() function.
 *
 * Create a child process.
 *
 * @return On success, the PID of the child process is returned in the
 * parent, and 0 is returned in the child. On failure, -1 is returned
 * in the parent, no child process is created, and errno is set
 * appropriately
 *
 * @since YARP 2.3.70
 */
YARP_OS_API int fork(void);

/**
 * @brief Toggle the OS energy saving feature
 *
 * This function toggle the state of the energy saving feature
 * that may affect YARP calls.
 *
 * For example, in macOS this function toggles the state of the AppNap
 * feature, which put the process into an idle state.
 *
 * @param enable true if the energy saving mode should be enabled. False otherwise.
 * @since YARP 2.3.72
 */
YARP_OS_API void setEnergySavingModeState(bool enabled);

} // namespace os
} // namespace yarp

#endif // YARP_OS_OS_H
