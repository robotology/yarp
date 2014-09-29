# Copyright: (C) 2011 IITRBCS
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

enable_testing()

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpIDL)

set(generated_libs_dir "${CMAKE_CURRENT_BINARY_DIR}")
yarp_idl_to_dir(demo.thrift ${generated_libs_dir})
yarp_idl_to_dir(namespaced.thrift ${generated_libs_dir})
yarp_idl_to_dir(objects3D.thrift ${generated_libs_dir})
yarp_idl_to_dir(wrapping.thrift ${generated_libs_dir})

include_directories(${generated_libs_dir}/demo)
include_directories(${generated_libs_dir}/namespaced)
include_directories(${generated_libs_dir}/objects3d)
include_directories(${generated_libs_dir}/wrapping)
include_directories(${YARP_INCLUDE_DIRS})
add_executable(demo_test main.cpp ${generated_libs_dir}/demo/Demo.h ${generated_libs_dir}/demo/Demo.cpp ${generated_libs_dir}/demo/DemoEnum.h ${generated_libs_dir}/demo/DemoEnum.cpp ${generated_libs_dir}/demo/DemoStruct.cpp ${generated_libs_dir}/demo/DemoStruct.h ${generated_libs_dir}/demo/DemoStructList.cpp ${generated_libs_dir}/demo/DemoStructList.h   ${generated_libs_dir}/namespaced/NSDemo.h ${generated_libs_dir}/namespaced/NSDemo.cpp ${generated_libs_dir}/namespaced/NSDemoEnum.h ${generated_libs_dir}/namespaced/NSDemoEnum.cpp ${generated_libs_dir}/namespaced/NSDemoStruct.cpp ${generated_libs_dir}/namespaced/NSDemoStruct.h ${generated_libs_dir}/objects3d/Box3D.h ${generated_libs_dir}/objects3d/Box3D.cpp ${generated_libs_dir}/objects3d/PointXYZ.h ${generated_libs_dir}/objects3d/PointXYZ.cpp ${generated_libs_dir}/objects3d/Polygon.h ${generated_libs_dir}/objects3d/Polygon.cpp ${generated_libs_dir}/objects3d/RGBA.h ${generated_libs_dir}/objects3d/RGBA.cpp ${generated_libs_dir}/objects3d/SurfaceMesh.h ${generated_libs_dir}/objects3d/SurfaceMesh.cpp ${generated_libs_dir}/objects3d/SurfaceMeshWithBoundingBox.h ${generated_libs_dir}/objects3d/SurfaceMeshWithBoundingBox.cpp ${generated_libs_dir}/wrapping/Wrapping.h ${generated_libs_dir}/wrapping/Wrapping.cpp ${generated_libs_dir}/demo/DemoStructExt.h ${generated_libs_dir}/demo/DemoStructExt.cpp)
target_link_libraries(demo_test ${YARP_LIBRARIES})

add_test(demo_basic demo_test)


