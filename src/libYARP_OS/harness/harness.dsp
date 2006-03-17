# Microsoft Developer Studio Project File - Name="harness" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=harness - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "harness.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "harness.mak" CFG="harness - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "harness - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "harness - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "harness - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\obj\winnt"
# PROP Intermediate_Dir ".\obj\winnt"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\src-test" /I "..\..\..\include" /I "..\..\..\include\winnt" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 libYARP_OS2_test.lib libYARP_OS2.lib ace.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\..\lib\winnt" /libpath:"..\lib\winnt"

!ELSEIF  "$(CFG)" == "harness - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\obj\winnt"
# PROP Intermediate_Dir ".\obj\winnt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\src-test" /I "..\..\..\include" /I "..\..\..\include\winnt" /I "$(ACE_INCLUDE)" /I "$(ACE_INCLUDE)\winnt" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libYARP_OSd.lib aced.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\lib\winnt" /libpath:"..\..\..\lib\winnt" /libpath:"$(ACE_LIB)"

!ENDIF 

# Begin Target

# Name "harness - Win32 Release"
# Name "harness - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddressTest.cpp
# End Source File
# Begin Source File

SOURCE=.\BottleTest.cpp
# End Source File
# Begin Source File

SOURCE=.\BufferedConnectionWriterTest.cpp
# End Source File
# Begin Source File

SOURCE=.\ElectionTest.cpp
# End Source File
# Begin Source File

SOURCE=.\harness.cpp
# End Source File
# Begin Source File

SOURCE=.\NameConfigTest.cpp
# End Source File
# Begin Source File

SOURCE=.\NameServerTest.cpp
# End Source File
# Begin Source File

SOURCE=.\PortCommandTest.cpp
# End Source File
# Begin Source File

SOURCE=.\PortCoreTest.cpp
# End Source File
# Begin Source File

SOURCE=.\PortTest.cpp
# End Source File
# Begin Source File

SOURCE=.\ProtocolTest.cpp
# End Source File
# Begin Source File

SOURCE=.\StreamConnectionReaderTest.cpp
# End Source File
# Begin Source File

SOURCE=.\StringInputStreamTest.cpp
# End Source File
# Begin Source File

SOURCE=.\StringOutputStreamTest.cpp
# End Source File
# Begin Source File

SOURCE=.\StringTest.cpp
# End Source File
# Begin Source File

SOURCE=.\TestList.h
# End Source File
# Begin Source File

SOURCE=.\ThreadTest.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeTest.cpp
# End Source File
# End Group
# End Target
# End Project
