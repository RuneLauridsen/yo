:: setup build directory
@echo off
if not exist build mkdir build
pushd build
setlocal

:: common flags
set compiler_flags=/Zi /nologo
set linker_flags=/INCREMENTAL:NO

:: yo
cl %compiler_flags% /Fe:yo.exe ..\yo\main.c             /link %linker_flags% /INCREMENTAL:NO /SUBSYSTEM:CONSOLE

:: yo_dll
cl %compiler_flags% /Fe:yo_dll.dll /LD ..\yo_dll\main.c /link %linker_flags% /INCREMENTAL:NO

:: yo_csharp
echo f | xcopy /y yo_dll.dll "../yo_csharp/yo_dll.dll"
dotnet build ../yo_csharp/yo_csharp.csproj -o .

endlocal
popd