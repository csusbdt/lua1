@echo off
rem del /q a.exe
del /q working\a.exe

cl /nologo /TC /D _WINDOWS /I lua /I include /I src src\*.c lua\*.c /MD /link win\*.lib  /SUBSYSTEM:CONSOLE /OUT:working/a.exe

if not exist working\a.exe goto fail

del /q *.obj
cd working
if not exist SDL2.dll          copy ..\win\SDL2.dll .
if not exist SDL2_ttf.dll      copy ..\win\SDL2_ttf.dll .
if not exist libfreetype-6.dll copy ..\win\libfreetype-6.dll .
if not exist zlib1.dll         copy ..\win\zlib1.dll .
if not exist libpng16-16.dll   copy ..\win\libpng16-16.dll .
a.exe
goto end

:fail
del /q *.obj
echo Failed to compile.
goto end

:end
