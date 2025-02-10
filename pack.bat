set PackTarget=%1
set PackPath=%2
set PackType=%3
if "%PackPath%"=="" set PackPath="%~dp0..\LightPack"
if "%PackType%"=="" set PackType=Debug

cd ../LightBuild/.
cmake ../Light/. -DPackTarget=%PackTarget% -DPackType=%PackType%
cmake --build . --config %PackType% --target=pack 
