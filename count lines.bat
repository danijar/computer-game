@echo off

cd Application

set sum = 0
set files = 0
for /r %%i in (*) do (
    set code=

    if %%~xi == .cpp  set code=true
    if %%~xi == .h    set code=true
    if %%~xi == .vert set code=true
    if %%~xi == .frag set code=true
    if %%~xi == .js   set code=true

    if defined code (
        for /f %%j in ('Find /V /C "" ^< %%i') do set lines=%%j
        set /a sum = "sum + lines"
        set /a files = "files + 1"
    )
)

echo There are %sum% lines of code in %files% files.
pause