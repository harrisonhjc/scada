#ifndef SV_DATABASE_H
#define SV_DATABASE_H

#include <QSqlDatabase> 
#include <QSqlQuery> 
#include <QSqlError>



class SV_database
{
public:
    static QSqlDatabase db;
	static QSqlQuery query; 
 
	static bool findAll(QStringList& dataList);
	static bool clearDb();
	static bool findData(const QString& timeStart, const QString& timeEnd, QStringList& dataList);
	static bool insertRow(int color, const QString& time, const QString& content);
	static bool createConnection();
	static void closeConnection();
	
};


#endif // SV_DATABASE_H
