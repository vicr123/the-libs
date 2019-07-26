#ifndef TABOUTDIALOG_H
#define TABOUTDIALOG_H

#include <QDialog>
#include "the-libs_global.h"

namespace Ui {
    class tAboutDialog;
}

class THELIBSSHARED_EXPORT tAboutDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit tAboutDialog(QWidget *parent = nullptr);
        ~tAboutDialog();

    private slots:
        void on_okButton_clicked();

    private:
        Ui::tAboutDialog *ui;
};

#endif // TABOUTDIALOG_H
