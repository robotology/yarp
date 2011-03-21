
!include "MUI2.nsh"

Name "YARP ${YARP_VERSION}"
OutFile "${NSIS_OUTPUT_PATH}\yarp_core_${YARP_VERSION}_${BUILD_VERSION}.exe"
InstallDir "$PROGRAMFILES\yarp\yarp-${YARP_VERSION}"
InstallDirRegKey HKCU "Software\YARP" ""
RequestExecutionLevel admin

!define MUI_ABORTWARNING

;--------------------------------
;Utilities

!define StrRep "!insertmacro StrRep"
!macro StrRep output string old new
    Push "${string}"
    Push "${old}"
    Push "${new}"
    !ifdef __UNINSTALL__
        Call un.StrRep
    !else
        Call StrRep
    !endif
    Pop ${output}
!macroend
 
!macro Func_StrRep un
    Function ${un}StrRep
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
            IntOp $R3 $R3 + 1 ;move offset by 1 to check the next character
            Goto loop
        found:
            StrCpy $R5 $R0 $R3
            IntOp $R8 $R3 + $R4
            StrCpy $R7 $R0 "" $R8
            StrCpy $R0 $R5$R2$R7
            StrLen $R6 $R0
            IntOp $R3 $R3 + $R9 ;move offset by length of the replacement string
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
!insertmacro Func_StrRep ""

Function ReplaceInFileFunction
 
  ClearErrors  ; want to be a newborn
 
  Exch $0      ; REPLACEMENT
  Exch
  Exch $1      ; SEARCH_TEXT
  Exch 2
  Exch $2      ; SOURCE_FILE
 
  Push $R0     ; SOURCE_FILE file handle
  Push $R1     ; temporary file handle
  Push $R2     ; unique temporary file name
  Push $R3     ; a line to sar/save
  Push $R4     ; shift puffer
 
  IfFileExists $2 +1 RIF_error      ; knock-knock
  FileOpen $R0 $2 "r"               ; open the door
 
  GetTempFileName $R2               ; who's new?
  FileOpen $R1 $R2 "w"              ; the escape, please!
 
  RIF_loop:                         ; round'n'round we go
    FileRead $R0 $R3                ; read one line
    IfErrors RIF_leaveloop          ; enough is enough
    RIF_sar:                        ; sar - search and replace
      Push "$R3"                    ; (hair)stack
      Push "$1"                     ; needle
      Push "$0"                     ; blood
      Call StrRep                  ; do the bartwalk
      StrCpy $R4 "$R3"              ; remember previous state
      Pop $R3                       ; gimme s.th. back in return!
      StrCmp "$R3" "$R4" +1 RIF_sar ; loop, might change again!
    FileWrite $R1 "$R3"             ; save the newbie
  Goto RIF_loop                     ; gimme more
 
  RIF_leaveloop:                    ; over'n'out, Sir!
    FileClose $R1                   ; S'rry, Ma'am - clos'n now
    FileClose $R0                   ; me 2
 
    Delete "$2.old"                 ; go away, Sire
    Rename "$2" "$2.old"            ; step aside, Ma'am
    Rename "$R2" "$2"               ; hi, baby!
 
    ClearErrors                     ; now i AM a newborn
    Goto RIF_out                    ; out'n'away
 
  RIF_error:                        ; ups - s.th. went wrong...
    SetErrors                       ; ...so cry, boy!
 
  RIF_out:                          ; your wardrobe?
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
  !include ${NSIS_OUTPUT_PATH}\yarp_base_add.nsi
  ${StrRep} $0 "$INSTDIR" "\" "/"
  !insertmacro ReplaceInFile "YARPConfig.cmake" "${YARP_ORG_DIR}/install" "$0"
SectionEnd

Section "Libraries for compiling" SecLibraries
  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\lib"
  !include ${NSIS_OUTPUT_PATH}\yarp_libraries_add.nsi
SectionEnd

Section "Runtime libraries (DLLs)" SecDLLs
  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\bin"
  !include ${NSIS_OUTPUT_PATH}\yarp_dlls_add.nsi
SectionEnd

Section "Programs" SecPrograms
  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\bin"
  !include ${NSIS_OUTPUT_PATH}\yarp_programs_add.nsi
SectionEnd

Section "Header files" SecHeaders
  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\include"
  !include ${NSIS_OUTPUT_PATH}\yarp_headers_add.nsi
SectionEnd

Section "Visual Studio redistributable runtime (DLLs)" SecVcDlls
  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\bin"
  CreateDirectory "$INSTDIR\yarpview"
  !include ${NSIS_OUTPUT_PATH}\yarp_vc_dlls_add.nsi
SectionEnd

SectionGroup "Math library"
  Section "Math libraries for compiling" SecMathLibraries
    SetOutPath "$INSTDIR"
    CreateDirectory "$INSTDIR\lib"
    !include ${NSIS_OUTPUT_PATH}\yarp_math_libraries_add.nsi
  SectionEnd

  Section "Math runtime libraries(DLLs)" SecMathDLLs
    SetOutPath "$INSTDIR"
    CreateDirectory "$INSTDIR\bin"
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
  CreateDirectory "$INSTDIR\yarpview"
  SetOutPath "$INSTDIR"
  !include ${NSIS_OUTPUT_PATH}\yarp_guis_add.nsi
  #CreateShortCut "$INSTDIR\bin\yarpview.lnk" "$INSTDIR\yarpview\yarpview.exe"
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

  !include ${NSIS_OUTPUT_PATH}\yarp_base_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_libraries_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_dlls_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_programs_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_headers_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_math_libraries_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_math_dlls_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_ace_libraries_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_ace_dlls_remove.nsi
  !include ${NSIS_OUTPUT_PATH}\yarp_vc_dlls_remove.nsi

  !include ${NSIS_OUTPUT_PATH}\yarp_guis_remove.nsi
  #Delete "$INSTDIR\bin\yarpview.lnk"
  
  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR\lib"
  #RMDir "$INSTDIR\yarpview"
  RMDir /r "$INSTDIR\include"
  RMDir "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\YARP"

SectionEnd
