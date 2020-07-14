#include "sensordlg.h"
#include "ui_sensordlg.h"

SensorDlg::SensorDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SensorDlg)
{
    ui->setupUi(this);

	QString fn;
	for(int i=0;i<5;i++){
		for(int j=0;j<9;j++){
			fn = QString(":/actions/sensor%1.png").arg((j+1) + i*9);
			QTableWidgetItem* item = new QTableWidgetItem(QIcon(fn),"");
			item->setWhatsThis(fn);
			ui->twSensorTb->setItem(j, i, item);
			
		}
	}
	connect(ui->twSensorTb, SIGNAL(cellClicked(int,int)), this, SLOT(slotOnClick(int,int)));
}

SensorDlg::~SensorDlg()
{
    delete ui;
}

void SensorDlg::slotOnClick(int row, int column)
{
	QTableWidgetItem* item = ui->twSensorTb->item(row,column);
	if(item == NULL)
		return;

	fnSensor = item->whatsThis();
	close();
}

QString SensorDlg::getSensorFn()
{
	return fnSensor;
}

