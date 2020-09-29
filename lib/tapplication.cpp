#include "tapplication.h"
#include <QFileOpenEvent>
#include <QTranslator>
#include <QSysInfo>
#include <QPainter>
#include <QSvgRenderer>
#include <QProcess>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSharedMemory>
#include <QLocalServer>
#include <QLocalSocket>

#ifdef T_OS_UNIX_NOT_MAC
    #include <signal.h>
#endif

#ifdef Q_OS_MAC
    #include <CoreFoundation/CFBundle.h>
#endif

#ifdef Q_OS_WIN
    #include <Windows.h>
    #include <DbgHelp.h>
    #include "tnotification/tnotification-win.h"
#endif

#ifdef HAVE_LIBUNWIND
    #define UNW_LOCAL_ONLY
    #include <libunwind.h>
#endif

#if defined(__GNUG__)
    #include <cxxabi.h>
#endif

struct tApplicationPrivate {
    QTranslator translator;
    QStringList pluginTranslators;
    QList<QTranslator*> applicationTranslators;
    tApplication* applicationInstance;

    bool crashHandlingEnabled = false;
    QIcon applicationIcon;

    QString shareDir;
    QString genericName;
    QPixmap aboutDialogSplashGraphic;
    QList<QPair<QString, QString>> versions;
    QStringList copyrightLines;
    tApplication::KnownLicenses license = tApplication::Other;
    QString copyrightHolder, copyrightYear;

    QSharedMemory* singleInstanceMemory = nullptr;
    QLocalServer* singleInstanceServer = nullptr;

#ifdef T_OS_UNIX_NOT_MAC
    static void crashTrapHandler(int sig);
#elif defined(Q_OS_WIN)
    static LONG WINAPI crashTrapHandler(PEXCEPTION_POINTERS exceptionInfo);
    PCONTEXT crashCtx = nullptr;

    QString winClassId;
#else
    static void crashTrapHandler();
#endif
};

tApplicationPrivate* tApplication::d = nullptr;

tApplication::tApplication(int& argc, char** argv) : QApplication(argc, argv) {
    d = new tApplicationPrivate();
    d->applicationInstance = this;

    //Mark some strings for translation
    if (false) {
        //macOS Application Menu
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Services");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Hide %1");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Hide Others");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Show All");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Preferences...");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "About %1");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Quit %1");

        //QCommandLineParser
        (void) QT_TRANSLATE_NOOP("QCommandLineParser", "Displays version information.");
        (void) QT_TRANSLATE_NOOP("QCommandLineParser", "Displays this help.");
        (void) QT_TRANSLATE_NOOP("QCommandLineParser", "Unknown option '%1'.");
        (void) QT_TRANSLATE_NOOP("QCommandLineParser", "Unknown options: %1.");
        (void) QT_TRANSLATE_NOOP("QCommandLineParser", "Missing value after '%1'.");
        (void) QT_TRANSLATE_NOOP("QCommandLineParser", "Unexpected value after '%1'.");
        (void) QT_TRANSLATE_NOOP("QCommandLineParser", "[options]");
        (void) QT_TRANSLATE_NOOP("QCommandLineParser", "Usage: %1");
        (void) QT_TRANSLATE_NOOP("QCommandLineParser", "Options:");
        (void) QT_TRANSLATE_NOOP("QCommandLineParser", "Arguments:");
    }

    Q_INIT_RESOURCE(thelibs_translations);
    Q_INIT_RESOURCE(thelibs_icons);
    d->translator.load(QLocale::system().name(), ":/the-libs/translations/");
    installTranslator(&d->translator);

#ifdef Q_OS_MAC
    this->setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

    d->versions.append({"the-libs", QStringLiteral("%1 (API %2)").arg(THE_LIBS_VERSION).arg(THE_LIBS_API_VERSION)});
    d->versions.append({"Qt", QString(qVersion())});
}

bool tApplication::event(QEvent* event) {
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent* openEvent = (QFileOpenEvent*) event;
        emit openFile(openEvent->file());
    } else if (event->type() == QEvent::LocaleChange) {
        //Reinstall the-libs translator
        d->translator.load(QLocale::system().name(), ":/the-libs/translations/");

        //Reinstall the application translators
        installTranslators();

        //Tell everyone the translations have changed
        emit updateTranslators();
    }

    return QApplication::event(event);
}

QStringList tApplication::exportBacktrace(void* data) {
    QStringList backtrace;
#ifdef HAVE_LIBUNWIND
    unw_cursor_t cur;
    unw_context_t ctx;

    unw_getcontext(&ctx);
    unw_init_local(&cur, &ctx);

    //Start unwinding
    while (unw_step(&cur) > 0) {
        unw_word_t offset;
        unw_word_t pc;
        unw_get_reg(&cur, UNW_REG_IP, &pc);
        if (pc == 0) break;

        char sym[256];
        if (unw_get_proc_name(&cur, sym, sizeof(sym), &offset) == 0) {
            //Demangle the name depending on the current compiler
            QString functionName;
#if defined(__GNUG__)
            //We're using a gcc compiler
            int status;
            char* demangled = abi::__cxa_demangle(sym, nullptr, nullptr, &status);
            if (status == 0) functionName = QString::fromLocal8Bit(demangled);
            std::free(demangled);
#elif defined(_MSC_VER)
            //We're using msvc
#endif

            if (functionName == "") {
                functionName = QString::fromLocal8Bit(sym);
            }

            backtrace.append(QString("0x" + QString::number(pc, 16) + ": " + functionName));
        } else {
            backtrace.append(QString("0x" + QString::number(pc, 16) + ": ??\n").toUtf8());
        }
    }
#endif

#ifdef Q_OS_WIN
    const int maxNameLen = 255;
    HANDLE proc = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    DWORD64 displacement;

    CONTEXT ctx;
    if (data == nullptr) {
        ctx.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&ctx);
    } else {
        ctx = *reinterpret_cast<PCONTEXT>(data);
    }

    SymInitialize(proc, NULL, TRUE);
    SymSetOptions(SYMOPT_LOAD_LINES);

    STACKFRAME64 frame;
    frame.AddrPC.Offset = ctx.Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = ctx.Rbp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = ctx.Rsp;
    frame.AddrStack.Mode = AddrModeFlat;

    //Start walking
    while (StackWalk(IMAGE_FILE_MACHINE_AMD64, proc, thread, &frame, &ctx, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL)) {
        char buffer[sizeof(SYMBOL_INFO) + maxNameLen * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = maxNameLen;
        SymFromAddr(proc, frame.AddrPC.Offset, &displacement, symbol);

        //Unmangle the name
        char name[maxNameLen];
        if (!UnDecorateSymbolName(symbol->Name, reinterpret_cast<PSTR>(name), maxNameLen, UNDNAME_COMPLETE)) {
            memcpy(name, symbol->Name, maxNameLen);
        }

        backtrace.append(QString("0x" + QString::number(frame.AddrPC.Offset, 16) + ": " + QString::fromLatin1(name)));
    }

    SymCleanup(proc);
#endif
    return backtrace;
}

tApplication::~tApplication() {
    if (d->singleInstanceMemory) {
        d->singleInstanceMemory->detach();
        delete d->singleInstanceMemory;
    }
    if (d->singleInstanceServer) d->singleInstanceServer->close();
}

void writeCrashSysInfo(QStringList& bt) {
    bt.append("Application name:\t" + tApplication::applicationName());
    bt.append("Application PID:\t" + QString::number(tApplication::applicationPid()));
    bt.append("Application Command:\t" + tApplication::applicationFilePath());

    bt.append(" ");

    bt.append("System Name:\t" + QSysInfo::prettyProductName());
    bt.append("System Architecture:\t" + QSysInfo::currentCpuArchitecture());
    bt.append("System Kernel:\t" + QSysInfo::kernelType());
    bt.append("System Type:\t" + QSysInfo::productType());
    bt.append("System Version:\t" + QSysInfo::productVersion());

    bt.append(" ");
}

#ifdef T_OS_UNIX_NOT_MAC
void tApplicationPrivate::crashTrapHandler(int sig) {
    signal(sig, SIG_DFL);
    //Attempt to start Bonkers to tell the user that the app has crashed
    QStringList args = {
        "--appname", "\"" + tApplication::applicationName() + "\"",
        "--apppath", "\"" + tApplication::applicationFilePath() + "\"",
        "--bt"
    };

    QProcess* process = new QProcess();
    process->setEnvironment(QProcess::systemEnvironment());
    process->start("/usr/lib/bonkers " + args.join(" "), QProcess::Unbuffered | QProcess::WriteOnly);

    //Write out crash information
    QStringList bt;
    writeCrashSysInfo(bt);

    QString signalName;
    switch (sig) {
        case SIGSEGV:
            signalName = QStringLiteral("SIGSEGV");
            break;
        case SIGBUS:
            signalName = QStringLiteral("SIGBUS");
            break;
        case SIGFPE:
            signalName = QStringLiteral("SIGFPE");
            break;
        case SIGILL:
            signalName = QStringLiteral("SIGILL");
            break;
        case SIGABRT:
            signalName = QStringLiteral("SIGABRT");
            break;
        default:
            signalName = QStringLiteral("SIG") + QString::number(sig);
    }
    char* signalText = strsignal(sig);
    if (signalText == nullptr) {
        bt.append("Received signal:\t" + signalName);
    } else {
        bt.append("Received signal:\t" + signalName + " (" + QString::fromLocal8Bit(signalText) + ")");
    }

    bt.append(" ");
    bt.append("Backtrace:");

    //Get a backtrace
    bt.append(tApplication::exportBacktrace());
    process->write(bt.join("\n").toUtf8());
    process->closeWriteChannel();
    process->waitForBytesWritten();

    //Reset the signal and re-raise it
    raise(sig);
}
#endif

#ifdef Q_OS_WIN
LONG WINAPI tApplicationPrivate::crashTrapHandler(PEXCEPTION_POINTERS exceptionInfo) {

    //Attempt to start Bonkers to tell the user that the app has crashed
    QStringList args = {
        "--appname", "\"" + tApplication::applicationName() + "\"",
        "--apppath", "\"" + tApplication::applicationFilePath() + "\"",
        "--bt"
    };

    QProcess* process = new QProcess();
    process->setEnvironment(QProcess::systemEnvironment());
    process->start(QDir(tApplication::applicationDirPath()).absoluteFilePath("bonkers.exe") + " " + args.join(" "), QProcess::Unbuffered | QProcess::WriteOnly);

    //Write out crash information
    QStringList bt;
    writeCrashSysInfo(bt);

    bt.append("Windows Exception " + QString("0x" + QString::number(exceptionInfo->ExceptionRecord->ExceptionCode, 16)));

    bt.append(" ");
    bt.append("Backtrace:");

    //Get a backtrace
    bt.append(tApplication::exportBacktrace(exceptionInfo->ContextRecord));
    process->write(bt.join("\n").toUtf8());
    process->closeWriteChannel();
    process->waitForBytesWritten();

    //Reset the signal and re-raise it

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void tApplication::registerCrashTrap() {
    //macOS has its own crash trap handler so we don't do anything for macOS

#ifdef T_OS_UNIX_NOT_MAC
    //Check that the crash handler exists
    if (QFile("/usr/lib/bonkers").exists()) {
        //Enable the crash trap
        d->crashHandlingEnabled = true;

        signal(SIGSEGV, tApplicationPrivate::crashTrapHandler);
        signal(SIGBUS, tApplicationPrivate::crashTrapHandler);
        signal(SIGFPE, tApplicationPrivate::crashTrapHandler);
        signal(SIGILL, tApplicationPrivate::crashTrapHandler);
        signal(SIGABRT, tApplicationPrivate::crashTrapHandler);
    }
#endif

#ifdef Q_OS_WIN
    //Check that the crash handler exists
    if (QFile(QDir(this->applicationDirPath()).absoluteFilePath("bonkers.exe")).exists()) {
        //Enable the crash trap
        d->crashHandlingEnabled = true;

        SetUnhandledExceptionFilter(tApplicationPrivate::crashTrapHandler);
    }
#endif
}

QStringList tApplication::exportBacktrace() {
    return tApplication::exportBacktrace(nullptr);
}

QIcon tApplication::applicationIcon() {
    return d->applicationIcon;
}

void tApplication::setApplicationIcon(QIcon icon) {
    d->applicationIcon = icon;
}

QString tApplication::shareDir() {
    return d->shareDir;
}

void tApplication::setShareDir(QString shareDir) {
    d->shareDir = shareDir;
}

void tApplication::installTranslators() {
    for (QTranslator* translator : d->applicationTranslators) {
        removeTranslator(translator);
        translator->deleteLater();
    }
    d->applicationTranslators.clear();


    QTranslator* localTranslator = new QTranslator();
#if defined(Q_OS_MAC)
    localTranslator->load(QLocale::system().name(), macOSBundlePath() + "/Contents/translations/");
#elif defined(Q_OS_LINUX)
    localTranslator->load(QLocale::system().name(), d->shareDir + "/translations");
#elif defined(Q_OS_WIN)
    localTranslator->load(QLocale::system().name(), this->applicationDirPath() + "\\translations");
#endif
    this->installTranslator(localTranslator);
    d->applicationTranslators.append(localTranslator);

    for (QString plugin : d->pluginTranslators) {
        QTranslator* translator = new QTranslator();
#if defined(Q_OS_MAC)
        translator->load(QLocale::system().name(), macOSBundlePath() + "/Contents/Resources/Plugins/" + plugin + "/translations/");
#elif defined(Q_OS_LINUX)
        translator->load(QLocale::system().name(), d->shareDir + "/" + plugin + "/translations");
#elif defined(Q_OS_WIN)
        translator->load(QLocale::system().name(), this->applicationDirPath() + "\\plugins\\" + plugin + "\\translations");
#endif
        installTranslator(translator);
        d->applicationTranslators.append(translator);
    }
}

void tApplication::addPluginTranslator(QString pluginName) {
    d->pluginTranslators.append(pluginName);
    d->applicationInstance->installTranslators();
}

void tApplication::removePluginTranslator(QString pluginName) {
    d->pluginTranslators.removeOne(pluginName);
    d->applicationInstance->installTranslators();
}

QString tApplication::macOSBundlePath() {
#ifdef Q_OS_MAC
    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    const char* pathPtr = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());

    QString bundlePath = QString::fromLocal8Bit(pathPtr);

    CFRelease(appUrlRef);
    CFRelease(macPath);
    return bundlePath;
#else
    return "";
#endif
}

void tApplication::setGenericName(QString genericName) {
    d->genericName = genericName;
}

void tApplication::setAboutDialogSplashGraphic(QPixmap aboutDialogSplashGraphic) {
    d->aboutDialogSplashGraphic = aboutDialogSplashGraphic;
}

void tApplication::addLibraryVersion(QString libraryName, QString version) {
    d->versions.append({libraryName, version});
}

void tApplication::addCopyrightLine(QString copyrightLine) {
    d->copyrightLines.append(copyrightLine);
}

void tApplication::setCopyrightHolder(QString copyrightHolder) {
    d->copyrightHolder = copyrightHolder;
}

void tApplication::setCopyrightYear(QString copyrightYear) {
    d->copyrightYear = copyrightYear;
}

void tApplication::setApplicationLicense(tApplication::KnownLicenses license) {
    d->license = license;
}

#ifdef Q_OS_WIN
void tApplication::setWinApplicationClassId(QString classId) {
    d->winClassId = classId;
    tNotificationWindows::initialise(classId);
}
#endif

QPixmap tApplication::aboutDialogSplashGraphicFromSvg(QString svgFile) {
    QImage image(SC_DPI_T(QSize(100, 340), QSize), QImage::Format_ARGB32);
    QPainter painter(&image);
    QSvgRenderer renderer(svgFile);
    renderer.render(&painter, QRectF(QPointF(0, 0), SC_DPI_T(QSizeF(100, 340), QSizeF)));
    return QPixmap::fromImage(image);
}

QString tApplication::genericName() {
    return d->genericName;
}

QPixmap tApplication::aboutDialogSplashGraphic() {
    return d->aboutDialogSplashGraphic;
}

QList<QPair<QString, QString>> tApplication::versions() {
    QList<QPair<QString, QString>> versions;
    versions.append({tApplication::applicationName(), tApplication::applicationVersion()});
    versions.append(d->versions);
    return versions;
}

QStringList tApplication::copyrightLines() {
    QStringList copyrightLines = d->copyrightLines;

    switch (d->license) {
        case Gpl3:
            copyrightLines.prepend(tr("Licensed under the terms of the %1.").arg(tr("GNU General Public License, version 3")));
            break;
        case Gpl3OrLater:
            copyrightLines.prepend(tr("Licensed under the terms of the %1.").arg(tr("GNU General Public License, version 3, or later")));
            break;
        case Gpl2:
            copyrightLines.prepend(tr("Licensed under the terms of the %1.").arg(tr("GNU General Public License, version 2")));
            break;
        case Gpl2OrLater:
            copyrightLines.prepend(tr("Licensed under the terms of the %1.").arg(tr("GNU General Public License, version 2, or later")));
            break;
        case Lgpl3:
            copyrightLines.prepend(tr("Licensed under the terms of the %1.").arg(tr("GNU Lesser General Public License, version 3")));
            break;
        case Lgpl3OrLater:
            copyrightLines.prepend(tr("Licensed under the terms of the %1.").arg(tr("GNU Lesser General Public License, version 3, or later")));
            break;
        case Lgpl2_1:
            copyrightLines.prepend(tr("Licensed under the terms of the %1.").arg(tr("GNU Lesser General Public License, version 2.1")));
            break;
        case Lgpl2_1OrLater:
            copyrightLines.prepend(tr("Licensed under the terms of the %1.").arg(tr("GNU Lesser General Public License, version 2.1, or later")));
            break;
        case Other:
            ;
    }

    if (!d->copyrightHolder.isEmpty()) {
        copyrightLines.prepend(tr("Copyright © %1.").arg(d->copyrightYear.isEmpty() ? d->copyrightHolder : d->copyrightHolder + " " + d->copyrightYear));
    }

    return copyrightLines;
}

tApplication::KnownLicenses tApplication::applicationLicense() {
    return d->license;
}

void tApplication::ensureSingleInstance(QJsonObject launchData) {
    QString sharedMemoryName = QStringList({"the-libs-single-instance", organizationName(), applicationName()}).join("_");

#ifdef Q_OS_UNIX
    //Mitigate crashes by discarding the memory if it is not being used
    d->singleInstanceMemory = new QSharedMemory(sharedMemoryName);
    d->singleInstanceMemory->attach();
    delete d->singleInstanceMemory;
    d->singleInstanceMemory = nullptr;
#endif

    d->singleInstanceMemory = new QSharedMemory(sharedMemoryName);
    if (d->singleInstanceMemory->create(sharedMemoryName.length())) {
        QLocalServer::removeServer(sharedMemoryName);
        d->singleInstanceServer = new QLocalServer();
        connect(d->singleInstanceServer, &QLocalServer::newConnection, [ = ] {
            QLocalSocket* socket = d->singleInstanceServer->nextPendingConnection();
            connect(socket, &QLocalSocket::readyRead, [ = ] {
                QJsonObject obj = QJsonDocument::fromJson(socket->readAll()).object();
                emit static_cast<tApplication*>(tApplication::instance())->singleInstanceMessage(obj);
            });
            connect(socket, &QLocalSocket::disconnected, [ = ] {
                socket->deleteLater();
            });
        });
        d->singleInstanceServer->listen(sharedMemoryName);
    } else {
        if (!d->singleInstanceMemory->attach()) std::exit(0); //Can't attach to the memory

        QLocalSocket* socket = new QLocalSocket();
        socket->connectToServer(sharedMemoryName);
        socket->waitForConnected();
        socket->write(QJsonDocument(launchData).toJson());
        socket->waitForBytesWritten();
        socket->close();

        std::exit(0);
    }
}

QString tApplication::copyrightHolder() {
    return d->copyrightHolder;
}

QString tApplication::copyrightYear() {
    return d->copyrightYear;
}

void tApplication::restart() {
#ifdef Q_OS_MAC
    QProcess::startDetached("open", {macOSBundlePath(), "-n"});
#else
    QProcess::startDetached(tApplication::applicationFilePath());
#endif
    tApplication::quit();
}
