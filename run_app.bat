@echo off
set PATH=C:\msys64\mingw64\bin;C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64;%PATH%
echo Launching ParallelVisionSuite...
.\build\ParallelVisionSuite.exe
pause