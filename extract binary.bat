@echo off

IF exist distribution (
    cd distribution
    del /q .
    cd ..
)

xcopy %~dp0 distribution /s /e /i /y
cd distribution

del *.exp
del *.lib
del *.pdb

for /r %%i in (*) do (if %%~xi==.cpp (del %%i))
for /r %%i in (*) do (if %%~xi==.h   (del %%i))
for /f "usebackq" %%d in (`"dir /ad/b/s | sort /R"`) do rd "%%d"
