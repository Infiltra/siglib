@echo off

rem Batch file for creating Perl wrapper for accessing SigLib library
rem Ensure swig.exe is in your system PATH

rem Delete the wrapper c file so that if it is not regenerated then the compile will fail
del siglib_wrap_wrap.c

copy typemaps.c#.i typemaps.i /Y
copy siglib_wrap.c#.i siglib_wrap.i /Y

swig -Wall -csharp -I%SIGLIB_PATH%\include -includeall -namespace siglib_csharp siglib_wrap.i
rem swig -Wall -csharp -dllimport siglibglue -I%SIGLIB_PATH%\include -includeall -namespace siglib_csharp siglib_wrap.i

cl -Od -MDd -DNDEBUG -DWIN32 -D_CONSOLE -DSIGLIB_STATIC_LIB=1 -DNO_STRICT siglib_wrap_wrap.c /link user32.lib %SIGLIB_PATH%\lib\Microsoft\static_library_64\Debug\siglib.lib /DLL /out:siglib_wrap.dll

csc.exe /unsafe /out:siglib_csharp.dll /target:library *.cs

copy *.dll C:\WINDOWS\system32 /Y

