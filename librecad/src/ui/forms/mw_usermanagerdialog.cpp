#include "mw_usermanagerdialog.h"
#include "ui_mw_usermanagerdialog.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>
#include "mw_window.h"

MW_userManagerDialog::MW_userManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MW_userManagerDialog)
{
    ui->setupUi(this);
	authBuilding = Qt::Unchecked;
	authEditing = Qt::Unchecked;
	authSupervise = Qt::Unchecked;
    readConfig();

}

MW_userManagerDialog::~MW_userManagerDialog()
{
    delete ui;
}

void MW_userManagerDialog::on_checkBoxBuilding_stateChanged(int arg1)
{
	authBuilding = ui->checkBoxBuilding->checkState();

}

void MW_userManagerDialog::on_checkBoxSensor_stateChanged(int arg1)
{
	authEditing = ui->checkBoxSensor->checkState();
}

void MW_userManagerDialog::on_checkBoxSupervise_stateChanged(int arg1)
{
	authSupervise = ui->checkBoxSupervise->checkState();
}

QString MW_userManagerDialog::getAuth()
{
	QString auth = "";
	
	auth += ((authBuilding == Qt::Checked) ? "x":"h");
	auth += ((authEditing == Qt::Checked) ? "x":"h");
	auth += ((authSupervise == Qt::Checked) ? "x":"h");
	return auth;
	

	return auth;
}

void MW_userManagerDialog::setAuth(QString& auth)
{
	if(auth.at(0) == QChar('x'))
		ui->checkBoxBuilding->setCheckState(Qt::Checked);
	else
		ui->checkBoxBuilding->setCheckState(Qt::Unchecked);

	if(auth.at(1) == QChar('x'))
		ui->checkBoxSensor->setCheckState(Qt::Checked);
	else
        ui->checkBoxSensor->setCheckState(Qt::Unchecked);


	if(auth.at(2) == QChar('x'))
		ui->checkBoxSupervise->setCheckState(Qt::Checked);
	else
		ui->checkBoxSupervise->setCheckState(Qt::Unchecked);
	 
}

void MW_userManagerDialog::on_lePasswd_editingFinished()
{
    passwd = ui->lePasswd->text();
}

void MW_userManagerDialog::on_lePasswd_2_editingFinished()
{
    passwd2 = ui->lePasswd_2->text();
}



void MW_userManagerDialog::on_pBaddUSer_pressed()
{

    if(passwd2 != passwd){
        QMessageBox::about(NULL,"SCADA" ,tr("MW_passwords are not the same."));
        return;
    }

    //check if new user
    QByteArray md5;
    QString newUser = ui->cBUser->lineEdit()->text();
    if(newUser == userName){ //user existed. update password.
    	QMessageBox message(QMessageBox::Warning,"Information",tr("MW_user exists,do you want to update password?"),
			               QMessageBox::Yes|QMessageBox::No,NULL);  
	    if (message.exec()==QMessageBox::Yes){ 
			int index = userList.indexOf(userName);
			md5 = QCryptographicHash::hash(passwd.toLatin1(), QCryptographicHash::Md5);
			userList[index+1] = md5.toBase64();
			userList[index+2] = getAuth();
			setConfig();
			
	    }else{
	        return;
	    }
    }else{ //new user
		userList.append(newUser);
		md5 = QCryptographicHash::hash(passwd.toLatin1(), QCryptographicHash::Md5);
		userList.append(md5.toBase64());
		userList.append(getAuth());
		setConfig();
		
    }
     QMessageBox::about(NULL,"SCADA" ,tr("MV_Add user successfully."));
	
}

void MW_userManagerDialog::on_pBDeleteUser_pressed()
{
	if(passwd2 != passwd){
        QMessageBox::about(NULL,"SCADA" ,tr("MW_passwords are not the same."));
        return;
    }

    //check if user exists
    QByteArray md5;
    QString newUser = ui->cBUser->lineEdit()->text();
    if(newUser == userName){ //user existed. delete.
    	QMessageBox message(QMessageBox::Warning,"Information",tr("MW_do you want to delete user?"),
			               QMessageBox::Yes|QMessageBox::No,NULL);  
	    if (message.exec()==QMessageBox::Yes){ 
			int index = userList.indexOf(userName);
			userList.removeAt(index);
			userList.removeAt(index+1);
			userList.removeAt(index+2);
			setConfig();
			
	    }else{
	        return;
	    }
    }else{ //new user
		QMessageBox::about(NULL,"SCADA" ,tr("MV_User does not exist."));
		return;
    }
	 QMessageBox::about(NULL,"SCADA" ,tr("MV_Delete user successfully."));
}

void MW_userManagerDialog::on_pBCancel_pressed()
{
	close();
}

void MW_userManagerDialog::on_cBUser_currentIndexChanged(const QString &arg1)
{
    userName = arg1;
    int index = userList.indexOf(userName);
    setAuth(userList[index+2]);

}
void MW_userManagerDialog::readConfig()
{
	//format : name\n passwd\n auth\n
	//auth format : xxx  3 bytes,x or h , h:no,x:yes, B0:building,B1:sensor editing,B2:supervise
	QString fileName = MW_Window::getScadaSystemFolder()  + "user.profile";
    QFile fIn(fileName);
    if (fIn.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream sIn(&fIn);
      while (!sIn.atEnd())
        userList += sIn.readLine();
    } else {
      return ;
    }
	fIn.close();
	QString user;
	QString passwd;
	QString auth;
	for(int i=0;i<userList.size();i+=3){
		user = userList[i];
		passwd = userList[i+1];
		auth = userList[i+2];
        ui->cBUser->addItem(user);
	}
}

void MW_userManagerDialog::setConfig()
{
	//check if update user data
	QString fileName = MW_Window::getScadaSystemFolder() + "user.profile";
    QFile fOut(fileName);
    if(fOut.open(QFile::WriteOnly | QFile::Text)){
		QTextStream s(&fOut);
		for(int i=0;i<userList.size();i+=3){
			s << userList[i]  << "\n";
			s << userList[i+1]<< "\n";
			s << userList[i+2]<< "\n";
		}
      	
    }else{

      return ;
    }
	fOut.flush();
    fOut.close();
}


void MW_userManagerDialog::on_cBUser_currentTextChanged(const QString &arg1)
{

}

