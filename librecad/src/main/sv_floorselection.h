#ifndef SV_FLOORSELECTION_H
#define SV_FLOORSELECTION_H

#include <QDialog>
#include <QModelIndex>
#include "mv_treeview.h"
#include <QStandardItemModel>

class SV_FloorSelection : public QDialog
{
    Q_OBJECT
public:
    explicit SV_FloorSelection(QWidget *parent = 0);
	~SV_FloorSelection();
	void AlertNotification(QString fn, Qt::GlobalColor color);
    void init(QMap<QString,int>& r,QMap<QString,int>& g,
            QMap<QString,int>& b,QMap<QString,int>& m);
	void clearAllItems();
	QString getMapName(){
		return mapName;
	}
	
	
	
signals:
	void getMapName(const QString &s);

public slots:
	
	void Ondoubleclicktree(const QModelIndex&);
    void quit();

protected:
	void closeEvent(QCloseEvent *event);
	
private:
	MV_TreeView* view;
	QString mapName;

	void updateAlertStatus();
	void insertRow(QModelIndex& index,int rowCount, QString text);
	void insertChild(QModelIndex& index, QString col0, QString col1);
	void readConfig();

	QMap<QString,int>	redAlertMap;
	QMap<QString,int>	greenAlertMap;
	QMap<QString,int>	blueAlertMap;
	//QMap<QString,int>	yellowAlertMap;
	QMap<QString,int>	magentaAlertMap;
	

};

#endif // SV_FLOORSELECTION_H
