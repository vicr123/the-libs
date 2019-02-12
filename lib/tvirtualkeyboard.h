#ifndef TVIRTUALKEYBOARD_H
#define TVIRTUALKEYBOARD_H

#include "the-libs_global.h"
#include <QObject>

#ifdef T_OS_UNIX_NOT_MAC
#include <QDBusInterface>
#include <QDBusConnectionInterface>
#endif

class THELIBSSHARED_EXPORT tVirtualKeyboard : public QObject
{
    Q_OBJECT
public:
    static tVirtualKeyboard* instance();

Q_SIGNALS:
    void keyboardVisibleChanged(bool isVisible);

public Q_SLOTS:
    int height();
    void showKeyboard();
    void hideKeyboard();
    bool isKeyboardRunning();

    bool keyboardVisible();
    void setKeyboardType(QString type);
    void setPredictive(bool predictive);
    void setHidden(bool hidden);
    void setSensitive(bool sensitive);
    void setAutoUppercase(bool autoUpperCase);
    void setEnterKeyType(QString type);

private:
    tVirtualKeyboard();

#ifdef T_OS_UNIX_NOT_MAC
    QDBusInterface* keyboardInterface;
#endif
};

#endif // TVIRTUALKEYBOARD_H
