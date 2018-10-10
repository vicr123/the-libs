#-------------------------------------------------
#
# Project created by QtCreator 2016-12-30T16:59:03
#
#-------------------------------------------------

QT       += widgets
CONFIG   += c++14

TARGET = the-libs
TEMPLATE = lib

DEFINES += THELIBS_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += tvariantanimation.cpp \
    tpropertyanimation.cpp \
    thelibsglobal.cpp \
    ttoast.cpp \
    tvirtualkeyboard.cpp \
    tcircularspinner.cpp \
    tnotification/tnotification-common.cpp \
    tapplication.cpp \
    tshortcuthud.cpp

HEADERS += tvariantanimation.h\
        the-libs_global.h \
    tpropertyanimation.h \
    ttoast.h \
    tnotification.h \
    tvirtualkeyboard.h \
    tcircularspinner.h \
    tapplication.h \
    tpromise.h \
    tshortcuthud.h

unix {
    module.files = qt_thelib.pri
    module.path = $$[QMAKE_MKSPECS]/modules
    header.files = *.h
    INSTALLS += target module header
}

unix:!macx {
    QT += dbus

    target.path = /usr/lib
    header.path = /usr/include/the-libs

    SOURCES += tnotification/tnotification-linux.cpp
}

macx {
    target.path = /usr/local/lib
    header.path = /usr/local/include/the-libs

    SOURCES += tnotification/tnotification-mac.cpp
}

win32 {
    CONFIG(debug, debug|release): TARGET = the-libsd

    module.files = qt_thelib.pri
    module.path = $$[QMAKE_MKSPECS]/modules

    header.files = *.h
    header.path = "C:/Program Files/thelibs/include"

    target.path = "C:/Program Files/thelibs/lib"

    INSTALLS += target module header

    SOURCES += tnotification/tnotification-win.cpp
}

DISTFILES += \
    qt_thelib.pri

FORMS += \
    tshortcuthud.ui
