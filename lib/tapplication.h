#ifndef TAPPLICATION_H
#define TAPPLICATION_H

#include "the-libs_global.h"
#include <QApplication>

struct tApplicationPrivate;

class THELIBSSHARED_EXPORT tApplication : public QApplication
{
    Q_OBJECT
    public:
        explicit tApplication(int &argc, char **argv);
        ~tApplication();

        enum KnownLicenses {
            Gpl3,
            Gpl3OrLater,
            Gpl2,
            Gpl2OrLater,
            Lgpl3,
            Lgpl3OrLater,
            Lgpl2_1,
            Lgpl2_1OrLater,
            Other
        };

        static QIcon applicationIcon();
        static QString shareDir();
        static QString genericName();
        static QPixmap aboutDialogSplashGraphic();
        static QList<QPair<QString, QString>> versions();
        static QStringList copyrightLines();
        static QString copyrightHolder();
        static QString copyrightYear();
        static KnownLicenses applicationLicense();

    Q_SIGNALS:
        void openFile(QString file);
        void updateTranslators();

    public Q_SLOTS:
        void registerCrashTrap();
        static QStringList exportBacktrace();

        static void setApplicationIcon(QIcon icon);
        static void setShareDir(QString shareDir);
        static void setGenericName(QString genericName);
        static void setAboutDialogSplashGraphic(QPixmap aboutDialogSplashGraphic);
        static void addLibraryVersion(QString libraryName, QString version);
        static void addCopyrightLine(QString copyrightLine);
        static void setCopyrightHolder(QString copyrightHolder);
        static void setCopyrightYear(QString copyrightYear);
        static void setApplicationLicense(KnownLicenses license);

        static QPixmap aboutDialogSplashGraphicFromSvg(QString svgFile);

        void installTranslators();
        static QString macOSBundlePath();

    private:
        bool event(QEvent * event);

        static tApplicationPrivate* d;
};

#endif // TAPPLICATION_H
