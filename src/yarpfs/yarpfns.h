/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2007 Giacomo Spigler
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARPFNS_INC
#define YARPFNS_INC

#include <fuse/fuse.h>

// in yarpfs.cpp
int yarp_getattr(const char *path, struct stat *stbuf);
int yarp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi);
int yarp_open(const char *path, struct fuse_file_info *fi);
int yarp_release(const char *path, struct fuse_file_info *fi);
int yarp_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi);
int yarp_write(const char *path, const char *buf, size_t size,
               off_t offset, struct fuse_file_info *fi);
int yarp_rename(const char *from, const char *to);
int yarp_rmdir(const char *path);
int yarp_truncate(const char *path, off_t size);
void *yarp_init(struct fuse_conn_info *conn);

// in link.cpp
int yarp_readlink(const char *path, char *buf, size_t size);
int yarp_unlink(const char *path);
int yarp_symlink(const char *from, const char *to);
int yarp_link(const char *from, const char *to);

#endif
