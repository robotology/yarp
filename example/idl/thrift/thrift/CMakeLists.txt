# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

set(THRIFT_FILES_LIST firstInterface/PointD.thrift secondInterface/demo.thrift)

set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}/../generated")
foreach(thrift_file ${THRIFT_FILES_LIST})
   yarp_idl_to_dir(${thrift_file} ${generated_libs_dir})
endforeach()


