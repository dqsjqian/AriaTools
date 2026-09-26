@echo off
call "D:\worksoft\VS2026\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
cd /d D:\Coding\AriaTools
set "HTTPS_PROXY=http://127.0.0.1:10808"
set "HTTP_PROXY=http://127.0.0.1:10808"
python tools\ci\fetch_aria.py || exit /b 1
"C:\Program Files\CMake\bin\cmake.exe" -S Workbench -B build\web -G Ninja -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_BUILD_TYPE=Release -DWORKBENCH_TARGET_QT=OFF -DWORKBENCH_TARGET_WEB=ON || exit /b 1
"C:\Program Files\CMake\bin\cmake.exe" --build build\web --parallel 8 || exit /b 1
build\web\bin\workbench_web.exe --probe
