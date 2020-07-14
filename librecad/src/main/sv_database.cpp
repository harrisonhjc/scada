#include "sv_database.h"
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSqlQueryModel>
#include <QCoreApplication>
#include "MW_Window.h"

QSqlDatabase SV_database::db;


bool SV_database::createConnection()  
{  
	if(MW_Window::getProjectFolder() == NULL)
		return false;
	
    db = QSqlDatabase::addDatabase("QSQLITE");
	QString dbName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + "scada.db";
    db.setDatabaseName(dbName); 
    
    QTime t;
	t.start();
    bool dbFound = false;
	while(t.elapsed()<5000){
    	QCoreApplication::processEvents();
    	if(db.open()){
    		qDebug() << "SV_database::createConnection:" << dbName << " found.";
    		dbFound = true;
			break;
    	}else{
    		qDebug() << "SV_database::createConnection:" << dbName << " not found.";
    	}
	}
	if(dbFound == false)
		return false;

   
    QSqlQuery query(db);  
	query.exec("create table Events (id INTEGER PRIMARY KEY AUTOINCREMENT, color INTEGER, time TEXT, content TEXT)");
    return true;
}  

bool SV_database::insertRow(int color, const QString& time, const QString& content)
{
	bool ret;
	QString cmd = QString("insert into Events (color,time,content) values (%1,'%2','%3')").arg(color).arg(time).arg(content);
	QSqlQuery query(db);  
	query.prepare(cmd);
    ret = query.exec(cmd);
	return ret;
	
}

bool SV_database::findData(const QString& timeStart, const QString& timeEnd, QStringList& dataList)
{
	QSqlQueryModel *model = new QSqlQueryModel;  
    QString cmd = QString("select * from Events where datetime([time]) > datetime('%1') and \
                            datetime([time]) < datetime('%2')").arg(timeStart).arg(timeEnd);
	model->setQuery(cmd);
    if (model->lastError().isValid())
		return false;
	
	for (int i = 0; i < model->rowCount(); ++i) {
	    QSqlRecord record = model->record(i);
        QString color = record.value("color").toString();
	    QString time = record.value("time").toString();
	    QString content = record.value("content").toString();
        dataList << color << time << content;
	}
	return true;
}

bool SV_database::findAll(QStringList& dataList)
{
	QSqlQueryModel *model = new QSqlQueryModel;  
    QString cmd = QString("select * from Events");
	model->setQuery(cmd);  
    if (model->lastError().isValid())
		return false;

	while(model->canFetchMore())
	model->fetchMore();

	//qDebug() << "db count:" << model->rowCount();
	
	for (int i = model->rowCount()-1; i >= 0 ; i--) {
	    QSqlRecord record = model->record(i);
        QString color = record.value("color").toString();
	    QString time = record.value("time").toString();
	    QString content = record.value("content").toString();
        dataList << color << time << content;
	}
	return true;
}

bool SV_database::clearDb()
{
	QSqlQueryModel *model = new QSqlQueryModel;  
    QString cmd = QString("delete from Events");
	model->setQuery(cmd);  
    if (model->lastError().isValid()){
		qDebug() << model->lastError().text();
		return false;
    }
	else
		return true;
}


void SV_database::closeConnection(){
    db.close();
}

