@echo off
echo ========================================
echo Building 8502_CrouseWork (Debug x64)
echo ========================================

"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
    "8502_CrouseWork\8502_CrouseWork.vcxproj" ^
    /p:Configuration=Debug ^
    /p:Platform=x64 ^
    /t:Build ^
    /v:minimal

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Build succeeded!
    echo ========================================
) else (
    echo.
    echo ========================================
    echo Build failed with error code %ERRORLEVEL%
    echo ========================================
)

pause
