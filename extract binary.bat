@echo off

IF exist Distribution (
    cd Distribution
    del /q .
    cd ..
)

xcopy Application Distribution /s /e /i /y
cd Distribution

del *.exp
del *.lib
del *.pdb

for /r %%i in (*) do (if %%~xi==.cpp (del %%i))
for /r %%i in (*) do (if %%~xi==.h   (del %%i))
for /f "usebackq" %%d in (`"dir /ad/b/s | sort /R"`) do rd "%%d"
