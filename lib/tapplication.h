#ifndef TAPPLICATION_H
#define TAPPLICATION_H

#include "the-libs_global.h"
#include <QApplication>
#include <QJsonObject>
#include <QUrl>

struct tApplicationPrivate;

class THELIBSSHARED_EXPORT tApplication : public QApplication {
        Q_OBJECT
    public:
        explicit tApplication(int& argc, char** argv);
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

        enum UrlType {
            HelpContents,
            Sources,
            FileBug
        };

        enum Platform {
            TheDesk,
            Flatpak,
            Windows,
            WindowsAppPackage,
            MacOS,
            OtherPlatform
        };
        Q_DECLARE_FLAGS(Platforms, Platform)

        static QIcon applicationIcon();
        static QString shareDir();
        static QString genericName();
        static QPixmap aboutDialogSplashGraphic();
        static QList<QPair<QString, QString>> versions();
        static QStringList copyrightLines();
        static QString copyrightHolder();
        static QString copyrightYear();
        static bool haveApplicationUrl(UrlType type);
        static QUrl applicationUrl(UrlType type);
        static KnownLicenses applicationLicense();
        static void ensureSingleInstance(QJsonObject launchData);
        static Platform currentPlatform();

    Q_SIGNALS:
        void openFile(QString file);
        void singleInstanceMessage(QJsonObject launchData);
        void dockIconClicked();
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
        static void setApplicationUrl(UrlType type, QUrl url);

#ifdef Q_OS_WIN
        static void setWinApplicationClassId(QString classId);
#endif

        static QPixmap aboutDialogSplashGraphicFromSvg(QString svgFile);

        void installTranslators();
        static void addPluginTranslator(QString pluginName);
        static void removePluginTranslator(QString pluginName);
        static void addLibraryTranslator(QString sharePath);
        static QString macOSBundlePath();

        static void restart();

    protected:
        friend tApplicationPrivate;
        static QStringList exportBacktrace(void* data);

    private:
        bool event(QEvent* event);

        static tApplicationPrivate* d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(tApplication::Platforms)

#endif // TAPPLICATION_H
