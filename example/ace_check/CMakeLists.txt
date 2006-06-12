
# This is a small project to check if ace is working
PROJECT(ace_check)

# There is a script for finding the ACE library in the current directory
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

# We need ACE
FIND_PACKAGE(Ace REQUIRED)

# Find source code files
FILE(GLOB SRC *.cpp)

# Create our executable
ADD_EXECUTABLE(ace_check ${SRC})

