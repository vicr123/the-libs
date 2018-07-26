#ifndef THELIBS_GLOBAL_H
#define THELIBS_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusConnection>
#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QSettings>

#if defined(THELIBS_LIBRARY)
#  define THELIBSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define THELIBSSHARED_EXPORT Q_DECL_IMPORT
#endif

#define THE_LIBS_API_VERSION 1

class THELIBSSHARED_EXPORT theLibsGlobal : public QObject {
    Q_OBJECT

public:
    static theLibsGlobal* instance();

    static float getDPIScaling();
    static QStringList searchInPath(QString executable);

public slots:
    bool powerStretchEnabled();
    bool allowSystemAnimations();

private slots:
    void powerStretchChangedPrivate(bool isOn);

signals:
    void powerStretchChanged(bool isOn);

private:
    theLibsGlobal();

    bool powerStretch = false;
    QSettings* themeSettings = new QSettings("theSuite", "ts-qtplatform");
};

#endif // THELIBS_GLOBAL_H
