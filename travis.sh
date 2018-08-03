if [ $STAGE = "script" ]; then
  if [ $TRAVIS_OS_NAME = "linux" ]; then
    echo "[TRAVIS] Running qmake"
    qmake
    echo "[TRAVIS] Building project"
    make
  else
    echo "[TRAVIS] Building for macOS"
    export PATH="/usr/local/opt/qt/bin:$PATH"
    cd ..
    mkdir "build-thelibs"
    cd "build-thelibs"
    echo "[TRAVIS] Running qmake"
    qmake "INCLUDEPATH += /usr/local/opt/qt/include" "LIBS += -L/usr/local/opt/qt/lib" ../the-libs/the-libs.pro
    echo "[TRAVIS] Building project"
    make
  fi
elif [ $STAGE = "before_install" ]; then
  if [ $TRAVIS_OS_NAME = "linux" ]; then
    sudo apt-get update -qq
    sudo apt-get install libqt5core5a libqt5dbus5 libqt5gui5 libqt5x11extras5 qtchooser qt5-default
  else
    echo "[TRAVIS] Preparing to build for macOS"
    brew update
    brew install qt5
  fi
fi
