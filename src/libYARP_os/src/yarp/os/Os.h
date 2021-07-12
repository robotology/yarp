/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_OS_H
#define YARP_OS_OS_H

#include <yarp/os/api.h>

#include <cstddef>
#include <string>


namespace yarp {
namespace os {

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4.0
/**
 * @brief Portable wrapper for the getenv() function.
 *
 * Get an environment variable.
 *
 * @param[in] var string that contains the environment variable name
 * @return the value corresponding to the envarionment variable v
 *
 * @deprecated since YARP 3.4.0. Use std::getenv instead.
 */
YARP_DEPRECATED_MSG("Use std::getenv instead")
YARP_os_API const char* getenv(const char* var);
#endif // YARP_NO_DEPRECATED

/**
 * @brief Portable wrapper for the getppid() function.
 *
 * Get process identification.
 *
 * @return the process id (pid)
 */
YARP_os_API int getpid();

/**
 * @brief Portable wrapper for the gethostname() function.
 *
 * Returns the null-terminated hostname in the character array
 * @c hostname, which has a length of @c size bytes.
 *
 * @param[out] hostname the system hostname
 * @param size The size of the @c hostname array
 */
YARP_os_API void gethostname(char* hostname, size_t size);

/**
 * @brief Portable wrapper for the gethostname() function.
 *
 * Returns the hostname as string
 *
 * @return hostname the system hostname
 */
YARP_os_API std::string gethostname();

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
YARP_os_API int mkdir(const char* p);

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
YARP_os_API int mkdir_p(const char* p, int ignoreLevels = 0);

/**
 * @brief Portable wrapper for the rmdir() function.
 *
 * Remove an empty directory.
 *
 * @param[in] p name of the directory
 * @return 0 on success
 */
YARP_os_API int rmdir(const char* p);

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
YARP_os_API int rename(const char* oldname, const char* newname);

/**
 * @brief Portable wrapper for the stat() function.
 *
 * Get file status.
 *
 * @param[in]
 * @return 0 on success (i.e. the file exists)
 */
YARP_os_API int stat(const char* path);

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
 */
YARP_os_API char* getcwd(char* buf, size_t size);

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
 */
YARP_os_API void setEnergySavingModeState(bool enabled);


#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
/**
 * @brief Portable wrapper for the setprogname() function.
 *
 * Set the program name.
 *
 * @param[in] progname the program name
 * @deprecated Since YARP 3.0.0.
 */
YARP_DEPRECATED
YARP_os_API void setprogname(const char* progname);

/**
 * @brief Portable wrapper for the getprogname() function.
 *
 * Get the program name.
 *
 * @param[out] progname the program name
 * @param size The size of the @c progname array
 * @deprecated Since YARP 3.0.0. Use yarp::os::SystemInfo::getProcessInfo().name.
 */
YARP_DEPRECATED_MSG("This method is deprecated. Use yarp::os::SystemInfo::getProcessInfo().name instead")
YARP_os_API void getprogname(char* progname, size_t size);

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
 * @deprecated since YARP 3.0.0
 */
YARP_os_DEPRECATED_API_MSG("Use native fork()")
int fork();
#endif // YARP_NO_DEPRECATED


} // namespace os
} // namespace yarp

#endif // YARP_OS_OS_H
