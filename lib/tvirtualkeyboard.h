#ifndef TVIRTUALKEYBOARD_H
#define TVIRTUALKEYBOARD_H

#include "the-libs_global.h"
#include <QObject>

#ifdef T_OS_UNIX_NOT_MAC
#include <QDBusInterface>
#include <QDBusConnectionInterface>
#endif

class tVirtualKeyboard : public QObject
{
    Q_OBJECT
public:
    static tVirtualKeyboard* instance();

signals:
    void keyboardVisibleChanged(bool isVisible);

public slots:
    int height();
    void showKeyboard();
    void hideKeyboard();
    bool isKeyboardRunning();

    bool keyboardVisible();
    void setKeyboardType(QString type);

private:
    tVirtualKeyboard();

#ifdef T_OS_UNIX_NOT_MAC
    QDBusInterface* keyboardInterface;
#endif
};

#endif // TVIRTUALKEYBOARD_H
