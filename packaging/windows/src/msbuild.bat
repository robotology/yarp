@echo off

 if not exist %1 goto :missing
 echo call %1 %2
 call %1 %2

 echo cygpath --unix --path "%PATH%" > msbuild_path.sh
 echo echo "%INCLUDE%" > msbuild_include.sh
 echo echo "%LIB%" > msbuild_lib.sh
 goto :eof

 :missing
 echo Missing file
 echo %1
 goto :eof
 