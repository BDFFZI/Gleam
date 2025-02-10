@echo off
@cd /d %~dp0

set PackTarget=%1
set PackType=%2
set PackPath=%3
if "%PackPath%"=="" set PackPath="%~dp0..\GleamPack"
if "%PackType%"=="" set PackType=Debug

if "%PackTarget%"=="" (
    echo "���棡δ�ڲ�����ָ��Ŀ����Ŀ����Ĭ��ʹ�á�HelloGleam����Ŀ����ʾ������ܡ�"
    pause
    set PackTarget="HelloGleam"
)

cmake %~dp0 -DPackTarget=%PackTarget% -DPackPath=%PackPath% -DPackType=%PackType% --preset=default

mkdir "../GleamBuild/."
cd "../GleamBuild/."
cmake -E rm -fr ./output/%PackTarget%
cmake --build . --config %PackType% --target=pack 

pause