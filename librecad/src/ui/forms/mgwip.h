#ifndef MGWIP_H
#define MGWIP_H

#include <QDialog>

namespace Ui {
class Mgwip;
}

class Mgwip : public QDialog
{
    Q_OBJECT

public:
    explicit Mgwip(QWidget *parent = 0);
    ~Mgwip();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Mgwip *ui;
    QString ip;
	void readConfig();
	void setConfig();


};

#endif // MGWIP_H
