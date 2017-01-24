#ifndef TTOAST_H
#define TTOAST_H

#include <QObject>
#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include "tpropertyanimation.h"

class tToast : public QObject
{
    Q_OBJECT
public:
    explicit tToast(QObject *parent = 0);
    ~tToast();

    void setTitle(QString title);
    void setText(QString text);
    void setActions(QMap<QString, QString> actions);
    void setTimeout(int timeout);
signals:
    void dismissed();
    void actionClicked(QString key);
    void doDefaultOption();

public slots:
    void show(QWidget* parent);
    void dismiss();
    void announceAction(QString action);

private:
    QMap<QString, QString> actn;

    QWidget *widget, *parent;
    QLabel *titleLabel, *textLabel;
    QBoxLayout* buttonLayout;

    tVariantAnimation* timer;
    int timerProg = 0;

    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // TTOAST_H
