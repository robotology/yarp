# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF(UNIX)
    INCLUDE(FindGtkMMUnix)
ELSE(UNIX)
    IF(WIN32 AND NOT CYGWIN)
        INCLUDE(FindGtkMMWin32)
    ENDIF(WIN32 AND NOT CYGWIN)
ENDIF(UNIX)
