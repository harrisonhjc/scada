#include "workerbackupdb.h"
#include <QDateTime>
#include <QStringList>
#include "MW_Window.h"
#include <QCoreApplication>
#include <QDebug>

void workerBackupDb::doWork()
{
    QDateTime time;

	time = QDateTime::currentDateTime(); 
    //qDebug() << "workerBackupDb::check:" << time.toString("yyyy-MM-dd-hhmm");

    if(time.time().hour() == 0 &&
	   time.time().minute() >= 0 &&
       time.time().minute() < 10){

        QString buf;
		QStringList dataList;

		qDebug() << "workerBackupDb::doWork:" << time.toString("yyyy-MM-dd-hhmm");
		
		SV_database::findAll(dataList);

		buf = time.toString("yyyy-MM-dd-hhmm") + ".log"; 
		QString fileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + buf;
		if(fileName.isEmpty())
			return;

		/*
		int count = 1;
		QFile fOut(fileName);
		QString text;
		if(fOut.open(QFile::WriteOnly | QFile::Text)){
			QTextStream s(&fOut);
			for(int i=0;i<dataList.size();i += 3){
				text = QString("%1 %2 %3").arg(count).arg(dataList[i+1]).arg(dataList[i+2]);
				s << text << "\n";
				count++;
			}
		}else{
		  qDebug() << "error opening log output file\n";
		  return ;
		}
		fOut.flush();
		fOut.close();
		*/
		///////////////////////////////////////////////////
		int count = 1;
	    QFile fOut(fileName);
		QString text;
		Qt::GlobalColor color;
		QString colorStr;
		QString countStr;
	    if(fOut.open(QFile::WriteOnly | QFile::Text)){
			QTextStream s(&fOut);
			for(int i=0;i<dataList.size();i += 3){
				color = (Qt::GlobalColor)dataList[i].toInt();
				if(color == Qt::red)
					colorStr = "R";
				else
				if(color == Qt::green)
					colorStr = "G";
				else
				if(color == Qt::blue)
					colorStr = "B";
				else
				if(color == Qt::magenta)
					colorStr = "P";

				countStr.sprintf("%04d",count);
				text = QString("%1 %2 %3 %4").arg(countStr).arg(colorStr).arg(dataList[i+1]).arg(dataList[i+2]);
				s << text << "\n";
		        count++;
				QCoreApplication::processEvents();
			}
	    }else{
	      qDebug() << "error opening log output file\n";
	      return ;
	    }
		fOut.flush();
	    fOut.close();
		///////////////////////////////////////////////////

		SV_database::clearDb();
			 
		emit resultReady();
	}
		
}

