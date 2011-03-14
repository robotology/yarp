
!include "MUI2.nsh"

Name "YARP ${YARP_VERSION}"
OutFile "${NSIS_OUTPUT_PATH}\yarp_core_${YARP_VERSION}_${BUILD_VERSION}.exe"
InstallDir "$PROGRAMFILES\yarp\yarp-${YARP_VERSION}"
InstallDirRegKey HKCU "Software\YARP" ""
RequestExecutionLevel admin
  
!define MUI_ABORTWARNING

;--------------------------------
;Pages

  !define MUI_PAGE_HEADER_TEXT "Welcome to YARP, Yet Another Robot Platform"
  !define MUI_PAGE_HEADER_SUBTEXT "YARP is free software"
  !define MUI_LICENSEPAGE_TEXT_TOP "We grant you lots of freedoms under this Free Software license."
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

Section "Base" SecBase
  SetOutPath "$INSTDIR"
  WriteRegStr HKCU "Software\YARP" "" $INSTDIR
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  SectionIn RO
SectionEnd

Section "Libraries for compiling" SecLibraries
  SetOutPath "$INSTDIR"
    ;CreateDirectory "$INSTDIR\lib"
  !include ${NSIS_OUTPUT_PATH}\yarp_libraries_add.nsi
SectionEnd

Section "Runtime libraries (DLLs)" SecDLLs
  SetOutPath "$INSTDIR"
    ;CreateDirectory "$INSTDIR\bin"
  !include ${NSIS_OUTPUT_PATH}\yarp_dlls_add.nsi
SectionEnd

Section "Visual Studio redistributable runtime (DLLs)" SecVcDlls
  SetOutPath "$INSTDIR"
  !include ${NSIS_OUTPUT_PATH}\yarp_vc_dlls_add.nsi
SectionEnd

Section "Programs" SecPrograms
  SetOutPath "$INSTDIR"
    ;CreateDirectory "$INSTDIR\bin"
  !include ${NSIS_OUTPUT_PATH}\yarp_programs_add.nsi
SectionEnd

SectionGroup "Math library"
  Section "Math libraries for compiling" SecMathLibraries
    SetOutPath "$INSTDIR"
    CreateDirectory "$INSTDIR\lib"
    !include ${NSIS_OUTPUT_PATH}\yarp_math_libraries_add.nsi
  SectionEnd

  Section "Math runtime libraries(DLLs)" SecMathDLLs
    SetOutPath "$INSTDIR"
    ;CreateDirectory "$INSTDIR\bin"
    !include ${NSIS_OUTPUT_PATH}\yarp_math_dlls_add.nsi
  SectionEnd
SectionGroupEnd

SectionGroup "ACE library"
  Section "ACE libraries for compiling" SecAceLibraries
    SetOutPath "$INSTDIR"
    CreateDirectory "$INSTDIR\lib"
    !include ${NSIS_OUTPUT_PATH}\yarp_ace_libraries_add.nsi
  SectionEnd

  Section "ACE runtime library(DLL)" SecAceDLLs
    SetOutPath "$INSTDIR"
    !include ${NSIS_OUTPUT_PATH}\yarp_ace_dlls_add.nsi
  SectionEnd
SectionGroupEnd

Section "yarpview" SecGuis
  ;CreateDirectory "$INSTDIR\yarpview"
  SetOutPath "$INSTDIR"
  !include ${NSIS_OUTPUT_PATH}\yarp_guis_add.nsi
  CreateShortCut "$INSTDIR\bin\yarpview.lnk" "$INSTDIR\yarpview\yarpview.exe"
SectionEnd

;--------------------------------
;Descriptions

;Language strings
LangString DESC_SecBase ${LANG_ENGLISH} "Install an uninstaller!"
LangString DESC_SecLibraries ${LANG_ENGLISH} "Files required for linking against YARP."
LangString DESC_SecDLLs ${LANG_ENGLISH} "Files required for running YARP."
LangString DESC_SecPrograms ${LANG_ENGLISH} "YARP utility programs."

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecBase} $(DESC_SecBase)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecLibraries} $(DESC_SecLibraries)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDLLs} $(DESC_SecDLLs)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPrograms} $(DESC_SecPrograms)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  !include ${NSIS_OUTPUT_PATH}\yarp_libraries_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_dlls_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_programs_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_math_libraries_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_math_dlls_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_ace_libraries_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_ace_dlls_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_vc_dlls_remove.nsi

  !include ${NSIS_OUTPUT_PATH}\yarp_guis_remove.nsi
  Delete "$INSTDIR\bin\yarpview.lnk"
  
  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR\lib"
  RMDir "$INSTDIR\yarpview"
  RMDir "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\YARP"

SectionEnd
