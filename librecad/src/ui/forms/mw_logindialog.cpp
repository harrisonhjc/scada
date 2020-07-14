#include "mw_logindialog.h"
#include "ui_mw_logindialog.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QtDebug>

MW_loginDialog::MW_loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MW_loginDialog)
{
    ui->setupUi(this);
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

MW_loginDialog::~MW_loginDialog()
{
    delete ui;
}

void MW_loginDialog::setAuthorityInfo(QStringList& list)
{
    userList = list;

	
}

QString MW_loginDialog::getLoginUser()
{
    return loginUser;
}


void MW_loginDialog::on_pbLogin_pressed()
{
	
   loginUser = ui->leUser->text();
   QString passwd = ui->lePasswd->text();
   
   if(userList.size() == 0){ //auth file not exist.
   		
   		// admin
        //static const char pdData[] = {0x21,0x23,0x2f,0x29,0x7a,0x57,0xa5,0xa7,0x43,0x89,
        //                            0x4a,0x0e,0x4a,0x80,0x1f,0xc3};
        //scada
        static const unsigned char pdData[] = {0x54,0x79,0xbf,0x0e,0xbb,0x6d,0xc9,0x2d,
                                   0xfc,0x06,0xcb,0x80,0x32,0x06,0x89,0xb9};
		
        QByteArray pdArray = QByteArray::fromRawData((char*)pdData, sizeof(pdData));
		QByteArray hash = QCryptographicHash::hash(passwd.toLatin1(), QCryptographicHash::Md5);
		if(loginUser != "scada"){
			QMessageBox::about(NULL,"SCADA" ,tr("MV_user does not exist."));
			return;
		}
        if(loginUser == "scada" && pdArray == hash){
			accept();
			done(QDialog::Accepted);
            return;
   		}else{
   			QMessageBox::about(NULL,"SCADA" ,tr("MV_password is not correct."));
   			return;
   		}
   }
   int i = userList.indexOf(loginUser);
   if(i == -1){
   		qDebug() << "user not found.";
   	   QMessageBox::about(NULL,"SCADA" ,tr("MV_user does not exist."));
	   ui->leUser->setText("");
	   return;
   }else{
	   QByteArray hash = QCryptographicHash::hash(passwd.toLatin1(), QCryptographicHash::Md5);
	   if(userList[i+1] == hash.toBase64()){
	   		accept();
			done(QDialog::Accepted);
	   		return;
	   }else
	   QMessageBox::about(NULL,"SCADA" ,tr("MV_password is not correct."));
   }

}


