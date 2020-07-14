#ifndef MW_SETPROJECTNAMEDIALOG_H
#define MW_SETPROJECTNAMEDIALOG_H

#include <QDialog>

namespace Ui {
class MW_setProjectNameDialog;
}

class MW_setProjectNameDialog : public QDialog
{
    Q_OBJECT

public:
	void	readConfig();
	void setConfig();
	QString getProjectName();

    explicit MW_setProjectNameDialog(QWidget *parent = 0);
    ~MW_setProjectNameDialog();
	QString projectName;

private slots:
    void on_MW_setProjectNameDialog_accepted();

private:
    Ui::MW_setProjectNameDialog *ui;
};

#endif // MW_SETPROJECTNAMEDIALOG_H
