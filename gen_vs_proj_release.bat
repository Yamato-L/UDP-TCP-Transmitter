@echo off
set CurrentPath=%cd%
cd %CurrentPath%

if not exist build md build
cd build
cmake .. -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_BUILD_NAME=SimOneAPI
cd ..

pause