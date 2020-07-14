#ifndef SV_LOGDIALOG_H
#define SV_LOGDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QColor>
#include <QScrollBar>


namespace Ui {
class SV_logDialog;
}

class SV_logDialog : public QDialog
{
    Q_OBJECT

public:
	enum LoadType {
			Type_DB,
			Type_File,
		};
	
    explicit SV_logDialog(QWidget *parent = 0);
    ~SV_logDialog();

	
	QTimer *timeTimer;

	void loadLogFile(QString&);
	void setCurrentTime();
	void addEventLog(Qt::GlobalColor, const QString& txt);

private:
    LoadType loadType;
	bool loadingItems;
	int itemCount;
	QScrollBar *verticalScrollBar;
    Ui::SV_logDialog *ui;
	QStringList dataList;

public slots:
	void slotSbValueChanged(int);
	void slotTimer();
	void slotPrint();
	void slotOpen();
	void slotSave();
	void slotLoadDb();
	void slotFileDialogSelected(int);
private slots:
    void on_cbAlert_stateChanged(int arg1);
    void on_cbFault_stateChanged(int arg1);
    void on_cbSwitch_stateChanged(int arg1);
    void on_cbResponse_stateChanged(int arg1);
    void on_cbOthers_stateChanged(int arg1);
};

#endif // SV_LOGDIALOG_H
