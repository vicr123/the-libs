#ifndef TTOAST_H
#define TTOAST_H

#include <QObject>
#include <QTimer>
#include <QBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include "tpropertyanimation.h"

class tToast : public QObject
{
    Q_OBJECT

public:
    explicit tToast(QObject *parent = 0);
    ~tToast();

    void setTitle(QString title);
    QString title();

    void setText(QString text);
    QString text();

    void setActions(QMap<QString, QString> actions);
    QMap<QString, QString> actions();

signals:
    void actionClicked(QString key);
    void dismissed();

public slots:
    void show(QWidget* parent);
    void dismiss();

private:
    QMap<QString, QString> actn;

    QWidget* toastWidget;
    QLabel *titleLabel, *textLabel;
    QBoxLayout* buttons;
    QTimer* hideTimer;
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // TTOAST_H
