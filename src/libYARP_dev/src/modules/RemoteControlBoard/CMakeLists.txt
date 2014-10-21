# using PARENT_SCOPE to make files visible from upper level
set(rmc_core_srcs   	${CMAKE_CURRENT_SOURCE_DIR}/RemoteControlBoard.cpp 
			${CMAKE_CURRENT_SOURCE_DIR}/stateExtendedReader.cpp 	
			PARENT_SCOPE)

set(rmc_core_hrds   	${CMAKE_CURRENT_SOURCE_DIR}/RemoteControlBoard.h
			${CMAKE_CURRENT_SOURCE_DIR}/stateExtendedReader.hpp 	
			PARENT_SCOPE)

set(rmc_core_incl_dirs  ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
