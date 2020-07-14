#include "sv_rtusendcmddialog.h"
#include "ui_sv_rtusendcmddialog.h"

SV_RtuSendCmdDialog::SV_RtuSendCmdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SV_RtuSendCmdDialog)
{
    ui->setupUi(this);
    setWindowTitle("感測器");
	socketCmd = new QLocalSocket(this);
}

SV_RtuSendCmdDialog::~SV_RtuSendCmdDialog()
{
    delete ui;
	delete socketCmd;
}

void SV_RtuSendCmdDialog::setImage(RS_Image& e) {
    image = &e;

    QString rtuAddress = image->getRtuAddress();
    ui->leNode->setText(rtuAddress.mid(0,2));
    ui->leZone->setText(rtuAddress.mid(2,4));
    ui->leLoop->setText(rtuAddress.mid(6,2));
    ui->leAddr1->setText(rtuAddress.mid(8,3));
    ui->leAddr2->setText(rtuAddress.mid(11));
    ui->leName->setText(image->getLocation());
}

void SV_RtuSendCmdDialog::on_pbOn_pressed()
{
	
	QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QString buf = "C1_ON:" + image->getRtuAddress();
	//cmd format: C1:ON:address
	out << buf;
    out.device()->seek(0);
	socketCmd->abort();
	socketCmd->connectToServer("SCADA");
    socketCmd->write(block);
    socketCmd->flush();
	
    
}

void SV_RtuSendCmdDialog::on_pbOff_pressed()
{
	QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QString buf = "C1_OFF:" + image->getRtuAddress();
    out <<  buf;
    out.device()->seek(0);
	socketCmd->abort();
	socketCmd->connectToServer("SCADA");
    socketCmd->write(block);
    socketCmd->flush();
}

void SV_RtuSendCmdDialog::on_pbOK_pressed()
{
	socketCmd->disconnectFromServer();
	socketCmd->close();
    close();
}
