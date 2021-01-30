#ifndef CSDBUTTONBOX_P_H
#define CSDBUTTONBOX_P_H

#include <QWidget>

class CsdButtonBox;
struct CsdButtonBoxPrivate {
    QWidget* parentWidget;
    CsdButtonBox* box;

#ifdef Q_OS_MAC
    void windowShown();
    void windowHidden();

    QWidget* closeWidget;
    QWidget* minWidget;
    QWidget* fsWidget;
#endif
};

#endif // CSDBUTTONBOX_P_H
