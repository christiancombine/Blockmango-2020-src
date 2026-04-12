@echo off
set PROJECT_DIR="%cd%"
echo %PROJECT_DIR%

set APP_APP_ABI="x86"

rd /s /q %PROJECT_DIR%\app\src\main\assets\resources\Media
rd /s /q %PROJECT_DIR%\app\src\main\assets\resources\recipe
xcopy /e /s /q /y %PROJECT_DIR%\..\..\..\..\..\res\client\Media\*  %PROJECT_DIR%\app\src\main\assets\resources\Media\
xcopy /e /s /q /y %PROJECT_DIR%\..\..\..\..\..\res\client\recipe\*  %PROJECT_DIR%\app\src\main\assets\resources\recipe\
xcopy /e /s /q /y %PROJECT_DIR%\..\..\..\..\..\res\client\resource.cfg  %PROJECT_DIR%\app\src\main\assets\resources\resource.cfg

cd %PROJECT_DIR%
set PLATFORM_DIR=%PROJECT_DIR%\..\..\..\..\..\..\and-mcpeonline
if exist %PLATFORM_DIR% (
    rd /s /q %PLATFORM_DIR%\app\src\main\assets\resources\
    xcopy /e /s /q /y %PROJECT_DIR%\app\src\main\assets\resources\* %PLATFORM_DIR%\app\src\main\assets\resources\
    xcopy /e /s /q /y %PROJECT_DIR%\app\libs\%APP_APP_ABI%\libBlockMan.so  %PLATFORM_DIR%\app\libs\%APP_APP_ABI%\libBlockMan.so
    xcopy /e /s /q /y %PROJECT_DIR%\app\libs\%APP_APP_ABI%\libBlockMan.so  %PLATFORM_DIR%\app\build\intermediates\jniLibs\envtest\debug\%APP_APP_ABI%\libBlockMan.so
    xcopy /e /s /q /y %PROJECT_DIR%\app\libs\%APP_APP_ABI%\libBlockMan.so  %PLATFORM_DIR%\app\build\intermediates\jniLibs\google\debug\%APP_APP_ABI%\libBlockMan.so
)