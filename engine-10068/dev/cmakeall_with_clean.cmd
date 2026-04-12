@echo off

set /p head_branch=<"../.git/HEAD"

for /f "tokens=1,2,3,* delims=/" %%a in ("%head_branch%") do (set engine_branch=%%c)

echo engine_branch=%engine_branch%

set core_branch=core_old_%engine_branch%
echo core_branch=%core_branch%

if not exist ..\engine-core md ..\engine-core && git clone -b %core_branch% http://gitlab001.sandboxol.cn/game/blockmango-engine.git ..\engine-core
cd ..\engine-core
git checkout %core_branch%
git pull
cd dev

for /d %%a in (*) do if exist %%a\CMakeLists.txt (
  if exist %%a\Build  rd /q /s %%a\Build
)

where "C:\Program Files\CMake\bin\;C:\Program Files (x86)\CMake\bin\;%PATH%:cmake.exe" > tmp
set /P cmake=<tmp
del tmp
for /d %%a in (*) do if exist %%a\CMakeLists.txt (
  echo build %%a ...
  if not exist %%a\Build md %%a\Build
  cd %%a\Build
  "%cmake%" -D CMAKE_GENERATOR_PLATFORM:INTERNAL=Win32 -A Win32 ..\
  cd ..\..
  echo .
)

cd ..\..\dev

for /d %%a in (*) do if exist %%a\CMakeLists.txt (
  if exist %%a\Build  rd /q /s %%a\Build
)

for /d %%a in (*) do if exist %%a\CMakeLists.txt (
  echo build %%a ...
  if not exist %%a\Build md %%a\Build
  cd %%a\Build
  "%cmake%" -D CMAKE_GENERATOR_PLATFORM:INTERNAL=Win32 -A Win32 ..\
  cd ..\..
  echo .
)

pause
