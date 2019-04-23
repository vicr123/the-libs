#include "tapplication.h"
#include <QFileOpenEvent>
#include <QTranslator>
#include <QSysInfo>

#ifdef T_OS_UNIX_NOT_MAC
#include <QProcess>
#include <signal.h>
#endif

#ifdef Q_OS_MAC
#include <CoreFoundation/CFBundle.h>
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
    tApplication* applicationInstance;

    bool crashHandlingEnabled = false;
    QIcon applicationIcon;

    QString shareDir;
};

tApplicationPrivate* tApplication::d = nullptr;

tApplication::tApplication(int& argc, char** argv) : QApplication(argc, argv)
{
    d = new tApplicationPrivate();
    d->applicationInstance = this;

    //Mark some strings for translation so apps look right on macOS
    if (false) {
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Services");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Hide %1");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Hide Others");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Show All");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Preferences...");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "About %1");
        (void) QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Quit %1");
    }

    Q_INIT_RESOURCE(thelibs_translations);
    d->translator.load(QLocale::system().name(), ":/the-libs/translations/");
    installTranslator(&d->translator);

#ifdef Q_OS_MAC
    this->setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif
}

bool tApplication::event(QEvent *event) {
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = (QFileOpenEvent*) event;
        emit openFile(openEvent->file());
    }

    return QApplication::event(event);
}

tApplication::~tApplication() {

}

#ifdef T_OS_UNIX_NOT_MAC
void crashTrapHandler(int sig) {
    signal(sig, SIG_DFL);
    //Attempt to start Bonkers to tell the user that the app has crashed
    QStringList args = {
        "--appname", tApplication::applicationName(),
        "--apppath", tApplication::applicationFilePath(),
        "--bt"
    };

    QProcess* process = new QProcess();
    process->start("/usr/lib/bonkers " + args.join(" "), QProcess::Unbuffered | QProcess::WriteOnly);

    //Write out crash information
    QStringList bt;
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

void tApplication::registerCrashTrap() {
#ifdef T_OS_UNIX_NOT_MAC
    //Check that the crash handler exists
    if (QFile("/usr/lib/bonkers").exists()) {
        //Enable the crash trap
        d->crashHandlingEnabled = true;

        signal(SIGSEGV, crashTrapHandler);
        signal(SIGBUS, crashTrapHandler);
        signal(SIGFPE, crashTrapHandler);
        signal(SIGILL, crashTrapHandler);
        signal(SIGABRT, crashTrapHandler);
    }
#endif
}

QStringList tApplication::exportBacktrace() {
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
    return backtrace;
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
    QTranslator* localTranslator = new QTranslator();
#ifdef Q_OS_MAC
    this->setAttribute(Qt::AA_DontShowIconsInMenus, true);

    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());

    bundlePath = QString::fromLocal8Bit(pathPtr);
    localTranslator->load(QLocale::system().name(), bundlePath + "/Contents/translations/");

    CFRelease(appUrlRef);
    CFRelease(macPath);
#endif

#ifdef Q_OS_LINUX
    localTranslator->load(QLocale::system().name(), d->shareDir + "/translations");
#endif

#ifdef Q_OS_WIN
    localTranslator->load(QLocale::system().name(), a.applicationDirPath() + "\\translations");
#endif

    this->installTranslator(localTranslator);
}
