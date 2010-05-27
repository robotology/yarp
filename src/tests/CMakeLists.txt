
set(targets OS sig dev)
get_property(YARP_HAS_MATH_LIB GLOBAL PROPERTY YARP_HAS_MATH_LIB)
if (YARP_HAS_MATH_LIB)
  set(targets ${targets} math)
endif (YARP_HAS_MATH_LIB)

foreach(test_family ${targets})
  file(GLOB harness_code ../libYARP_${test_family}/harness/*.cpp)
  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_${test_family} 
               PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})
  include_directories(${ACE_INCLUDE_DIRS})
  set(EXE harness_${test_family})
  string(TOLOWER ${EXE} EXE)
  add_executable(${EXE} ${harness_code})
  target_link_libraries(${EXE} YARP_init YARP_${test_family})
  GET_TARGET_PROPERTY(EXELOC ${EXE} LOCATION)
  foreach(test ${harness_code})
    get_filename_component(XN ${test} NAME_WE)
    add_test("${test_family}::${XN}" ${EXELOC} regression ${XN})
  endforeach()
endforeach()

# touch 1
