#ifndef TNOTIFICATIONWIN_H
#define TNOTIFICATIONWIN_H

#include <QString>

namespace winrt {
    namespace Windows {
        namespace UI {
            namespace Notifications {
                struct ToastNotifier;
            }
        }
    }
}

class tNotificationWindows {
    public:
        static void initialise(QString classId);

        static QString classId;

        static winrt::Windows::UI::Notifications::ToastNotifier createToastNotifier();
};

#endif // TNOTIFICATIONWIN_H
