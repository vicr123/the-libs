if [ $STAGE = "script" ]; then
  if [ $TRAVIS_OS_NAME = "linux" ]; then
    echo "[TRAVIS] Preparing build environment"
    source /opt/qt510/bin/qt510-env.sh
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
      sudo add-apt-repository -y ppa:beineri/opt-qt-5.10.0-xenial
      sudo apt-get update -qq
      sudo apt-get install qt510-meta-minimal qt510multimedia qt510x11extras qt510tools qt510translations qt510svg qt510websockets xorg-dev libxcb-util0-dev libgl1-mesa-dev
  else
    echo "[TRAVIS] Preparing to build for macOS"
    brew update
    brew install qt5
  fi
fi
