@echo off
set PROJECT_DIR="%cd%"
echo %PROJECT_DIR%

set BUILD_TYP=0
set APP_APP_ABI="armeabi-v7a"
set APK_NAME="app-blockmango-v0.2.16-build-16.apk"
set BUILD_ARG="assembleDebug"
set GAME_ID = "Blockymods"

if [%1] == [-t] if not [%2] == [1] if not [%2] == [0] echo "error: intput argument -t=%2 ,-t value 1(Debug) or 0 (Release)" & exit /B
if [%3] == [-t] if not [%4] == [1] if not [%4] == [0] echo "error: intput argument -t=%2 ,-t value 1(Debug) or 0 (Release)" & exit /B


if [%1] == [-i] if not [%2] == [arm64-v8a] if not [%2] == [armeabi] if not [%2] == [armeabi-v7a] if not [%2] == [mips]  if not [%2] == [mips64]  if not [%2] == [x86]  if not [%2] == [x86_64]  if not [%2] == [all] echo "error: intput argument -i=%2 , '-i' value is phone CPU architecture : [ all arm64-v8a armeabi armeabi-v7a mips mips64 x86 x86_64 ]" & exit /B
if [%3] == [-i] if not [%4] == [arm64-v8a] if not [%4] == [armeabi] if not [%4] == [armeabi-v7a] if not [%4] == [mips]  if not [%4] == [mips64]  if not [%4] == [x86]  if not [%4] == [x86_64]  if not [%4] == [all] echo "error: intput argument -i=%4 , '-i' value is phone CPU architecture : [ all arm64-v8a armeabi armeabi-v7a mips mips64 x86 x86_64 ]" & exit /B

if "%1" == "-i" set APP_APP_ABI="%2"
if "%1" == "-t" set BUILD_TYP="%2"
if "%1" == "-g" set GAME_ID="%2"
if "%3" == "-i" set APP_APP_ABI="%4"
if "%3" == "-t" set BUILD_TYP="%4"
if "%3" == "-g" set GAME_ID="%4"
if "%5" == "-i" set APP_APP_ABI="%6"
if "%5" == "-t" set BUILD_TYP="%6"
if "%5" == "-g" set GAME_ID="%6"

if [%BUILD_TYP%] == [1] set BUILD_ARG="assembleDebug"
if [%BUILD_TYP%] == [0] set BUILD_ARG="assembleDebug"

echo info : [BUILD_TYP = %BUILD_TYP%] [APP_APP_ABI = %APP_APP_ABI%] [APK_NAME = %APK_NAME%] [BUILD_ARG = %BUILD_ARG%]

@rem build FreeType FreeImage zlib libLua
cd %PROJECT_DIR%/../../../../libraries/ApkBuild/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
if not [%errorlevel%] == [0] echo "error:build FreeType FreeImage zlib libLua" & cd %PROJECT_DIR% & exit /B

@rem build LordCore LordGLES2Render
cd %PROJECT_DIR%/../../../../engine/NDKBuild/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
if not [%errorlevel%] == [0] echo "error:build LordCore LordGLES2Render" & cd %PROJECT_DIR% & exit /B

@rem build libLogic
cd %PROJECT_DIR%/../../../../logic/ApkBuild/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
if not [%errorlevel%] == [0] echo "error:build libLogic" & cd %PROJECT_DIR% & exit /B

@rem build libGameClient
cd %PROJECT_DIR%/../../../ApkBuild/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
if not [%errorlevel%] == [0] echo "error:build libGameClient" & cd %PROJECT_DIR% & exit /B

@rem build libMineCraft
if [%BUILD_TYP%] == [0] (
    cd %PROJECT_DIR%/app/
    call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
    if not [%errorlevel%] == [0] echo "error:build libMineCraft" & cd %PROJECT_DIR% & exit /B
)

cd %PROJECT_DIR%

echo copy media start %GAME_ID%
copy_resource.py -g %GAME_ID%
echo copy media end %GAME_ID%

rd /s /q %PROJECT_DIR%\app\src\main\assets\resources\recipe
xcopy /e /s /q /y %PROJECT_DIR%\..\..\..\..\..\res\client\recipe\*  %PROJECT_DIR%\app\src\main\assets\resources\recipe\
xcopy /e /s /q /y %PROJECT_DIR%\..\..\..\..\..\res\client\resource.cfg  %PROJECT_DIR%\app\src\main\assets\resources\resource.cfg
xcopy /e /s /q /y %PROJECT_DIR%\..\..\..\..\..\res\client\version.json  %PROJECT_DIR%\app\src\main\assets\resources\version.json

@rem build libMineCraft
if [%BUILD_TYP%] == [0] (
    @rem apk
    cd %PROJECT_DIR%
    call gradlew.bat %BUILD_ARG%
    if not [%errorlevel%] == [0] echo "error:build apk" & exit /B

    @rem install akp
    call adb install -r %PROJECT_DIR%/app/build/outputs/apk/%APK_NAME%
    if not [%errorlevel%] == [0] echo "error:install apk" & cd %PROJECT_DIR% & exit /B

    call adb shell am start -n com.sandboxol.blockmango/com.sandboxol.blockmango.EchoesActivity
)

cd %PROJECT_DIR%

echo "end"
