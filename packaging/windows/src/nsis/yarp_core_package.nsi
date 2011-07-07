
!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "EnvVarUpdate.nsh"
!include "YarpNsisUtils.nsh"

!define MULTIUSER_EXECUTIONLEVEL Highest
!include MultiUser.nsh

Name "YARP ${YARP_VERSION}"
OutFile "${NSIS_OUTPUT_PATH}\yarp_core_${YARP_VERSION}_${BUILD_VERSION}.exe"

InstallDir "$PROGRAMFILES\${VENDOR}"
# this part no longer included in install path "\yarp-${YARP_VERSION}"

InstallDirRegKey HKCU "Software\YARP\Common" "LastInstallLocation"
RequestExecutionLevel admin

!define MUI_ABORTWARNING

!macro AddEnv Key Val
   ${EnvVarUpdate} $0 "${Key}" "A" "${WriteEnvStr_Base}" "${Val}"
   ${EnvVarUpdate} $0 "${VENDOR}_yarp_${Key}" "A" "${WriteEnvStr_Base}" "${Val}"
!macroend

;--------------------------------
;Pages

  !define MUI_PAGE_HEADER_TEXT "Welcome to YARP, Yet Another Robot Platform"
  !define MUI_PAGE_HEADER_SUBTEXT "YARP.  Free software for free robots."
  !define MUI_LICENSEPAGE_TEXT_TOP "YARP is distributed under the LGPL Free Software license."
  !define MUI_LICENSEPAGE_TEXT_BOTTOM "You are free to use YARP personally without agreeing to this license. Follow the terms of the license if you wish to take advantage of the extra rights it grants."
  !define MUI_LICENSEPAGE_BUTTON "Next >"
  !insertmacro MUI_PAGE_LICENSE "${YARP_LICENSE}"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "-first"
  SetOutPath "$INSTDIR"
  # WriteRegStr HKCU "Software\YARP" "" $INSTDIR
  # WriteRegStr HKCU "Software\${VENDOR}" ""

  # All robotology_yarp material should be removed, to avoid
  # environment variables getting too long over time  
   #!insertmacro AddEnv "PATH" "R" "${WriteEnvStr_Base}" "$%robotology_PATH%"

  Push $R0
  ReadEnvStr $R0 "robotology_yarp_PATH" 
  DetailPrint "robotology_yarp_PATH: [$R0]"
  Exch $R0
  Push "PATH"
  Push $0
  Call RemoveOldEnv
  WriteRegExpandStr ${WriteEnvStr_RegKey} "robotology_yarp_PATH" ""
 
  Push $R0
  ReadEnvStr $R0 "robotology_yarp_INCLUDE" 
  DetailPrint "robotology_yarp_INCLUDE: [$R0]"
  Exch $R0
  Push "INCLUDE"
  Push $0
  Call RemoveOldEnv
  WriteRegExpandStr ${WriteEnvStr_RegKey} "robotology_yarp_INCLUDE" ""
 
  Push $R0
  ReadEnvStr $R0 "robotology_yarp_LIB" 
  DetailPrint "robotology_yarp_LIB: [$R0]"
  Exch $R0
  Push "LIB"
  Push $0
  Call RemoveOldEnv
  WriteRegExpandStr ${WriteEnvStr_RegKey} "robotology_yarp_LIB" ""
 
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
   
  WriteUninstaller "$INSTDIR\Uninstall_YARP.exe"
  SectionIn RO
  !include ${NSIS_OUTPUT_PATH}\yarp_base_add.nsi
SectionEnd

SectionGroup "YARP core" SecYarp

  Section "-yarp_first"
    SetOutPath "$INSTDIR"
    SectionIn RO
    !include ${NSIS_OUTPUT_PATH}\yarp_base_add.nsi
  SectionEnd

  Section "Command-line utilities" SecPrograms
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_programs_add.nsi
  SectionEnd

  Section "Libraries" SecLibraries
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_libraries_add.nsi
  SectionEnd

  Section "Header files" SecHeaders
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_headers_add.nsi
  SectionEnd

  Section "Runtime DLLs" SecDLLs
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_dlls_add.nsi
  SectionEnd
	
  Section "Examples" SecExamples
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_examples_add.nsi
  SectionEnd

  Section "Set environment variables and registry keys" SecYarpEnv
    WriteRegStr HKCU "Software\${VENDOR}\YARP\${YARP_SUB}" "" "$INSTDIR\${YARP_SUB}"
    WriteRegStr HKCU "Software\${VENDOR}\YARP\Common" "LastInstallLocation" $INSTDIR
    WriteRegStr HKCU "Software\${VENDOR}\YARP\Common" "LastInstallVersion" ${YARP_SUB}
    !insertmacro AddEnv "PATH" "$INSTDIR\${YARP_SUB}\bin"
    !insertmacro AddEnv "LIB" "$INSTDIR\${YARP_SUB}\lib"
    !insertmacro AddEnv "INCLUDE" "$INSTDIR\${YARP_SUB}\include"
    WriteRegExpandStr ${WriteEnvStr_RegKey} YARP_DIR "$INSTDIR\${YARP_SUB}"
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  SectionEnd

SectionGroupEnd

SectionGroup "ACE (Adaptive Communication Environment)" SecAce
  Section "ACE headers" SecAceHeaders
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\ace_headers_add.nsi
  SectionEnd

  Section "ACE library" SecAceLibraries
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\ace_libraries_add.nsi
  SectionEnd

  Section "ACE runtime DLL" SecAceDLLs
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\ace_dlls_add.nsi
  SectionEnd

  Section "Set environment variables and registry keys" SecAceEnv
    WriteRegStr HKCU "Software\${VENDOR}\ACE\${ACE_SUB}" "" "$INSTDIR\${ACE_SUB}"
    WriteRegStr HKCU "Software\${VENDOR}\ACE\Common" "LastInstallLocation" $INSTDIR
    WriteRegStr HKCU "Software\${VENDOR}\ACE\Common" "LastInstallVersion" ${YARP_SUB}
    !insertmacro AddEnv "PATH" "$INSTDIR\${ACE_SUB}\bin"
    !insertmacro AddEnv "LIB" "$INSTDIR\${ACE_SUB}\lib"
    !insertmacro AddEnv "INCLUDE" "$INSTDIR\${ACE_SUB}"
    WriteRegExpandStr ${WriteEnvStr_RegKey} ACE_ROOT "$INSTDIR\${ACE_SUB}"
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  SectionEnd

SectionGroupEnd
  
SectionGroup "GSL (GNU Scientific Library) (GPL license)" SecGsl
  Section "GSL headers" SecGslHeaders
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\gsl_headers_add.nsi
  SectionEnd

  Section "GSL libraries" SecGslLibraries
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\gsl_libraries_add.nsi
  SectionEnd

  Section "YARP headers for math" SecYarpMathHeaders
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_math_headers_add.nsi
  SectionEnd

  Section "YARP library for math" SecYarpMathLibraries
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_math_libraries_add.nsi
  SectionEnd
  
  Section "YARP runtime DLL for math" SecYarpMathDLLs
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_math_dlls_add.nsi
  SectionEnd

  Section "Set environment variables and registry keys" SecGslEnv
    WriteRegStr HKCU "Software\${VENDOR}\GSL\${GSL_SUB}" "" "$INSTDIR\${GSL_SUB}"
    WriteRegStr HKCU "Software\${VENDOR}\GSL\Common" "LastInstallLocation" $INSTDIR
    WriteRegStr HKCU "Software\${VENDOR}\GSL\Common" "LastInstallVersion" ${GSL_SUB}
    # !insertmacro AddEnv "PATH" "$INSTDIR\${GSL_SUB}\bin"
    !insertmacro AddEnv "LIB" "$INSTDIR\${GSL_SUB}\lib"
    !insertmacro AddEnv "INCLUDE" "$INSTDIR\${GSL_SUB}\include"
    WriteRegExpandStr ${WriteEnvStr_RegKey} GSL_DIR "$INSTDIR\${GSL_SUB}"
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  SectionEnd

SectionGroupEnd

SectionGroup "GTKMM" SecGtkmm

  Section "GTKMM headers" SecGtkmmHeaders
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\gtkmm_headers_add.nsi
  SectionEnd

  Section "GTKMM library" SecGtkmmLibraries
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\gtkmm_libraries_add.nsi
  SectionEnd

  Section "GTKMM runtime DLL" SecGtkmmDLLs
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\gtkmm_dlls_add.nsi
  SectionEnd

  Section "Set environment variables and registry keys" SecGtkmmEnv
    WriteRegStr HKCU "Software\${VENDOR}\GTKMM\${GSL_SUB}" "" "$INSTDIR\${GTKMM_SUB}"
    WriteRegStr HKCU "Software\${VENDOR}\GTKMM\Common" "LastInstallLocation" $INSTDIR
    WriteRegStr HKCU "Software\${VENDOR}\GTKMM\Common" "LastInstallVersion" ${GTKMM_SUB}
    !insertmacro AddEnv "PATH" "$INSTDIR\${GTKMM_SUB}\bin"
    !insertmacro AddEnv "LIB" "$INSTDIR\${GTKMM_SUB}\lib"
    !insertmacro AddEnv "INCLUDE" "$INSTDIR\${GTKMM_SUB}\include"
    WriteRegExpandStr ${WriteEnvStr_RegKey} GTKMM_BASEPATH "$INSTDIR\${GTKMM_SUB}"
    WriteRegExpandStr ${WriteEnvStr_RegKey} GTK_BASEPATH "$INSTDIR\${GTKMM_SUB}"
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  SectionEnd

  Section "yarpview utility" SecGuis
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_guis_add.nsi
    #CreateShortCut "$INSTDIR\bin\yarpview.lnk" "$INSTDIR\yarpview\yarpview.exe"
  SectionEnd

SectionGroupEnd

Section "Visual Studio Runtime (nonfree)" SecVcDlls
  SetOutPath "$INSTDIR"
  !include ${NSIS_OUTPUT_PATH}\yarp_vc_dlls_add.nsi
SectionEnd

Section "-last"
  !insertmacro SectionFlagIsSet ${SecGsl} ${SF_PSELECTED} isSel chkAll
   chkAll:
     !insertmacro SectionFlagIsSet ${SecGsl} ${SF_SELECTED} isSel notSel
   notSel:
     !insertmacro ReplaceInFile "$INSTDIR\${YARP_SUB}\cmake\YARPConfig.cmake" "YARP_math;" ""
	 !insertmacro ReplaceInFile "$INSTDIR\${YARP_SUB}\cmake\YARPConfig.cmake" "YARP_HAS_MATH_LIB TRUE" "YARP_HAS_MATH_LIB FALSE"
   isSel:
SectionEnd

;--------------------------------
;Descriptions

;Language strings
LangString DESC_SecYarp ${LANG_ENGLISH} "YARP libraries and tools. Unselect this if you intend to compile YARP yourself and just want YARP's dependencies."
LangString DESC_SecAce ${LANG_ENGLISH} "The Adaptive Communications Environment, used by this version of YARP."
LangString DESC_SecGsl ${LANG_ENGLISH} "The YARP math library.  Based on the GNU Scientific Library.  This is therefore GPL software, not the LGPL like YARP."
LangString DESC_SecGtkmm ${LANG_ENGLISH} "User interface library.  Not needed to use the YARP library.  Used by the yarpview program."

LangString DESC_SecPrograms ${LANG_ENGLISH} "YARP programs, including the standard YARP companion, and the standard YARP name server."
LangString DESC_SecDevelopment ${LANG_ENGLISH} "Files needed for compiling against YARP."
LangString DESC_SecLibraries ${LANG_ENGLISH} "Libraries for linking against YARP."
LangString DESC_SecHeaders ${LANG_ENGLISH} "YARP header files."
LangString DESC_SecExamples ${LANG_ENGLISH} "A basic example of using YARP.  See online documentation, and many more examples in source code package."
LangString DESC_SecRuntime ${LANG_ENGLISH} "Files needed for running YARP programs."
LangString DESC_SecDLLs ${LANG_ENGLISH} "Libraries needed for YARP programs to run."

LangString DESC_SecAceLibraries ${LANG_ENGLISH} "ACE library files."
LangString DESC_SecAceDLLs ${LANG_ENGLISH} "ACE library run-time."

LangString DESC_SecGslLibraries ${LANG_ENGLISH} "Math library files."
LangString DESC_SecGslDLLs ${LANG_ENGLISH} "Math library run-time."
LangString DESC_SecGslHeaders ${LANG_ENGLISH} "Math library header files."
LangString DESC_SecGuis ${LANG_ENGLISH} "Utility for viewing image streams.  Uses GTK+."
LangString DESC_SecPath ${LANG_ENGLISH} "Add YARP to PATH, LIB, and INCLUDE variables, and set YARP_DIR variable."
LangString DESC_SecDebug ${LANG_ENGLISH} "Debug versions of the YARP and ACE libraries."

LangString DESC_SecVcDlls ${LANG_ENGLISH} "Visual Studio runtime redistributable files.  Not free software.  If you already have Visual Studio installed, you may want to skip this."

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecYarp} $(DESC_SecYarp)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecGtkmm} $(DESC_SecGtkmm)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPrograms} $(DESC_SecPrograms)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDevelopment} $(DESC_SecDevelopment)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecLibraries} $(DESC_SecLibraries)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecHeaders} $(DESC_SecHeaders)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecExamples} $(DESC_SecExamples)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecRuntime} $(DESC_SecRuntime)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDLLs} $(DESC_SecDLLs)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecVcDlls} $(DESC_SecVcDlls)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecAce} $(DESC_SecAce)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecAceLibraries} $(DESC_SecAceLibraries)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecAceDLLs} $(DESC_SecAceDLLs)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecGsl} $(DESC_SecGsl)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecGslLibraries} $(DESC_SecGslLibraries)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecGslDLLs} $(DESC_SecGslDLLs)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecGslHeaders} $(DESC_SecGslHeaders)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecGuis} $(DESC_SecGuis)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPath} $(DESC_SecPath)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDebug} $(DESC_SecDebug)
  #!insertmacro MUI_DESCRIPTION_TEXT ${Sec} $(DESC_Sec)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
  ${un.EnvVarUpdate} $0 "PATH" "R" "${WriteEnvStr_Base}" "$INSTDIR\${YARP_SUB}\bin"
  ${un.EnvVarUpdate} $0 "LIB" "R" "${WriteEnvStr_Base}" "$INSTDIR\${YARP_SUB}\lib"
  ${un.EnvVarUpdate} $0 "INCLUDE" "R" "${WriteEnvStr_Base}" "$INSTDIR\${YARP_SUB}\include"

  DeleteRegValue ${WriteEnvStr_RegKey} YARP_DIR
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  
  RMDir /r "$INSTDIR\${YARP_SUB}"
  RMDir /r "$INSTDIR\${GSL_SUB}"

  # cleanup YARP registry entries
  DeleteRegKey HKCU "Software\${VENDOR}\YARP\Common\LastInstallLocation"
  DeleteRegKey HKCU "Software\${VENDOR}\YARP\Common\LastInstallVersion"
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\YARP\Common"
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\YARP\${YARP_SUB}"
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\YARP"
  
  # cleanup GSL registry entries
  DeleteRegKey HKCU "Software\${VENDOR}\GSL\Common\LastInstallLocation"
  DeleteRegKey HKCU "Software\${VENDOR}\GSL\Common\LastInstallVersion"  
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\GSL\Common"
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\GSL\${GSL_SUB}"
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\GSL"

  # cleanup vendor entry if empty
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}"

  Delete "$INSTDIR\Uninstall_YARP.exe"
  
SectionEnd

Function .onInit
  !insertmacro MULTIUSER_INIT
FunctionEnd

Function un.onInit
  !insertmacro MULTIUSER_UNINIT
FunctionEnd
