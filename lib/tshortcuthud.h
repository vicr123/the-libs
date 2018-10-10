#ifndef TSHORTCUTHUD_H
#define TSHORTCUTHUD_H

#include "the-libs_global.h"
#include <QWidget>
#include <QShortcut>
#include <QPaintEvent>
#include <QPainter>

namespace Ui {
    class tShortcutHud;
}

class THELIBSSHARED_EXPORT tShortcutHud : public QWidget
{
        Q_OBJECT

    public:
        explicit tShortcutHud(QWidget *parent = nullptr);
        ~tShortcutHud();

        enum ShortcutHudSide {
            Leading,
            Trailing
        };

        class ShortcutGroup {
            public:
                ShortcutGroup();
                ShortcutGroup(QList<QShortcut*> shortcuts, QString shortcutText, ShortcutHudSide side = Leading);

                void addShortcut(QShortcut* shortcut);
                void setText(QString text);
                void setSize(ShortcutHudSide side);

                QList<QShortcut*> shortcuts();
                QString shortcutText();
                ShortcutHudSide side();

            private:
                QList<QShortcut*> scs;
                QString st;
                ShortcutHudSide sd;
        };

    public slots:
        void newShortcut(QShortcut* shortcut, QString shortcutText, ShortcutHudSide side = Leading);
        void newShortcut(ShortcutGroup group);

    private:
        Ui::tShortcutHud *ui;

        QPixmap getKeyIcon(QString key);

        void resizeToParent();
        void paintEvent(QPaintEvent* paintEvent);
        bool eventFilter(QObject* watched, QEvent* event);
};

#endif // TSHORTCUTHUD_H
