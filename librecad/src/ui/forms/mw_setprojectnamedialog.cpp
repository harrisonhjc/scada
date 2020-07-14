#include "mw_setprojectnamedialog.h"
#include "ui_mw_setprojectnamedialog.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include "mw_window.h"

MW_setProjectNameDialog::MW_setProjectNameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MW_setProjectNameDialog)
{
    ui->setupUi(this);
	readConfig();
}

MW_setProjectNameDialog::~MW_setProjectNameDialog()
{
    delete ui;
}

void MW_setProjectNameDialog::readConfig()
{
	QStringList dataList;

	QString fileName = MW_Window::getScadaSystemFolder() + "project.profile";
	
	
    QFile fIn(fileName);
    if (fIn.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream sIn(&fIn);
      while (!sIn.atEnd())
        dataList += sIn.readLine();
    } else {
      return ;
    }
	fIn.close();
    projectName = dataList[0];
	ui->leProjectName->setText(projectName);
}

void MW_setProjectNameDialog::setConfig()
{
    projectName = ui->leProjectName->text();
	MW_Window::setProjectFolder(projectName);
	QString fileName = MW_Window::getScadaSystemFolder() + "project.profile";
	QFile fOut(fileName);
    if(fOut.open(QFile::WriteOnly | QFile::Text)){
		QTextStream s(&fOut);
      	s << projectName;
    }else{

      return ;
    }
	fOut.flush();
    fOut.close();
}

QString MW_setProjectNameDialog::getProjectName()
{
	return projectName;
}


void MW_setProjectNameDialog::on_MW_setProjectNameDialog_accepted()
{
    setConfig();
}
