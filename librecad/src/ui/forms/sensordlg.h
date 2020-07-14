#ifndef SENSORDLG_H
#define SENSORDLG_H

#include <QDialog>

namespace Ui {
class SensorDlg;
}

class SensorDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SensorDlg(QWidget *parent = 0);
    ~SensorDlg();
	QString getSensorFn();

private:
    Ui::SensorDlg *ui;
	QString fnSensor;

public slots:
	void slotOnClick(int,int);
};

#endif // SENSORDLG_H
