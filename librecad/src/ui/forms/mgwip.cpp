#include "mgwip.h"
#include "ui_mgwip.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include "mw_window.h"

Mgwip::Mgwip(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Mgwip)
{
    ui->setupUi(this);
    readConfig();
}

Mgwip::~Mgwip()
{
    delete ui;
}

void Mgwip::on_buttonBox_accepted()
{
    
    setConfig();
}

void Mgwip::readConfig()
{
	QString fileName = MW_Window::getScadaSystemFolder() + "mgw.profile";
	
    QFile fIn(fileName);
    if (fIn.open(QFile::ReadOnly | QFile::Text)) {
    	QTextStream sIn(&fIn);
        ip = sIn.readLine();
    }
	fIn.close();
    if(ip.length() == 0){
    	ip = "192.168.0.162";
    }
    ui->lineEdit->setText(ip);
	
}

void Mgwip::setConfig()
{
    ip = ui->lineEdit->text();
	QString fileName = MW_Window::getScadaSystemFolder() + "mgw.profile";
	QFile fOut(fileName);
    if(fOut.open(QFile::WriteOnly | QFile::Text)){
		QTextStream s(&fOut);
      	s << ip;
    }
	fOut.flush();
    fOut.close();
}

