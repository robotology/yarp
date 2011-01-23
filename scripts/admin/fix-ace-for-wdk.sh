#!/bin/bash

# call from build directory

src=ace4yarp/src
inc=ace4yarp/include/ace

sed -i "s/\([ ()]\)::strdup/\1::_strdup/g" $inc/OS_NS_string.inl
sed -i "s/\([ ()]\)::unlink/\1::_unlink/g" $inc/OS_NS_unistd.inl
sed -i "s/\([ ()]\)::access/\1::_access/g" $inc/OS_NS_unistd.inl
sed -i "s/\([ ()]\)::fdopen/\1::_fdopen/g" $inc/OS_NS_stdio.inl
sed -i "s/\([ ()]\)::fdopen/\1::_fdopen/g" $src/OS_NS_stdio.cpp
sed -i "s/\([ ()]\)::mkdir/\1::_mkdir/g" $inc/OS_NS_sys_stat.inl

sed -i "s/(\(msg_len >= 0\)/((int)\1/g" $src/WIN32_Asynch_IO.cpp
