# @author Luca Tricerri <luca.tricerri@iit.it> 
project(ultrapythonunittest)

set(SOURCES gtest_main.cc
            test1.cpp
            )

set(CMAKE_EXE_LINKER_FLAGS  "-rdynamic")
set(CMAKE_INSTALL_RPATH ./)

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} gtest gmock)

# install
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})