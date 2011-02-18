@echo off

 if not exist %1 goto :missing
 echo call %1
 call %1

 echo cygpath --unix --path "%PATH%" > msbuild_path.sh
 rem echo %INCLUDE%
 rem echo %LIB%
 
 goto :eof

 :missing
 echo Missing file
 echo %1
 goto :eof
 