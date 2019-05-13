#include "nativeeventfilter.h"

#include <QDebug>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

NativeEventFilter::NativeEventFilter(QObject *parent) : QObject(parent), QAbstractNativeEventFilter()
{

}

bool NativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result) {
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_POWERBROADCAST) {
            if (msg->wParam == PBT_POWERSETTINGCHANGE) {
                POWERBROADCAST_SETTING* powerSettings = reinterpret_cast<POWERBROADCAST_SETTING*>(msg->lParam);
                if (powerSettings->PowerSetting == GUID_POWER_SAVING_STATUS) {
                    bool powerSaverOn = powerSettings->Data[0];
                    emit powerStretchChanged(powerSaverOn);
                }
            }
        }
    }
#endif
    return false;
}
