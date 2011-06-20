
!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "EnvVarUpdate.nsh"

!define MULTIUSER_EXECUTIONLEVEL Highest
!include MultiUser.nsh

Name "YARP ${YARP_VERSION}"
OutFile "${NSIS_OUTPUT_PATH}\yarp_core_${YARP_VERSION}_${BUILD_VERSION}.exe"

InstallDir "$PROGRAMFILES\${VENDOR}"
# this part no longer included in install path "\yarp-${YARP_VERSION}"

InstallDirRegKey HKCU "Software\YARP\Common" "LastInstallLocation"
RequestExecutionLevel admin

!define MUI_ABORTWARNING

;--------------------------------
;Utilities

!define StrRepLocal "!insertmacro StrRepLocal"
!macro StrRepLocal output string old new
    Push "${string}"
    Push "${old}"
    Push "${new}"
    !ifdef __UNINSTALL__
        Call un.StrRepLocal
    !else
        Call StrRepLocal
    !endif
    Pop ${output}
!macroend
 
!macro Func_StrRepLocal un
    Function ${un}StrRepLocal
        Exch $R2 ;new
        Exch 1
        Exch $R1 ;old
        Exch 2
        Exch $R0 ;string
        Push $R3
        Push $R4
        Push $R5
        Push $R6
        Push $R7
        Push $R8
        Push $R9
 
        StrCpy $R3 0
        StrLen $R4 $R1
        StrLen $R6 $R0
        StrLen $R9 $R2
        loop:
            StrCpy $R5 $R0 $R4 $R3
            StrCmp $R5 $R1 found
            StrCmp $R3 $R6 done
            IntOp $R3 $R3 + 1
            Goto loop
        found:
            StrCpy $R5 $R0 $R3
            IntOp $R8 $R3 + $R4
            StrCpy $R7 $R0 "" $R8
            StrCpy $R0 $R5$R2$R7
            StrLen $R6 $R0
            IntOp $R3 $R3 + $R9
            Goto loop
        done:
 
        Pop $R9
        Pop $R8
        Pop $R7
        Pop $R6
        Pop $R5
        Pop $R4
        Pop $R3
        Push $R0
        Push $R1
        Pop $R0
        Pop $R1
        Pop $R0
        Pop $R2
        Exch $R1
    FunctionEnd
!macroend
!insertmacro Func_StrRepLocal ""

Function ReplaceInFileFunction
 
  ClearErrors
  Exch $0      ; REPLACEMENT
  Exch
  Exch $1      ; SEARCH_TEXT
  Exch 2
  Exch $2      ; SOURCE_FILE
 
  Push $R0
  Push $R1
  Push $R2
  Push $R3
  Push $R4
 
  IfFileExists $2 +1 RIF_error
  FileOpen $R0 $2 "r"
 
  GetTempFileName $R2
  FileOpen $R1 $R2 "w"
 
  RIF_loop:
    FileRead $R0 $R3
    IfErrors RIF_leaveloop
    #RIF_sar:
      Push "$R3"
      Push "$1"
      Push "$0"
      Call StrRepLocal
      StrCpy $R4 "$R3"
      Pop $R3
      #StrCmp "$R3" "$R4" +1 RIF_sar
    FileWrite $R1 "$R3"
  Goto RIF_loop
 
  RIF_leaveloop:
    FileClose $R1
    FileClose $R0
 
    Delete "$2" 
    Rename "$R2" "$2"
 
    ClearErrors
    Goto RIF_out
 
  RIF_error:
    SetErrors
 
  RIF_out:
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Pop $R0
  Pop $2
  Pop $0
  Pop $1
 
FunctionEnd


!macro ReplaceInFile SOURCE_FILE SEARCH_TEXT REPLACEMENT
  Push "${SOURCE_FILE}"
  Push "${SEARCH_TEXT}"
  Push "${REPLACEMENT}"
  Call ReplaceInFileFunction
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
  WriteRegStr HKCU "Software\${VENDOR}\YARP\${INST2}" "" "$INSTDIR\${INST2}"
  WriteRegStr HKCU "Software\${VENDOR}\YARP\Common" "LastInstallLocation" $INSTDIR
  WriteRegStr HKCU "Software\${VENDOR}\YARP\Common" "LastInstallVersion" ${INST2}
  WriteRegStr HKCU "Software\${VENDOR}\GSL\${GSL_INST2}" "" "$INSTDIR\${GSL_INST2}"
  WriteRegStr HKCU "Software\${VENDOR}\GSL\Common" "LastInstallLocation" $INSTDIR
  WriteRegStr HKCU "Software\${VENDOR}\GSL\Common" "LastInstallVersion" ${GSL_INST2}
  
  WriteUninstaller "$INSTDIR\Uninstall_YARP.exe"
  SectionIn RO
  !include ${NSIS_OUTPUT_PATH}\yarp_base_add.nsi
  # ${StrRepLocal} $0 "$INSTDIR" "\" "/"
  # !insertmacro ReplaceInFile "$INSTDIR\YARPConfig.cmake" "${YARP_ORG_DIR}/install" "$0"
  # !insertmacro ReplaceInFile "$INSTDIR\${INST2}\lib\${YARP_LIB_DIR}\${YARP_LIB_FILE}" "${ACE_ORG_DIR}" "$0"
SectionEnd

Section "Command-line utilities" SecPrograms
  SetOutPath "$INSTDIR"
  !include ${NSIS_OUTPUT_PATH}\yarp_programs_add.nsi
SectionEnd

SectionGroup "Compile environment" SecDevelopment

  Section "Libraries" SecLibraries
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_libraries_add.nsi
  SectionEnd

  Section "Header files" SecHeaders
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_headers_add.nsi
  SectionEnd

  Section "Examples" SecExamples
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_examples_add.nsi
  SectionEnd

SectionGroupEnd

SectionGroup "YARP Runtime" SecRuntime

  Section "YARP DLLs" SecDLLs
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_dlls_add.nsi
  SectionEnd
	
  Section "Visual Studio DLLs (nonfree)" SecVcDlls
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_vc_dlls_add.nsi
  SectionEnd

  SectionGroup "ACE library" SecAce
    Section "ACE library" SecAceLibraries
      SetOutPath "$INSTDIR"
      !include ${NSIS_OUTPUT_PATH}\yarp_ace_libraries_add.nsi
    SectionEnd

    Section "ACE DLL" SecAceDLLs
      SetOutPath "$INSTDIR"
      !include ${NSIS_OUTPUT_PATH}\yarp_ace_dlls_add.nsi
    SectionEnd
  SectionGroupEnd
  
SectionGroupEnd

SectionGroup "Math library (GPL)" SecMath
  Section "Math libraries" SecMathLibraries
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_math_libraries_add.nsi
  SectionEnd

  Section "Math DLLs" SecMathDLLs
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_math_dlls_add.nsi
  SectionEnd

  Section "Math headers" SecMathHeaders
    SetOutPath "$INSTDIR"
    #CreateDirectory "$INSTDIR\include"
    !include ${NSIS_OUTPUT_PATH}\yarp_math_headers_add.nsi
  SectionEnd
SectionGroupEnd

Section "yarpview utility" SecGuis
  #CreateDirectory "$INSTDIR\yarpview"
  SetOutPath "$INSTDIR"
  !include ${NSIS_OUTPUT_PATH}\yarp_guis_add.nsi
  #CreateShortCut "$INSTDIR\bin\yarpview.lnk" "$INSTDIR\yarpview\yarpview.exe"
SectionEnd

Section "${DBG_HIDE}Debug versions" SecDebug
  SetOutPath "$INSTDIR"
  !include ${NSIS_OUTPUT_PATH}\yarp_debug_add.nsi
  # ${StrRepLocal} $0 "$INSTDIR" "\" "/"
  # !insertmacro ReplaceInFile "$INSTDIR\${INST2}\lib\${YARP_LIB_DIR_DBG}\${YARP_LIB_FILE_DBG}" "${ACE_ORG_DIR_DBG}" "$0"
SectionEnd

!ifndef WriteEnvStr_Base
  !ifdef ALL_USERS
    !define WriteEnvStr_Base "HKLM"
    !define WriteEnvStr_RegKey \
       'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
  !else
    !define WriteEnvStr_Base "HKCU"
    !define WriteEnvStr_RegKey 'HKCU "Environment"'
  !endif
!endif

Section "Environment variables" SecPath
  ${EnvVarUpdate} $0 "PATH" "A" "${WriteEnvStr_Base}" "$INSTDIR\${INST2}\bin"
  ${EnvVarUpdate} $0 "LIB" "A" "${WriteEnvStr_Base}" "$INSTDIR\${INST2}\lib"
  ${EnvVarUpdate} $0 "INCLUDE" "A" "${WriteEnvStr_Base}" "$INSTDIR\${INST2}\include"
  WriteRegExpandStr ${WriteEnvStr_RegKey} YARP_DIR "$INSTDIR\${INST2}"
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
SectionEnd

Section "-last"
  !insertmacro SectionFlagIsSet ${SecMath} ${SF_PSELECTED} isSel chkAll
   chkAll:
     !insertmacro SectionFlagIsSet ${SecMath} ${SF_SELECTED} isSel notSel
   notSel:
     !insertmacro ReplaceInFile "$INSTDIR\${INST2}\cmake\YARPConfig.cmake" "YARP_math;" ""
	 !insertmacro ReplaceInFile "$INSTDIR\${INST2}\cmake\YARPConfig.cmake" "YARP_HAS_MATH_LIB TRUE" "YARP_HAS_MATH_LIB FALSE"
   isSel:
SectionEnd

;--------------------------------
;Descriptions

;Language strings
LangString DESC_SecPrograms ${LANG_ENGLISH} "YARP tools, including the standard YARP companion, and the standard YARP name server."
LangString DESC_SecDevelopment ${LANG_ENGLISH} "Files needed for compiling against YARP."
LangString DESC_SecLibraries ${LANG_ENGLISH} "Libraries for linking against YARP."
LangString DESC_SecHeaders ${LANG_ENGLISH} "Core YARP header files."
LangString DESC_SecExamples ${LANG_ENGLISH} "Some basic examples of using YARP.  See online documentation, and many more examples in source code package."
LangString DESC_SecRuntime ${LANG_ENGLISH} "Files needed for running YARP programs."
LangString DESC_SecDLLs ${LANG_ENGLISH} "Libraries needed for YARP programs to run."
LangString DESC_SecVcDlls ${LANG_ENGLISH} "Visual Studio runtime redistributable files.  Not free software.  If you already have Visual Studio installed, you may want to skip this."
LangString DESC_SecAce ${LANG_ENGLISH} "The Adaptive Communications Environment, used by this version of YARP."
LangString DESC_SecAceLibraries ${LANG_ENGLISH} "ACE library files."
LangString DESC_SecAceDLLs ${LANG_ENGLISH} "ACE library run-time."
LangString DESC_SecMath ${LANG_ENGLISH} "The YARP math library.  Based on the GNU Scientific Library.  This is therefore GPL software, not the LGPL like YARP."
LangString DESC_SecMathLibraries ${LANG_ENGLISH} "Math library files."
LangString DESC_SecMathDLLs ${LANG_ENGLISH} "Math library run-time."
LangString DESC_SecMathHeaders ${LANG_ENGLISH} "Math library header files."
LangString DESC_SecGuis ${LANG_ENGLISH} "Utility for viewing image streams.  Uses GTK+."
LangString DESC_SecPath ${LANG_ENGLISH} "Add YARP to PATH, LIB, and INCLUDE variables, and set YARP_DIR variable."
LangString DESC_SecDebug ${LANG_ENGLISH} "Debug versions of the YARP and ACE libraries."

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  ; !insertmacro MUI_DESCRIPTION_TEXT ${SecBase} $(DESC_SecBase)
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
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMath} $(DESC_SecMath)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMathLibraries} $(DESC_SecMathLibraries)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMathDLLs} $(DESC_SecMathDLLs)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMathHeaders} $(DESC_SecMathHeaders)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecGuis} $(DESC_SecGuis)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPath} $(DESC_SecPath)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDebug} $(DESC_SecDebug)
  #!insertmacro MUI_DESCRIPTION_TEXT ${Sec} $(DESC_Sec)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
  ${un.EnvVarUpdate} $0 "PATH" "R" "${WriteEnvStr_Base}" "$INSTDIR\${INST2}\bin"
  ${un.EnvVarUpdate} $0 "LIB" "R" "${WriteEnvStr_Base}" "$INSTDIR\${INST2}\lib"
  ${un.EnvVarUpdate} $0 "INCLUDE" "R" "${WriteEnvStr_Base}" "$INSTDIR\${INST2}\include"

  #Push "$INSTDIR\bin"
  #Call un.RemoveFromPath
  #Push "LIB"
  #Push "$INSTDIR\lib"
  #Call un.RemoveFromEnvVar
  DeleteRegValue ${WriteEnvStr_RegKey} YARP_DIR
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000

  !include ${NSIS_OUTPUT_PATH}\yarp_base_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_libraries_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_dlls_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_programs_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_headers_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_examples_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_math_libraries_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_math_dlls_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_math_headers_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_ace_libraries_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_ace_dlls_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_vc_dlls_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_guis_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_debug_remove.nsi
  #Delete "$INSTDIR\bin\yarpview.lnk"
  
  Delete "$INSTDIR\Uninstall_YARP.exe"

  RMDir /r "$INSTDIR\${INST2}\bin"
  RMDir /r "$INSTDIR\${INST2}\lib"
  #RMDir "$INSTDIR\yarpview"
  RMDir /r "$INSTDIR\${INST2}\include"
  RMDir "$INSTDIR\${INST2}\example"
  RMDir /r "$INSTDIR\${INST2}"
  RMDir /r "$INSTDIR\${GSL_INST2}"

  # cleanup YARP registry entries
  DeleteRegKey HKCU "Software\${VENDOR}\YARP\Common\LastInstallLocation"
  DeleteRegKey HKCU "Software\${VENDOR}\YARP\Common\LastInstallVersion"
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\YARP\Common"
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\YARP\${INST2}"
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\YARP"
  
  # cleanup GSL registry entries
  DeleteRegKey HKCU "Software\${VENDOR}\GSL\Common\LastInstallLocation"
  DeleteRegKey HKCU "Software\${VENDOR}\GSL\Common\LastInstallVersion"  
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\GSL\Common"
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\GSL\${GSL_INST2}"
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}\GSL"

  # cleanup vendor entry if empty
  DeleteRegKey /ifempty HKCU "Software\${VENDOR}"
  
SectionEnd

Function .onInit
  !insertmacro MULTIUSER_INIT
FunctionEnd

Function un.onInit
  !insertmacro MULTIUSER_UNINIT
FunctionEnd
