#ifndef SV_RTUSENDCMDDIALOG_H
#define SV_RTUSENDCMDDIALOG_H

#include <QDialog>
#include <qlocalsocket.h>
#include "rs_image.h"


namespace Ui {
class SV_RtuSendCmdDialog;
}

class SV_RtuSendCmdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SV_RtuSendCmdDialog(QWidget *parent = 0);
    ~SV_RtuSendCmdDialog();
	void setImage(RS_Image& e);

private slots:
    void on_pbOn_pressed();

    void on_pbOff_pressed();

    void on_pbOK_pressed();

private:
    Ui::SV_RtuSendCmdDialog *ui;
	RS_Image* image;
	QLocalSocket *socketCmd;
};

#endif // SV_RTUSENDCMDDIALOG_H
