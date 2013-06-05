# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(UNIX)
    include(FindGtkMMUnix)
elseif(WIN32 AND NOT CYGWIN)
    include(FindGtkMMWin32)
endif(UNIX)
