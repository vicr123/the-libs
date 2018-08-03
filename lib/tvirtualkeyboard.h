#ifndef TVIRTUALKEYBOARD_H
#define TVIRTUALKEYBOARD_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnectionInterface>

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

private:
    tVirtualKeyboard();

    QDBusInterface* keyboardInterface;
};

#endif // TVIRTUALKEYBOARD_H
