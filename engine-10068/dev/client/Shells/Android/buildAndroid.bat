@echo off
set PROJECT_DIR="%cd%/MineCraft"
echo %PROJECT_DIR%

@rem BUILD_TYP 1 debug , 0 release
set BUILD_TYP="0"
set APP_APP_ABI="armeabi-v7a"


if [%1] == [-t] if not [%2] == [1] if not [%2] == [0] echo "error: intput argument -t=%2 ,-t value 1(Debug) or 0 (Release)" & exit /B
if [%3] == [-t] if not [%4] == [1] if not [%4] == [0] echo "error: intput argument -t=%2 ,-t value 1(Debug) or 0 (Release)" & exit /B


if [%1] == [-i] if not [%2] == [arm64-v8a] if not [%2] == [armeabi] if not [%2] == [armeabi-v7a] if not [%2] == [mips]  if not [%2] == [mips64]  if not [%2] == [x86]  if not [%2] == [x86_64]  if not [%2] == [all] echo "error: intput argument -i=%2 , '-i' value is phone CPU architecture : [ all arm64-v8a armeabi armeabi-v7a mips mips64 x86 x86_64 ]" & exit /B
if [%3] == [-i] if not [%4] == [arm64-v8a] if not [%4] == [armeabi] if not [%4] == [armeabi-v7a] if not [%4] == [mips]  if not [%4] == [mips64]  if not [%4] == [x86]  if not [%4] == [x86_64]  if not [%4] == [all] echo "error: intput argument -i=%4 , '-i' value is phone CPU architecture : [ all arm64-v8a armeabi armeabi-v7a mips mips64 x86 x86_64 ]" & exit /B

if "%1" == "-i" set APP_APP_ABI="%2"
if "%1" == "-t" set BUILD_TYP="%2"
if "%3" == "-i" set APP_APP_ABI="%4"
if "%3" == "-t" set BUILD_TYP="%4"

echo info : [BUILD_TYP = %BUILD_TYP%] [APP_APP_ABI = %APP_APP_ABI%]

@rem build LordCore LordGLES2Render zlib
cd %PROJECT_DIR%/../../../../engine/NDKBuild/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
if not [%errorlevel%] == [0] echo "error:build LordCore LordGLES2Render zlib" & exit /B

@rem build FreeType FreeImage
cd %PROJECT_DIR%/../../../../engine/NDKBuild/DepBuild/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
if not [%errorlevel%] == [0] echo "error:build FreeType FreeImage" & exit /B

@rem build LordGLES2Render libLordCore
cd %PROJECT_DIR%/../../../../engine/ApkBuild/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
if not [%errorlevel%] == [0] echo "error:build LordGLES2Render libLordCore" & exit /B

@rem build libLogic
cd %PROJECT_DIR%/../../../../logic/ApkBuild/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
if not [%errorlevel%] == [0] echo "error:build libLogic" & exit /B

@rem build libGameClient
cd %PROJECT_DIR%/../../../ApkBuild/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
if not [%errorlevel%] == [0] echo "error:build libGameClient" & exit /B

@rem build libMineCraft
cd %PROJECT_DIR%/app/
call ndk-build NDK_DEBUG=%BUILD_TYP% APP_ABI=%APP_APP_ABI% -j4
if not [%errorlevel%] == [0] echo "error:build libMineCraft" & exit /B

cd %PROJECT_DIR%/../