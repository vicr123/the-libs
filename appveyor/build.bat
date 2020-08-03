if "%APPVEYOR_REPO_TAG_NAME%"=="continuous" (

    exit 1

)


set QTDIR=C:\Qt\5.15\msvc2019_64
set PATH=%PATH%;%QTDIR%\bin
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
qmake the-libs.pro
nmake release
nmake install
