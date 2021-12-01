#ifndef THELIBS_GLOBAL_H
#define THELIBS_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QSettings>

#ifdef QT_WIDGETS_LIB
    #include <QApplication>
    #include <QStyle>
    #include <QStyleFactory>
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC) && !defined(Q_OS_ANDROID)
    #define T_OS_UNIX_NOT_MAC
#endif

#ifdef T_OS_UNIX_NOT_MAC
    #include <QDBusMessage>
    #include <QDBusReply>
    #include <QDBusConnection>
#endif

#if defined(THELIBS_LIBRARY)
    #define THELIBSSHARED_EXPORT Q_DECL_EXPORT
#else
    #define THELIBSSHARED_EXPORT Q_DECL_IMPORT
#endif

#define THE_LIBS_API_VERSION 7
#define THE_LIBS_VERSION "5.0"

#ifdef QT_WIDGETS_LIB
    #define SC_DPI(pixels) static_cast<int>(pixels * theLibsGlobal::getDPIScaling())
    #define SC_DPI_T(value, type) static_cast<type>(value * theLibsGlobal::getDPIScaling())
#endif

struct theLibsGlobalPrivate;
class THELIBSSHARED_EXPORT theLibsGlobal : public QObject {
        Q_OBJECT

    public:
        static theLibsGlobal* instance();

#ifdef QT_WIDGETS_LIB
        static double getDPIScaling();
        static void tintImage(QImage& image, QColor tint);
#endif
        static QStringList searchInPath(QString executable);

        static QColor lineColor(QColor textColor);

    public Q_SLOTS:
        bool powerStretchEnabled();
        bool allowSystemAnimations();

    private Q_SLOTS:
        void powerStretchChangedPrivate(bool isOn);
        void dbusPropertyChangedPrivate(QString interfaceName, QMap<QString, QVariant> changedProperties, QStringList invalidatedProperties);

    Q_SIGNALS:
        void powerStretchChanged(bool isOn);

    private:
        theLibsGlobal();

        theLibsGlobalPrivate* d;

};

#endif // THELIBS_GLOBAL_H
