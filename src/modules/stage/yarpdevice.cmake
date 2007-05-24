
SET(YARPDEV_NAME "stage")
SET(YARPDEV_WRAPPER "controlboard")
SET(YARPDEV_INCLUDE "StageControl.h")
SET(YARPDEV_TYPE "StageControl")

IF (ENABLE_stage)
	MESSAGE(STATUS "checking for STAGE libraries etc")
	FIND_PACKAGE(Stage)
	IF (Stage_FOUND)
	ELSE (Stage_FOUND)
		MESSAGE(STATUS "STAGE libraries not found")
	ENDIF(Stage_FOUND)
ENDIF (ENABLE_stage)
