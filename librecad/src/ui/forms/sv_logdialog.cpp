#include "sv_logdialog.h"
#include "ui_sv_logdialog.h"
#include <QDateTime>
#include <QListWidgetItem>
#include <QProgressBar>
#include "sv_database.h"
#include "MW_Window.h"
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QScrollBar>
#include <QTextCodec>

SV_logDialog::SV_logDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SV_logDialog)
{
    ui->setupUi(this);

	loadingItems = false;
		
	ui->lwLog->setStyleSheet("QListWidget::item { border: 1px solid gray;background:rgba(0, 0, 0, 75%) }");
	ui->label->setStyleSheet("QLabel { background-color :  rgba(47, 75, 144, 95%); color : white; border: 1px solid black; }");
    ui->label->setFont(QFont("Verdana", 28, QFont::Bold));
	ui->label->setAlignment(Qt::AlignHCenter);
    ui->leTime->setStyleSheet("QLabel { background-color : transparent; color : darkred; }");
    ui->cbAlert->setChecked(true);
    ui->cbFault->setChecked(true);
    ui->cbSwitch->setChecked(true);
    ui->cbResponse->setChecked(true);
    ui->cbOthers->setChecked(true);
	ui->cbOthers->setVisible(false);

	ui->pbPrint->setIcon(QIcon(":/actions/fileprint.png"));
	ui->pbPrint->setIconSize(QSize(48,48));
	connect(ui->pbPrint, SIGNAL(released()), this, SLOT(slotPrint()));

	ui->pbSave->setIcon(QIcon(":/actions/filesave.png"));
	ui->pbSave->setIconSize(QSize(48,48));
	connect(ui->pbSave, SIGNAL(released()), this, SLOT(slotSave()));

	ui->pbOpen->setIcon(QIcon(":/actions/fileopen.png"));
	ui->pbOpen->setIconSize(QSize(48,48));
	connect(ui->pbOpen, SIGNAL(released()), this, SLOT(slotOpen()));

	ui->pbOpenDb->setIcon(QIcon(":/actions/loaddb.png"));
	ui->pbOpenDb->setIconSize(QSize(48,48));
	connect(ui->pbOpenDb, SIGNAL(released()), this, SLOT(slotLoadDb()));

	
	setCurrentTime();
	
	timeTimer = new QTimer(this);
	connect(timeTimer, SIGNAL(timeout()), this, SLOT(slotTimer()));
	timeTimer->start(60000);

	//connect database

	//eventLogDB = new SV_database();
	//eventLogDB->createConnection("connLogEvent");
	QTimer::singleShot(1000, this, SLOT(slotLoadDb()));

	verticalScrollBar = ui->lwLog->verticalScrollBar();
    connect(verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slotSbValueChanged(int)));
	
	/* test list
	QString text;
	for(int i=0;i<100;i++){
		text = "Item " + QString::number(i);
		addEventLog(Qt::red,text);
		addEventLog(Qt::blue,text);
		addEventLog(Qt::green,text);
        addEventLog(Qt::magenta,text);
		addEventLog(Qt::yellow,text);
	}
	*/

}


SV_logDialog::~SV_logDialog()
{
	
    delete ui;
}


void SV_logDialog::setCurrentTime()
{
	QDateTime time; 
	QString buf;
    time = QDateTime::currentDateTime();  
    buf = time.toString("yyyy-MM-dd hh:mm"); 
	ui->leTime->setText(buf);

}

void SV_logDialog::addEventLog(Qt::GlobalColor color, const QString& txt)
{
	QListWidgetItem* pItem;
	
	pItem = new QListWidgetItem();
    if(color == Qt::blue)
		pItem->setForeground(QColor(0x00,0x80,0xff,0xff));
	else
		pItem->setForeground(color);
	
    pItem->setData(Qt::DisplayRole, txt);
    pItem->setData(Qt::UserRole, QColor(color));
    ui->lwLog->addItem(pItem);
	//ui->lwLog->scrollToBottom();
}


void SV_logDialog::slotTimer()
{
	setCurrentTime();

}

void SV_logDialog::slotPrint()
{
	QPrinter printer;

	on_cbAlert_stateChanged(ui->cbAlert->isChecked());
	
    on_cbFault_stateChanged(ui->cbFault->isChecked());
	
    on_cbSwitch_stateChanged(ui->cbSwitch->isChecked());
	
    on_cbResponse_stateChanged(ui->cbResponse->isChecked());
	
	
	QPrintDialog *dialog = new QPrintDialog(&printer);
	dialog->setWindowTitle(tr("Print Log Data"));

	if (dialog->exec() != QDialog::Accepted)
        return;

	QDateTime time; 
	QString buf;
    time = QDateTime::currentDateTime();  
    buf = time.toString("yyyy-MM-dd hh:mm"); 
	
    int count = 1;
    QTextDocument document;
    document.setDefaultFont(QFont("Verdana", 10));
	QString text;
    QString eventText;
    QTextCodec::codecForLocale();
	Qt::GlobalColor color;
	if(loadType == Type_DB){
		for(int i=0;i<dataList.size();i += 3){
			color = (Qt::GlobalColor)dataList[i].toInt();
            if(!ui->cbAlert->isChecked() && color == Qt::red)
				continue;
			if(!ui->cbAlert->isChecked() && color == Qt::yellow)
				continue;
            if(!ui->cbFault->isChecked() && color == Qt::magenta)
				continue;
            if(!ui->cbSwitch->isChecked() && color == Qt::green)
				continue;
            if(!ui->cbResponse->isChecked() && color == Qt::blue)
				continue;
			
            eventText = dataList[i+2];
            eventText.replace('<','[');
            eventText.replace('>',']');

            text += "<tr>" + QString("<td>%1</td><td>%2</td><td>%3</td>").arg(count).arg(dataList[i+1]).arg(eventText) + "</tr>";
			count++;

			QCoreApplication::processEvents();
		}
	}else{
		
		for(int i=0;i<dataList.size();i++){
				QStringList listLine = dataList[i].split(" ");
				QString strtmp = listLine[1];
                if(!ui->cbAlert->isChecked() && strtmp[0] == QChar('R'))
					continue;
                if(!ui->cbFault->isChecked() && strtmp[0] == QChar('P'))
					continue;
                if(!ui->cbSwitch->isChecked() && strtmp[0] == QChar('G'))
					continue;
                if(!ui->cbResponse->isChecked() && strtmp[0] == QChar('B'))
					continue;

				QString time = listLine[2] + " " + listLine[3];
                eventText = listLine[4];
                eventText.replace('<','[');
                eventText.replace('>',']');
                text += "<tr>" + QString("<td>%1</td><td>%2</td><td>%3</td>").arg(count).arg(time).arg(eventText) + "</tr>";
				count++;

				QCoreApplication::processEvents();
			}
			
	}
	
	QString html =QString("<HTML><HEAD>\
                           <meta http-equiv=\"Content-Type\" content=\"text/html; charset=Big5\"/>\
                           </HEAD><BODY>\
							<H1 align=\"center\">%1</H1><H4 align=\"right\">%2</H4><hr/>\
                               <table border=\"0\">\
						  	<tr>\
						  	<th align=\"left\">%3</th>\
						    <th align=\"left\">%4</th>\
                              <th align=\"left\">%5</th></tr>\
						    %6</table>\
                            </BODY></HTML>").arg("歷史紀錄").arg(buf).arg("編號").arg("時間").arg("事件內容").arg(text);
	
  	
	
	document.setHtml(html);
	document.print( &printer);
	

}

void SV_logDialog::slotOpen()
{

	QString fn = QFileDialog::getOpenFileName(0, tr("Open File..."),
                                              QString("C:/SCADA/"), tr("Log-Files (*.log);;All Files (*)"));
	if (!fn.isEmpty())
        loadLogFile(fn);

	
	
}

void SV_logDialog::slotFileDialogSelected(int mode)
{
	qDebug() << "slotFileDialogSelected:" << mode;
}


void SV_logDialog::slotSbValueChanged(int pos)
{
	if(loadingItems == true)
		return;
	
	loadingItems = true;
	//qDebug() << "+slotSbValueChanged:" << pos <<":" << verticalScrollBar->maximum();

	Qt::GlobalColor color;
	QString time;
	QString content;
	QString text;
	QString strColor;
	int count;
    int lastItemCount;
	QString strtmp;
    //qDebug() << "slotSbValueChanged:itemCount:dataList:" << itemCount << ":" << dataList.size() ;
	if(dataList.size() == itemCount)
		return;
	
	if(pos == verticalScrollBar->maximum()){ //add more listwidget items
		lastItemCount = itemCount;
		if(dataList.size() < (itemCount+48))
			itemCount = dataList.size();
		else
			itemCount += 48;

		if(loadType == Type_DB){
			count = lastItemCount/3 + 1;
			for(int i=lastItemCount; i<itemCount; i += 3){
		        color = (Qt::GlobalColor)dataList[i].toInt();
		        time = dataList[i+1];
		        content = dataList[i+2];
				text = QString("%1 %2 %3").arg(count).arg(time).arg(content);
				//qDebug() << "time:" << time << "content:" << content;
		        addEventLog(color,text);
				count++;
				//ui->progressBar->setValue(i);
				QCoreApplication::processEvents();
			}
		}else{
			count = lastItemCount;
			for(int i=lastItemCount; i<itemCount; i++){
				QStringList listLine = dataList[i].split(" ");
		        color = Qt::green;
				strtmp = listLine[1];
				if(strtmp[0] == QChar('R')){
					color = Qt::red;
					time = listLine[2] + " " + listLine[3];
		        	content = listLine[4];
				}else
				if(strtmp[0] == QChar('G')){
					color = Qt::green;
					time = listLine[2] + " " + listLine[3];
		        	content = listLine[4];
				}else
				if(strtmp[0] == QChar('B')){
					color = Qt::blue;
					time = listLine[2] + " " + listLine[3];
		        	content = listLine[4];
				}else
				if(strtmp[0] == QChar('P')){
					color = Qt::magenta;
					time = listLine[2] + " " + listLine[3];
		        	content = listLine[4];
				}else{  //old version : no color column
					time = listLine[1] + " " + listLine[2];
		        	content = listLine[3];
				}
				
				text = QString("%1 %2 %3").arg(count).arg(time).arg(content);
		        addEventLog(color,text);
				count++;
				QCoreApplication::processEvents();
			}
		}
	}
	//qDebug() << "-slotSbValueChanged:" << pos <<":" << verticalScrollBar->maximum();
	loadingItems = false;

	on_cbAlert_stateChanged(ui->cbAlert->isChecked());
	
    on_cbFault_stateChanged(ui->cbFault->isChecked());
	
    on_cbSwitch_stateChanged(ui->cbSwitch->isChecked());
	
    on_cbResponse_stateChanged(ui->cbResponse->isChecked());
	
    //on_cbOthers_stateChanged(ui->cbOthers->isChecked());
}

void SV_logDialog::slotLoadDb()
{

    Qt::GlobalColor color;
    QString time;
    QString content;
    QString text;
	int count = 1;

	loadType = Type_DB;
	ui->lwLog->clear();	
	dataList.clear();
	itemCount = 0;
	loadingItems = false;
    SV_database::findAll(dataList);
	
	QCoreApplication::processEvents();

	//ui->progressBar->setVisible(true);
	//ui->progressBar->setRange(0,dataList.size());
	if(dataList.size() < 48)
		itemCount = dataList.size();
	else
		itemCount = 48;

	//qDebug() << "SV_logDialog::slotLoadDb:itemCount:" << itemCount;
    for(int i=0; i<itemCount; i += 3){
        color = (Qt::GlobalColor)dataList[i].toInt();
        time = dataList[i+1];
        content = dataList[i+2];
		text = QString("%1 %2 %3").arg(count).arg(time).arg(content);
		//qDebug() << "time:" << time << "content:" << content;
        addEventLog(color,text);
		count++;
		//ui->progressBar->setValue(i);
		QCoreApplication::processEvents();
	}
	//ui->progressBar->setVisible(false);

	
}

void SV_logDialog::loadLogFile(QString& fn)
{
	loadType = Type_File;
	ui->lwLog->clear();
	dataList.clear();
	itemCount = 0;
	loadingItems = false;
	//read log file
    //QString fileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + fn;
	QFile fIn(fn);
	if (fIn.open(QFile::ReadOnly | QFile::Text)) {
	  QTextStream sIn(&fIn);
	  while (!sIn.atEnd())
		dataList += sIn.readLine();
	  	QCoreApplication::processEvents();
	} else {
      QMessageBox::about(NULL,"SCADA" ,"無法打開資料庫備份檔.");
	  return ;
	}
	fIn.close();

	
	//add to list view
	int count = 1;
	Qt::GlobalColor color;
	QString strColor;
	QString time;
	QString content;
    QString text;
	QString strtmp;
	//ui->progressBar->setVisible(true);
	//ui->progressBar->setRange(0,dataList.size());
	if(dataList.size() < 48)
		itemCount = dataList.size();
	else
		itemCount = 48;
	
    for(int i=0;i<itemCount;i++){
		QStringList listLine = dataList[i].split(" ");
        color = Qt::green;
		strtmp = listLine[1];
		if(strtmp[0] == QChar('R')){
			color = Qt::red;
			time = listLine[2] + " " + listLine[3];
        	content = listLine[4];
		}else
		if(strtmp[0] == QChar('Y')){
			color = Qt::yellow;
			time = listLine[2] + " " + listLine[3];
        	content = listLine[4];
		}else
		if(strtmp[0] == QChar('G')){
			color = Qt::green;
			time = listLine[2] + " " + listLine[3];
        	content = listLine[4];
		}else
		if(strtmp[0] == QChar('B')){
			color = Qt::blue;
			time = listLine[2] + " " + listLine[3];
        	content = listLine[4];
		}else
		if(strtmp[0] == QChar('P')){
			color = Qt::magenta;
			time = listLine[2] + " " + listLine[3];
        	content = listLine[4];
		}else{  //old version : no color column
			time = listLine[1] + " " + listLine[2];
        	content = listLine[3];
		}
	    
		text = QString("%1 %2 %3").arg(count).arg(time).arg(content);
        //qDebug() << "text:" << text;
        addEventLog(color,text);
		count++;
		//ui->progressBar->setValue(i);
		QCoreApplication::processEvents();
	}
	//ui->progressBar->setVisible(false);

	
}

void SV_logDialog::slotSave()
{
	QDateTime time; 
	QString buf;

    QMessageBox message(QMessageBox::Warning,"備份","資料備份到檔案後,資料庫將會被清除,要繼續嗎",
			           QMessageBox::Yes|QMessageBox::No,NULL);  
    if (message.exec()==QMessageBox::No){ 
		return;
    }
	
    time = QDateTime::currentDateTime();  
    buf = time.toString("yyyy-MM-dd-hhmm") + ".log"; 
	QString fileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + buf;
                              
	/*
	QString fileName = QFileDialog::getSaveFileName(this,
                                tr("Save File"),
                                MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + buf,
                                tr("Log Files (*.log)"));
                                */
                                
	if(fileName.isEmpty())
		return;

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
			if(color == Qt::yellow)
				colorStr = "Y";
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

	SV_database::clearDb();
	ui->lwLog->clear();
    QMessageBox::about(NULL,"SCADA" ,"資料庫備份完成");
}



void SV_logDialog::on_cbAlert_stateChanged(int arg1)
{
    QListWidgetItem *item;
    int count = ui->lwLog->count();

    for(int i=0;i<count;i++){
        item = ui->lwLog->item(i);
        if(item->data(Qt::UserRole) == (QColor)Qt::red ||
			item->data(Qt::UserRole) == (QColor)Qt::yellow){
            item->setHidden(!arg1);
        }
    }


}

void SV_logDialog::on_cbFault_stateChanged(int arg1)
{
	
	QListWidgetItem *item;
	int count = ui->lwLog->count();
	
	for(int i=0;i<count;i++){
		item = ui->lwLog->item(i);
        if(item->data(Qt::UserRole) == (QColor)Qt::magenta){
            item->setHidden(!arg1);
		}
	}
}

void SV_logDialog::on_cbSwitch_stateChanged(int arg1)
{
	QListWidgetItem *item;
	int count = ui->lwLog->count();
		
	for(int i=0;i<count;i++){
		item = ui->lwLog->item(i);
        if(item->data(Qt::UserRole) == (QColor)Qt::green){
            item->setHidden(!arg1);
		}
	}

}

void SV_logDialog::on_cbResponse_stateChanged(int arg1)
{
	QListWidgetItem *item;
	int count = ui->lwLog->count();
		
	for(int i=0;i<count;i++){
		item = ui->lwLog->item(i);
        if(item->data(Qt::UserRole) ==(QColor) Qt::blue){
            item->setHidden(!arg1);
		}
	}
}

void SV_logDialog::on_cbOthers_stateChanged(int arg1)
{
	QListWidgetItem *item;
	int count = ui->lwLog->count();
		
	for(int i=0;i<count;i++){
		item = ui->lwLog->item(i);
        if(item->data(Qt::UserRole) == (QColor)Qt::green){
            item->setHidden(!arg1);
		}
	}

}
