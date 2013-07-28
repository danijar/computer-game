@echo off
@Are you sure?
pause

cd Application
for /r %%i in (*) do ( if %%~xi==.cpp ( del %%i ) )
for /r %%i in (*) do ( if %%~xi==.h   ( del %%i ) )
for /f "usebackq" %%d in (`"dir /ad/b/s | sort /R"`) do rd "%%d"
