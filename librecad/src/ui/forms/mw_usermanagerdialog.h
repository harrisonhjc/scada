#ifndef MW_USERMANAGERDIALOG_H
#define MW_USERMANAGERDIALOG_H

#include <QDialog>

namespace Ui {
class MW_userManagerDialog;
}

class MW_userManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MW_userManagerDialog(QWidget *parent = 0);
    ~MW_userManagerDialog();
	QString userName;
	QString passwd;
	QString passwd2;
	Qt::CheckState authBuilding;
	Qt::CheckState authEditing;
	Qt::CheckState authSupervise;
	QStringList userList;
	
	void readConfig();
	void setConfig();
	QString getAuth();
	void setAuth(QString&);
private slots:
    void on_checkBoxBuilding_stateChanged(int arg1);

    void on_checkBoxSensor_stateChanged(int arg1);

    void on_checkBoxSupervise_stateChanged(int arg1);

    void on_lePasswd_editingFinished();

    void on_lePasswd_2_editingFinished();

   
    void on_pBaddUSer_pressed();

    void on_pBDeleteUser_pressed();

    void on_pBCancel_pressed();

    void on_cBUser_currentIndexChanged(const QString &arg1);

    void on_cBUser_currentTextChanged(const QString &arg1);

private:
    Ui::MW_userManagerDialog *ui;
};

#endif // MW_USERMANAGERDIALOG_H
