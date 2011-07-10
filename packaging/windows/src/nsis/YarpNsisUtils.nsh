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

!ifndef WriteEnvStr_Base
  !ifdef ALL_USERS
    !define WriteEnvStr_Base "HKLM"
    !define WriteEnvStr_RegKey \
       'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
    !define WriteEnvStr_KeyOnly \
       "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
  !else
    !define WriteEnvStr_Base "HKCU"
    !define WriteEnvStr_RegKey 'HKCU "Environment"'
    !define WriteEnvStr_KeyOnly "Environment"
  !endif
!endif

!macro SplitFirstStrPart un
Function ${un}SplitFirstStrPart
  Exch $R0
  Exch
  Exch $R1
  Push $R2
  Push $R3
  StrCpy $R3 $R1
  StrLen $R1 $R0
  IntOp $R1 $R1 + 1
  loop:
    IntOp $R1 $R1 - 1
    StrCpy $R2 $R0 1 -$R1
    StrCmp $R1 0 exit0
    StrCmp $R2 $R3 exit1 loop
  exit0:
  StrCpy $R1 ""
  Goto exit2
  exit1:
    IntOp $R1 $R1 - 1
    StrCmp $R1 0 0 +3
     StrCpy $R2 ""
     Goto +2
    StrCpy $R2 $R0 "" -$R1
    IntOp $R1 $R1 + 1
    StrCpy $R0 $R0 -$R1
    StrCpy $R1 $R2
  exit2:
  Pop $R3
  Pop $R2
  Exch $R1 ;rest
  Exch
  Exch $R0 ;first
FunctionEnd
!macroend
!insertmacro SplitFirstStrPart ""
!insertmacro SplitFirstStrPart "un."


Function RemoveOldEnv
  Exch $R0 ; $0
  Exch
  Exch $R1 ; PATH
  Exch 2
  Exch $R2 ; robotology_PATH content
  
  Push $R3
  Push $R4
  
  StrCpy $R3 $R2
  
  looping:
  StrCmp $R3 "" im_done
  Push ";"
  Push $R3
  Call SplitFirstStrPart
  Pop $R4
  Pop $R3
  ;; DetailPrint "RemoveOldEnv / $R3 / $R4"
  StrCmp $R4 "" im_done
  ${EnvVarUpdate} $R0 $R1 "R" "${WriteEnvStr_Base}" "$R4"
  Goto looping
  
  im_done:
  Pop $R4
  Pop $R3
  
  Exch $R2
  Exch 2
  Exch $R1
  Exch
  Exch $R0
FunctionEnd

!macro SPLIT_STRING INPUT PART
  Push $R0
  Push $R1
  Push $R2
 
  StrCpy 	$R0 -1
  IntOp  	$R1 ${PART} * 2
  IntOp  	$R1 $R1 - 1
 
findStart_loop_${PART}:						
 
  IntOp  	$R0 $R0 + 1
  StrCpy	$R2 ${INPUT} 1 $R0
  StrCmp 	$R2 "" error_${PART}
  StrCmp 	$R2 '"' 0 findStart_loop_${PART}
 
  IntOp 	$R1 $R1 - 1
  IntCmp 	$R1 0 0 0 findStart_loop_${PART}		
 
  IntOp 	$R1 $R0 + 1
 
findEnd_loop_${PART}:						
  IntOp  	$R0 $R0 + 1
  StrCpy	$R2 ${INPUT} 1 $R0
  StrCmp 	$R2 "" error_${PART}
  StrCmp 	$R2 '"' 0 findEnd_loop_${PART}
 
  IntOp 	$R0 $R0 - $R1					
  StrCpy 	$R0 ${INPUT} $R0 $R1
  Goto 		done_${PART}
 
 
error_${PART}:
  StrCpy 	$R0 error
 
 
done_${PART}:
  Pop 		$R2
  Pop 		$R1
  Exch 		$R0
!macroend


!macro RemoveInventory un
Function ${un}RemoveInventory
  Exch $R8 ; $PATH
  Exch
  Exch $R9 ; $0

  IfFileExists "$R8" +2
    Goto RemoveInventoryDone

  Push $R0
  Push $R1
  Push $R2
  Push $R3
  Push $R4
  Push $R5
  Push $R6
  SetFileAttributes "$R8" NORMAL
  FileOpen $UninstLog "$R8" r
  StrCpy $R1 -1
 
  GetLineCount:
    ClearErrors
    FileRead $UninstLog $R0
    IntOp $R1 $R1 + 1
    StrCpy $R0 $R0 -2
	DetailPrint "Push |$R0|"
    Push $R0   
    IfErrors 0 GetLineCount
 
  Pop $R0
 
  LoopRead:
    StrCmp $R1 0 LoopDone
    Pop $R0
	Push "|"
	Push $R0
    Call ${un}SplitFirstStrPart
    Pop $R3 ;1st part
    Pop $R0 ;rest
	Push "|"
	Push $R0
    Call ${un}SplitFirstStrPart
    Pop $R4 ;2st part
    Pop $R0 ;rest
	Push "|"
	Push $R0
    Call ${un}SplitFirstStrPart
    Pop $R5 ;3rd part
    Pop $R0 ;rest
	Push "|"
	Push $R0
    Call ${un}SplitFirstStrPart
    Pop $R6 ;4th part
    Pop $R0 ;rest
	Push "|"
	Push $R0
    Call ${un}SplitFirstStrPart
    Pop $R7 ;5th part
    Pop $R0 ;rest
	# $R3|$R4|$R5|$R6|$R7
	# ENVADD|$R4=base|$R5=key|$R6=val
	# KEYSET|$R4=base|$R5=key|$R6=subkey|$R7=val
    StrCmp $R3 "ENVADD" EnvAdd
    StrCmp $R3 "KEYSET" KeySet
	Goto UnknownOp
	EnvAdd:
	  DetailPrint "Remove $R6 from $R5 in $R4"
	  ${${un}EnvVarUpdate} $R9 $R5 "R" "$R4" "$R6"
	KeySet:
	  Push $R0
	  DetailPrint "Delete key $R4 $R5 $R6 if it is $R7"
	  ; wow, NSIS is messed up
	  ${If} $R4 == HKLM
        ReadRegStr $R0 HKLM "$R5" "$R6"
      ${Else}
        ReadRegStr $R0 HKCU "$R5" "$R6"
	  ${Endif}
	  StrCmp $R0 $R7 0 SkipDel
	    ${If} $R4 == HKLM
          DeleteRegValue HKLM "$R5" "$R6"
        ${Else}
          DeleteRegValue HKCU "$R5" "$R6"
  	    ${Endif}
	  SkipDel:
	    Pop $R0
	UnknownOp:
    IntOp $R1 $R1 - 1
    Goto LoopRead
  LoopDone:
  FileClose $UninstLog
  Pop $R6
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Pop $R0

RemoveInventoryDone:
  Exch $R8 ; PATH
  Exch
  Exch $R9 ; $0
FunctionEnd
!macroend
!insertmacro RemoveInventory ""
!insertmacro RemoveInventory "un."
