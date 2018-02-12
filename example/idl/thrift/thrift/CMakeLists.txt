# Copyright: (C) 2012 Istituto Italiano di Tecnologia (IIT)
# Authors: Elena Ceseracciu
# CopyPolicy:  Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set(THRIFT_FILES_LIST firstInterface/PointD.thrift secondInterface/demo.thrift)

set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}/../generated")
foreach(thrift_file ${THRIFT_FILES_LIST})
   yarp_idl_to_dir(${thrift_file} ${generated_libs_dir})
endforeach()


