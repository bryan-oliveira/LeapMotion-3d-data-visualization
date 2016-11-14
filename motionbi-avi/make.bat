@echo off
setlocal
set PATH=C:\MinGW\bin;C:\MinGWlibs\bin;%PATH%

set C_INCLUDE_PATH=C:\MinGWlibs\include
set CPLUS_INCLUDE_PATH=C:\MinGWlibs\include
set LIBRARY_PATH=C:\MinGWlibs\lib

mingw32-make

if ERRORLEVEL 1 goto exit
bin\gprs.exe

:exit
pause
