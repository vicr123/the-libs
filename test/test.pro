#-------------------------------------------------
#
# Project created by QtCreator 2018-07-24T20:40:49
#
#-------------------------------------------------

QT       += core widgets testlib dbus

TARGET = tst_unittesting
CONFIG   += console testcase c++14
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        tst_unittesting.cpp

win32 {
    CONFIG(release, debug|release) {
        LIBS += -L$$OUT_PWD/../lib/release/ -lthe-libs
    } else {
        LIBS += -L$$OUT_PWD/../lib/debug/ -lthe-libs
    }
} else {
    mac {
        LIBS += -F$$OUT_PWD/../lib/ -lthe-libs
    } else {
        unix {
            LIBS += -L$$OUT_PWD/../lib/ -lthe-libs
        }
    }
}

INCLUDEPATH += $$PWD/../lib
DEPENDPATH += $$PWD/../lib
