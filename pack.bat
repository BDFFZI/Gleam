@echo off

set PackTarget=%1
set PackType=%2
set PackPath=%3
if "%PackPath%"=="" set PackPath="%~dp0..\GleamPack"
if "%PackType%"=="" set PackType=Debug

if "%PackTarget%"=="" (
    echo 警告！未在参数中指明目标项目，将默认使用“HelloGleam”项目来演示打包功能。
    pause
    set PackTarget="HelloGleam"
)

cd ../GleamBuild/.
cmake -E rm -fr ./output/%PackTarget%
cmake ../Gleam/. -DPackTarget=%PackTarget% -DPackPath=%PackPath% -DPackType=%PackType%
cmake --build . --config %PackType% --target=pack 

pause