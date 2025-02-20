# SPDX-FileCopyrightText: 2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# This test compile an example executable that include several YARP headers, and verify
# that they compile given the specified PUBLIC target_compile_features
add_executable(header_smoke_test header_smoke_test.cpp)
target_link_libraries(header_smoke_test PRIVATE YARP::YARP_os YARP::YARP_sig YARP::YARP_dev)

# We need to ensure that headers are compatible with C++17, if in the future we want to
# change the minimum compatible version of the headers, this value should be updated
# with the minimum version for which we want to keep compatibility.
set_target_properties(header_smoke_test PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS OFF)
