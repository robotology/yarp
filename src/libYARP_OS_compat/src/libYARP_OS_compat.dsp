# Microsoft Developer Studio Project File - Name="libYARP_OS_compat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libYARP_OS_compat - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libYARP_OS_compat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libYARP_OS_compat.mak" CFG="libYARP_OS_compat - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libYARP_OS_compat - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libYARP_OS_compat - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libYARP_OS_compat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\obj\winnt"
# PROP Intermediate_Dir "..\obj\winnt\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../include" /I "../../../include" /I "../../../include/winnt" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\winnt\libYARP_OS_compat.lib"

!ELSEIF  "$(CFG)" == "libYARP_OS_compat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\obj\winnt"
# PROP Intermediate_Dir "..\obj\winnt\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "../../libYARP_OS/include" /I "../../../include" /I "../../../include/winnt" /I "$(ACE_INCLUDE)" /I "$(ACE_INCLUDE)/winnt" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\winnt\libYARP_OS_compatd.lib"

!ENDIF 

# Begin Target

# Name "libYARP_OS_compat - Win32 Release"
# Name "libYARP_OS_compat - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\YARP1.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPBottle.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPConfigFile.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPParseParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPPort.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPRefCount.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPScheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSemaphore.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPThread.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPTime.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\yarp\AbstractCarrier.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Adapters.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Address.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\BlockReader.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\BlockWriter.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Bottle.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\BottleImpl.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\BufferedBlockWriter.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Bytes.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Carrier.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Carriers.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Companion.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\ConnectionReader.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\ConnectionWriter.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\ConstString.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Contact.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\DgramTwoWayStream.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Dispatcher.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Election.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Face.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\FacePortManager.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\FakeFace.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\FakeTwoWayStream.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\FallbackNameClient.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\FallbackNameServer.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\InputConnection.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\InputProtocol.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\InputStream.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\IOException.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Logger.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\ManagedBytes.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\McastCarrier.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Name.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\NameClient.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\NameConfig.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\NameServer.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\NetType.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\OutputConnection.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\OutputProtocol.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\OutputStream.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Port.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Portable.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\PortCommand.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\PortCore.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\PortCoreInputUnit.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\PortCoreOutputUnit.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\PortCoreUnit.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\PortManager.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\PortReader.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\PortWriter.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Protocol.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Readable.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Route.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Runnable.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Semaphore.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\SemaphoreImpl.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\ShiftStream.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\SizedWriter.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\SocketTwoWayStream.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\SplitString.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\StreamBlockReader.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\String.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\StringInputStream.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\StringOutputStream.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\TcpCarrier.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\TcpFace.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\TextCarrier.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Thread.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\ThreadImpl.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Time.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\TwoWayStream.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\UdpCarrier.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\UnitTest.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\UserDoc.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\Writable.h
# End Source File
# End Group
# End Target
# End Project
