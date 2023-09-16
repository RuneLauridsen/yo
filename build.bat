:: setup build directory
@echo off
if not exist build mkdir build
pushd build
setlocal

:: common flags
set compiler_flags=/Zi /nologo
set linker_flags=/INCREMENTAL:NO

:: freetype
set INCLUDE=%INCLUDE%;..\yo\thirdparty\freetype\include
if %VSCMD_ARG_TGT_ARCH%==x64 set LIB=%LIB%;..\yo\thirdparty\freetype\lib\x64
if %VSCMD_ARG_TGT_ARCH%==x86 set LIB=%LIB%;..\yo\thirdparty\freetype\lib\x86

:: yo
cl %compiler_flags% /Fe:yo.exe ..\yo\main.c ..\yo\thirdparty\tracy\TracyClient.Cpp   /link %linker_flags% /SUBSYSTEM:WINDOWS

:: yo_dll
cl %compiler_flags% /Fe:yo_dll.dll /LD ..\yo_dll\main.c                              /link %linker_flags%

:: yo_csharp
echo f | xcopy /y yo_dll.dll "../yo_csharp/yo_dll.dll"
echo f | xcopy /y yo_dll.dll "../yo_csharp/yo_dll.pdb"
dotnet build ../yo_csharp/yo_csharp.csproj -o .

endlocal
popd