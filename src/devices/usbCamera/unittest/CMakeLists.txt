# @author Luca Tricerri <luca.tricerri@iit.it> 
project(ultrapythonunittest)

set(SOURCES gtest_main.cc
            test1.cpp
            ../linux/InterfaceForCFunction.h
            ../linux/PythonCameraHelper.h
            ../linux/PythonCameraHelper.cpp
            )

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} gmock gtest udev)

# install
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        )