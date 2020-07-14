#ifndef MW_LOGINDIALOG_H
#define MW_LOGINDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QString>

namespace Ui {
class MW_loginDialog;
}

class MW_loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MW_loginDialog(QWidget *parent = 0);
    ~MW_loginDialog();
	void setAuthorityInfo(QStringList& list);
    QString getLoginUser();
	
	
private slots:
    void on_pbLogin_pressed();



private:
	QString loginUser;
	QStringList userList;
    Ui::MW_loginDialog *ui;
};

#endif // MW_LOGINDIALOG_H
