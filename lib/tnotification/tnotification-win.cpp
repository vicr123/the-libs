#include "tnotification.h"
#include "tnotification-win.h"

#include <tapplication.h>
#include <QOperatingSystemVersion>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QXmlStreamWriter>
#include <QDebug>

#include <unknwn.h>
#include <combaseapi.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/windows.data.xml.dom.h>
#include <winrt/windows.ui.notifications.h>

#include <Sdkddkver.h>

#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN10

#include <notificationactivationcallback.h>

using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::UI::Notifications;

QString tNotificationWindows::classId = "";

struct tNotificationPrivateByOS {
    static QMap<uint, tNotification*> notifications;
    static uint current;
};

QMap<uint, tNotification*> tNotificationPrivateByOS::notifications = QMap<uint, tNotification*>();
uint tNotificationPrivateByOS::current = 0;

void tNotification::post(bool deleteWhenDone) {
    bool shouldUseFallback = false;
    if (QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows10 && tNotificationWindows::classId != "") {
        if (QSettings(QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes\\CLSID\\%1\\LocalServer32").arg(tNotificationWindows::classId), QSettings::NativeFormat).value(".").toString().isEmpty() &&
                QSettings(QStringLiteral("HKEY_CURRENT_USER\\SOFTWARE\\Classes\\CLSID\\%1\\LocalServer32").arg(tNotificationWindows::classId), QSettings::NativeFormat).value(".").toString().isEmpty()) {
            shouldUseFallback = true;
        } else {
            QString amuid = QStringLiteral("%1.%2").arg(tApplication::organizationName()).arg(tApplication::applicationName());

            uint thisId = ++tNotificationPrivateByOS::current;
            tNotificationPrivateByOS::notifications.insert(thisId, this);
            connect(this, &tNotification::destroyed, this, [=] {
                tNotificationPrivateByOS::notifications.remove(thisId);
            });

            QString xmlContents;
            QXmlStreamWriter xmlWriter(&xmlContents);

            xmlWriter.writeStartDocument();
            xmlWriter.writeStartElement("toast");
            xmlWriter.writeStartElement("visual");
            xmlWriter.writeStartElement("binding");
            xmlWriter.writeAttribute("template", "ToastGeneric");
            xmlWriter.writeTextElement("text", sum);
            xmlWriter.writeTextElement("text", txt);
            xmlWriter.writeEndElement(); //binding
            xmlWriter.writeEndElement(); //visual
            xmlWriter.writeStartElement("audio");
            if (noSound) xmlWriter.writeAttribute("silent", "true");
            xmlWriter.writeEndElement(); //audio
            xmlWriter.writeStartElement("actions");
            for (QString key : actions.keys()) {
                xmlWriter.writeEmptyElement("action");
                xmlWriter.writeAttribute("activationType", "background");
                xmlWriter.writeAttribute("arguments", QJsonDocument(QJsonObject({{"key", key}, {"id", static_cast<int>(thisId)}})).toJson());
                xmlWriter.writeAttribute("content", actions.value(key));
            }
            xmlWriter.writeEndElement(); //actions
            xmlWriter.writeEndElement(); //toast
            xmlWriter.writeEndDocument();

            //Post
            XmlDocument toastXml;
            toastXml.LoadXml(xmlContents.toStdWString());

            ToastNotification toast(toastXml);
            toast.Dismissed([=](ToastNotification sender, ToastDismissedEventArgs args) {
                if (isTransient && args.Reason() != ToastDismissalReason::ApplicationHidden) {
                    ToastNotificationManager::CreateToastNotifier(amuid.toStdWString()).Hide(sender);

                    emit dismissed();

                    if (deleteWhenDone) {
                        this->deleteLater();
                    }
                }
            });
            ToastNotificationManager::CreateToastNotifier(amuid.toStdWString()).Show(toast);
        }
    } else {
        shouldUseFallback = true;
    }

    if (shouldUseFallback) {
        QSystemTrayIcon* ic = new QSystemTrayIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
        ic->show();
        ic->showMessage(sum, txt, QSystemTrayIcon::Information, 10000);
        QTimer::singleShot(10000, [=] {
            ic->hide();
            ic->deleteLater();
        });
    }
}

void tNotification::initialize() {
    dd = new tNotificationPrivateByOS();
}

void tNotification::destroy() {
    delete dd;
}

void tNotification::dismiss() {

}

struct CallbackFactory : winrt::implements<CallbackFactory, IClassFactory>
{
    HRESULT __stdcall CreateInstance( IUnknown* outer, GUID const& iid, void** result) noexcept;
    HRESULT __stdcall LockServer(BOOL) noexcept;
};

struct Callback : winrt::implements<Callback, INotificationActivationCallback>
{
    HRESULT __stdcall Activate(LPCWSTR app, LPCWSTR args, NOTIFICATION_USER_INPUT_DATA const* data, ULONG count) noexcept;
};

void tNotificationWindows::initialise(QString classId)
{
    tNotificationWindows::classId = classId;

    if (QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows10) {
        try {
            DWORD registration{};
            CLSID clsid;

            winrt::check_hresult(::CLSIDFromString(classId.toStdWString().c_str(), &clsid));
            winrt::check_hresult(::CoRegisterClassObject(
                clsid,
                winrt::make<CallbackFactory>().get(),
                CLSCTX_LOCAL_SERVER,
                REGCLS_MULTIPLEUSE,
                &registration));
        } catch (...) {
            //whatever
            qDebug() << "Exception caught in tNotificationWindows::initialise";
        }
    }
}

HRESULT CallbackFactory::CreateInstance(IUnknown *outer, const GUID &iid, void **result) noexcept {
    *result = nullptr;

    if (outer) {
        return CLASS_E_NOAGGREGATION;
    }

    return winrt::make<Callback>()->QueryInterface(iid, result);
}

HRESULT CallbackFactory::LockServer(BOOL) noexcept {
    return S_OK;
}

HRESULT Callback::Activate(LPCWSTR app, LPCWSTR args, const NOTIFICATION_USER_INPUT_DATA *data, ULONG count) noexcept {
    try {
        QString qargs = QString::fromWCharArray(args);
        QJsonObject json = QJsonDocument::fromJson(qargs.toUtf8()).object();

        tNotification* notification = tNotificationPrivateByOS::notifications.value(static_cast<uint>(json.value("id").toInt()), nullptr);
        if (notification) {
            emit notification->actionClicked(json.value("key").toString());
        }

        return S_OK;
    } catch (...) {
        qDebug() << "Caught an exception on activate";
        return winrt::to_hresult();
    }
}
