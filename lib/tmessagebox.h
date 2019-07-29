#ifndef TMESSAGEBOX_H
#define TMESSAGEBOX_H

#include "the-libs_global.h"
#include <QMessageBox>

class THELIBSSHARED_EXPORT tMessageBox : public QMessageBox
{
    Q_OBJECT
    public:
        explicit tMessageBox(QWidget *parent = nullptr);
        ~tMessageBox();

    signals:

    public slots:
        void setWindowTitle(QString windowTitle);
        void setText(QString text);

        int exec();
};

#endif // TMESSAGEBOX_H
