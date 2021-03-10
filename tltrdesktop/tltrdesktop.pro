QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        desktopfile.cpp \
        jsonfile.cpp \
        main.cpp

include($$PWD/../the-libs/prifiles/varset.pri)

unix:!macx {
    target.path = $$THELIBS_INSTALL_BIN
    INSTALLS += target
}

HEADERS += \
    desktopfile.h \
    jsonfile.h
