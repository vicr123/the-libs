QT.thelib.VERSION = 1.0
QT.thelib.MAJOR_VERSION = 1
QT.thelib.MINOR_VERSION = 1
QT.thelib.name = the-libs
QT.thelib.depends = core dbus

unix:!macx {
    QT.thelib.includes = /usr/include/the-libs/
    QT.thelib.libs = /usr/lib
}

macx {
    QT.thelib.includes = /usr/local/include/the-libs
    QT.thelib.libs = /usr/local/lib
}
