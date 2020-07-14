#include "sv_window.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QFileDialog>
#include <QtSerialPort/QSerialPort>
#include "settingsdialog.h"
#include <qlocalsocket.h>
#include <QMessageBox>
#include <QThread>
#include "sv_database.h"
#include "MW_Window.h"
#include "sv_floorselection.h"
#include "sv_progressbar.h"
#include <QPainter>
#include <QPaintEvent>
#include "playsoundthread.h"
#include "sv_label.h"
#include <QLibrary>
#include <QPointer>
#include <QSound>
#include <time.h>

SV_Window::SV_Window()
{
	switchLogData = false;
	connectionBreak = true;
	watchDog = 0;
    watchDogLast = 0;
	watchDogErr = 0;
	mapWindow = NULL;
	//mapWindowState = Window_Unknown;
	//settings = NULL;
	//communication settings
    DC1 = 0x11;
	STX = 0x02;
	ETX = 0x03;
    EOT = 0x04;
	ENQ = 0x05;
	ACK = 0x06;
	NAK = 0x15;
	cmdEOT[0] = 0x04;
    cmdEOT[1] = 0x00;
	cmdACK[0] = 0x06;
    cmdACK[1] = 0x00;
	POL = ENQ;
	settings = new Mgwip;
	/*
	serial = new QSerialPort(this);
	connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
	*/
	//load play-voice dll
	/*
    QString libName = "ttsagent.dll";
    QLibrary voiceLib(libName);
    if(voiceLib.load()){
        playSound = (tPlaySound)voiceLib.resolve("playSound");
    }
	*/
    
}
SV_Window::~SV_Window()
{
    delete displayConnectStatusWindow;
	delete displayTimeWindow;
	delete displayEventStatusWindow ;

	

}



void SV_Window::init()  
{  
	QPalette palette;
	QString fontName = "Microsoft YaHei UI";
		
	QBrush red = QBrush(QColor(0xff,0x00,0x00,0xff));
    QBrush green = QBrush(QColor(0x00,0xff,0x00,0xff));
    QBrush blue = QBrush(QColor(0x87,0xce,0xfa,0xff));
    QBrush yellow = QBrush(QColor(0xff,0xff,0x00,0xff));
    QBrush purple = QBrush(QColor(0x80,0x00,0x80,0xff));
	QBrush black = QBrush(QColor(0x00,0x00,0x00,0xff));
	QBrush white = QBrush(QColor(0xff,0xff,0xff,0xff));
	QBrush gray = QBrush(QColor(0x2f,0x4f,0x4f,0xff));
	QBrush trans = QBrush(QColor(0x00,0x00,0x00,0x00));
	
	red.setStyle(Qt::SolidPattern);
	green.setStyle(Qt::SolidPattern);
	blue.setStyle(Qt::SolidPattern);
	yellow.setStyle(Qt::SolidPattern);
	purple.setStyle(Qt::SolidPattern);
	black.setStyle(Qt::SolidPattern);
    //set white text    
	palette.setBrush(QPalette::Active, QPalette::WindowText, yellow);
	palette.setBrush(QPalette::Inactive, QPalette::WindowText, yellow);
	//set black background
	palette.setBrush(QPalette::Active, QPalette::Window, black);
	palette.setBrush(QPalette::Inactive, QPalette::Window, black);
	QFont font;
	font.setPointSize(28);
	font.setBold(true);

	//set sv_window bg color
	//setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1,"\
	//			  "stop: 0 #bdc3c7 , stop: 1 #2c3e50);");
	setStyleSheet("background-color: rgba(67,127,127,0%);");
	
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	//layout:
	//H0
	//H2 - V20(h201),V2
	
	//H0
    QHBoxLayout *h0Layout = new QHBoxLayout(this);
	    
    windowTitle = new QLabel(this);
	windowTitle->setStyleSheet("QLabel {background-image:url(:/actions/sv_header.jpg);\
										background: rgba(0,0,0,0%);\
												color: white;\
												border-radius: 12px;}");
	windowTitle->setText(projectName);
	windowTitle->setFont(QFont(fontName,28,QFont::Bold));
	windowTitle->setAlignment(Qt::AlignCenter);
	h0Layout->addWidget(windowTitle);
	h0Layout->setStretch(0,10);
	
	/*
	windowTitle = new SV_Label(this);
	windowTitle->setStyleSheet("QLabel {background-color : rgba(0,0,0,0%);color : white;}");
	windowTitle->setFont(QFont(fontName,28,QFont::Bold));
	QPixmap qpImage(":/actions/sv_header.jpg");
	windowTitle->setPixmap(qpImage);
	windowTitle->setText(projectName);
	windowTitle->setIndent(160);
	windowTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	h0Layout->addWidget(windowTitle);
	*/

	//H2 
	//V20
	
    QHBoxLayout *h2Layout = new QHBoxLayout(this);
	QVBoxLayout *v20Layout = new QVBoxLayout(this);
    QVBoxLayout *v21Layout = new QVBoxLayout(this);
	//QHBoxLayout *h201Layout = new QHBoxLayout(this);
	QVBoxLayout *v22Layout = new QVBoxLayout(this);

	eventList = new QListWidget(this);
	eventList->setStyleSheet("QListWidget {background:transparent}");
	eventList->setFont(QFont(fontName,16,QFont::Normal));


    /*
    eventList->setStyleSheet("QListWidget {background-image:url(:/actions/svbg.png);\
 	                                       background-origin: content;\
    									   background-clip: padding;\
									   border: 1px solid black;\
									   border-width: 1px;\
									   border-style: solid;\
									   border-radius: 16px;}");
	*/
	connect(eventList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotLwDoubleClicked(QListWidgetItem*)));
    

	//add buttons.alert msg boxes
	eventLogBtn = new QPushButton(this);
    eventLogBtn->setText("歷史資料庫");
	eventLogBtn->setIcon(QIcon(":/actions/sv1.png"));
	eventLogBtn->setIconSize(QSize(64,64));
    eventLogBtn->setStyleSheet("QPushButton{background-color: rgba(1,37,44,80%);"
    	                    "color: white;"
    	                    "text-align : left;}"
							"QPushButton:hover{"
							"background-color: rgb(85, 170, 255);}"
							"QPushButton:pressed{"
							"background-color: rgb(255,64,64);}");
    eventLogBtn->setFont(QFont(fontName,22,QFont::Bold));
   
	
	connect(eventLogBtn, SIGNAL(released()), this, SLOT(slotEventLog()));

	viewMapBtn = new QPushButton(this);
    viewMapBtn->setText("檢視樓層圖");
	viewMapBtn->setIcon(QIcon(":/actions/sv2.png"));
	viewMapBtn->setIconSize(QSize(64,64));
    viewMapBtn->setStyleSheet("QPushButton{background-color: rgba(1,37,44,80%);"
    						"color: white;"
    						"text-align : left;}"
							"QPushButton:hover{"
							"background-color: rgb(85, 170, 255);}"
							"QPushButton:pressed{"
							"background-color: rgb(255,64,64);}");
    viewMapBtn->setFont(QFont(fontName,22,QFont::Bold));

	
	connect(viewMapBtn, SIGNAL(released()), this, SLOT(slotViewMap()));

	checkMapBtn = new QPushButton(this);
    checkMapBtn->setText("檢視感測器位置");
	checkMapBtn->setIcon(QIcon(":/actions/sv3.png"));
	checkMapBtn->setIconSize(QSize(64,64));
    checkMapBtn->setStyleSheet("QPushButton{background-color: rgba(1,37,44,80%);"
							"color: white;"
							"text-align : left;}"
							"QPushButton:hover{"
							"background-color: rgb(85, 170, 255);}"
							"QPushButton:pressed{"
							"background-color: rgb(255,64,64);}");
    checkMapBtn->setFont(QFont(fontName,22,QFont::Bold));
	connect(checkMapBtn, SIGNAL(released()), this, SLOT(slotCheckMap()));
	checkMapBtn->setEnabled(true);
	
    resetBtn = new QPushButton(this);
    resetBtn->setText("復歸");
	resetBtn->setIcon(QIcon(":/actions/sv4.png"));
	resetBtn->setIconSize(QSize(64,64));
    resetBtn->setStyleSheet("QPushButton{background-color: rgba(1,37,44,80%);"
							"color: white;"
							"text-align : left;}"
							"QPushButton:hover{"
							"background-color: rgb(85, 170, 255);}"
							"QPushButton:pressed{"
							"background-color: rgb(255,64,64);}");
    resetBtn->setFont(QFont(fontName,22,QFont::Bold));
	connect(resetBtn, SIGNAL(released()), this, SLOT(slotResetHRN()));
	

	mainAlarmBtn = new QPushButton(this);
    mainAlarmBtn->setText("主音響停止");
	mainAlarmBtn->setIcon(QIcon(":/actions/sv5.png"));
	mainAlarmBtn->setIconSize(QSize(64,64));
    mainAlarmBtn->setStyleSheet("QPushButton{background-color: rgba(1,37,44,80%);"
							"color: white;"
							"text-align : left;}"
							"QPushButton:hover{"
							"background-color: rgb(85, 170, 255);}"
							"QPushButton:pressed{"
							"background-color: rgb(255,64,64);}");
    mainAlarmBtn->setFont(QFont(fontName,22,QFont::Bold));
	connect(mainAlarmBtn, SIGNAL(released()), this, SLOT(slotSwitchMainAlarm()));
	
	
	
	resetBtn->setEnabled(false);
	mainAlarmBtn->setEnabled(false);
	

	//add print button
	QIcon icon = QIcon();
	icon.addPixmap( QPixmap( ":/actions/sv6.png" ), QIcon::Normal, QIcon::On );
	icon.addPixmap( QPixmap( ":/actions/sv61.png" ), QIcon::Normal, QIcon::Off );
	autoPrintBtn = new QPushButton(this);
    autoPrintBtn->setText("警報自動列印");
	autoPrintBtn->setIcon(icon);
	autoPrintBtn->setIconSize(QSize(64,64));
	autoPrintBtn->setCheckable(true);
	autoPrintBtn->setChecked(false);
	autoPrintBtn->setStyleSheet("QPushButton{background-color: rgba(1,37,44,80%);"
							"color: white;"
							"text-align : left;}"
							"QPushButton:hover{"
							"background-color: rgb(85, 170, 255);}"
							"QPushButton:pressed{"
							"background-color: rgb(255,64,64);}");
	autoPrintBtn->setFont(QFont(fontName,22,QFont::Bold));
	connect(autoPrintBtn, SIGNAL(released()), this, SLOT(slotAutoPrint()));
	

	QLabel* eventTitle = new QLabel(this);
	eventTitle->setStyleSheet("QLabel {background: rgba(15,58,67,90%);\
												color: white;\
												border-radius: 12px;}");
	eventTitle->setText("事件列表");
	eventTitle->setFont(QFont(fontName,24,QFont::Normal));
	eventTitle->setAlignment(Qt::AlignCenter);
	v20Layout->setMargin(20);
	v20Layout->addWidget(eventTitle);
	
	//add background effects of eventlist
	sv_eventlist *listwidget = new sv_eventlist(this);
	QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(eventList);
    listwidget->setLayout(layout);
	v20Layout->addWidget(listwidget);
	
	//V21
	toolButton = new QPushButton(this);
	toolButton->setText("連線設定");
	toolButton->setIcon(QIcon(":/actions/menu.png"));
	toolButton->setIconSize(QSize(64,64));
    toolButton->setStyleSheet("QPushButton{background-color: rgba(1,37,44,80%);"
							"color: white;"
							"text-align : left;}"
							"QPushButton:hover{"
							"background-color: rgb(85, 170, 255);}"
							"QPushButton:pressed{"
							"background-color: rgb(255,64,64);}");
	toolButton->setFont(QFont(fontName,22,QFont::Bold));

	
	menu = new QMenu(this);
    menu->setStyleSheet("QMenu {color: white;\
    	                      font-size:28px;\
                              background-color: rgba(1,37,44,50%);\
							  margin: 2px;\
						      border: 1px solid black;}\
					   QMenu::item {background-color: transparent;\
                                    padding: 2px 20px;\
					                border: 1px solid transparent;}\
					   QMenu::item:selected {border-color: darkblue;\
                                             padding: 2px 20px;\
                                             background: rgba(100, 100, 100, 150);}\
    						");
    exitAction = new QAction("退出監控",this);
	exitAction->setIcon(QIcon(":/actions/appclose.png")); 
	connect(exitAction, SIGNAL(triggered()), this, SLOT(slotExit()));

    comConfigtAction = new QAction("消防主機IP設定",this);
	comConfigtAction->setIcon(QIcon(":/actions/comsetting.png")); 
	connect(comConfigtAction, SIGNAL(triggered()), this, SLOT(slotComConfig()));

    connectAction = new QAction("連線",this);
	connectAction->setIcon(QIcon(":/actions/comconnect.png")); 
	connect(connectAction, SIGNAL(triggered()), this, SLOT(slotComConnect()));

    switchLogDataAction = new QAction("分析資料",this);
    switchLogDataAction->setIcon(QIcon(":/actions/editundo.png"));
    connect(switchLogDataAction, SIGNAL(triggered()), this, SLOT(slotSwitchLogData()));

	menu->addAction(comConfigtAction);
	menu->addAction(connectAction);
	menu->addAction(exitAction);
    menu->addAction(switchLogDataAction);
	
	toolButton->setMenu(menu);
    connect(toolButton, SIGNAL(clicked()), toolButton, SLOT(showMenu()));

	v22Layout->setSpacing(0);
	toolButton->setMinimumHeight(122);
	eventLogBtn->setMinimumHeight(122);
	viewMapBtn->setMinimumHeight(122);
	checkMapBtn->setMinimumHeight(122);
	resetBtn->setMinimumHeight(122);
	mainAlarmBtn->setMinimumHeight(122);
	autoPrintBtn->setMinimumHeight(122);
	
    v22Layout->addWidget(toolButton,16);
    v22Layout->addWidget(eventLogBtn,16);
	v22Layout->addWidget(viewMapBtn,16);
	v22Layout->addWidget(checkMapBtn,16);
	v22Layout->addWidget(resetBtn,16);
	v22Layout->addWidget(mainAlarmBtn,16);
	v22Layout->addWidget(autoPrintBtn,16);
	
	//connection status
	QVBoxLayout *h0v0Layout = new QVBoxLayout(this);

	displayConnectStatusWindow = new QLabel(this);
	displayConnectStatusWindow->setAutoFillBackground(true);
	displayConnectStatusWindow->setStyleSheet("QLabel {background-color: rgb(15,58,67);\
												color: white;\
												border-radius: 12px;}");
	displayConnectStatusWindow->setText("未連線");
	displayConnectStatusWindow->setFont(QFont(fontName,18,QFont::Normal));
	displayConnectStatusWindow->setAlignment(Qt::AlignCenter);
	//show time
	displayTimeWindow = new QLabel(this);
	displayTimeWindow->setAutoFillBackground(true);
	displayTimeWindow->setStyleSheet("QLabel {background-color: rgb(15,58,67);\
												color: white;\
												border-radius: 12px;}");
	displayTimeWindow->setFont(QFont(fontName,18,QFont::Normal));
	displayTimeWindow->setAlignment(Qt::AlignCenter);
	h0v0Layout->addWidget(displayConnectStatusWindow);
	h0v0Layout->addWidget(displayTimeWindow);

	SV_Label *logo = new SV_Label(this);
	logo->setStyleSheet("QLabel {background-color : rgba(0,0,0,0%);color : white;}");
	logo->setFixedHeight(60);
	QPixmap qpImage(":/actions/sv_header.jpg");
	logo->setPixmap(qpImage);

	QVBoxLayout *v0v0Layout = new QVBoxLayout(this);
	v0v0Layout->addWidget(logo);

	displayEventStatusWindow = new SV_DisplayEventStatus();
	v21Layout->addStretch(1);
	v21Layout->addLayout(h0v0Layout);
	v21Layout->addStretch(4);
	v21Layout->addWidget(displayEventStatusWindow);
	v21Layout->addStretch(6);
	v21Layout->addLayout(v0v0Layout);
	v21Layout->addStretch(1);
	
	h2Layout->addLayout(v22Layout);
	h2Layout->addLayout(v20Layout);
    h2Layout->addLayout(v21Layout);
    h2Layout->setStretch(0,1);
    h2Layout->setStretch(1,24);
    h2Layout->setStretch(2,2);

	mainLayout->addLayout(h0Layout);
    mainLayout->addLayout(h2Layout);
	mainLayout->setStretch(0,1);
	mainLayout->setStretch(1,12);
	mainLayout->addSpacing(1);
	//mainLayout->setStretch(2,1);
	
	QDateTime time; 
	QString buf;
    time = QDateTime::currentDateTime();  
    buf = time.toString("yyyy-MM-dd hh:mm");  
	setCurrentTime(buf);

	initAlertReport();

    setLayout(mainLayout);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowState((windowState() & ~(Qt::WindowMinimized | Qt::WindowFullScreen))
                   | Qt::WindowMaximized);
	
    
	setActive();

    addDbItem(Qt::green, "", "監控系統開始");
	setConnectStatus(false);
	//connect database
	//eventLogDB = new SV_database();
	//eventLogDB->createConnection();

    //cmd agent server
    cmdAgentd = new QLocalServer(this);
    if (!cmdAgentd->listen("SCADA")) {
        cmdAgentd->close();
        return;
    }
    connect(cmdAgentd, SIGNAL(newConnection()), this, SLOT(recvCmd()));

    //loadAllMaps();
	
	//current time timer
	timeTimer = new QTimer(this);
	connect(timeTimer, SIGNAL(timeout()), this, SLOT(slotTimer()));
	timeTimer->start(60000);
	//cmd queue timer
	cmdQueueTimer = new QTimer(this);
	connect(cmdQueueTimer, SIGNAL(timeout()), this, SLOT(slotCmdDequeue()));
	cmdQueueTimer->start(200);
	
	//auto backup
	slotBackupDb();

	
	
	/////////////////////////////////////////////////////////////
    qRegisterMetaType<TypeMceEventLog>("TypeMceEventLog");
    qRegisterMetaType<TypeMceEventLog>("TypeMceEventLog&");
    qRegisterMetaType<TypeConnectStatus>("TypeConnectStatus");
    qRegisterMetaType<TypeConnectStatus>("TypeConnectStatus&");
    
	connect(&commThread, SIGNAL(request(TypeMceEventLog)), this, SLOT(readData(TypeMceEventLog)));
    connect(&commThread, SIGNAL(connectNotify(TypeConnectStatus)), this, SLOT(connectCheck(TypeConnectStatus)));
	connect(this, SIGNAL(addCmdQueue(QString)),&commThread, SLOT(slotGetCmdQueue(QString)));
	
	//run test
	/*
	QTimer* testTimer = new QTimer(this);
    connect(testTimer, SIGNAL(timeout()), this, SLOT(runTest1()));
	testTimer->setSingleShot(true);
    testTimer->start(2000);
    connect(testTimer, SIGNAL(timeout()), this, SLOT(runTest2()));
	testTimer->setSingleShot(true);
    testTimer->start(3000);
    connect(testTimer, SIGNAL(timeout()), this, SLOT(runTest3()));
	testTimer->setSingleShot(true);
    testTimer->start(4000);
    connect(testTimer, SIGNAL(timeout()), this, SLOT(runTest4()));
	testTimer->setSingleShot(true);
    testTimer->start(5000);
    connect(testTimer, SIGNAL(timeout()), this, SLOT(runTest5()));
	testTimer->setSingleShot(true);
    testTimer->start(6000);
    connect(testTimer, SIGNAL(timeout()), this, SLOT(runTest6()));
	testTimer->setSingleShot(true);
    testTimer->start(7000);
    connect(testTimer, SIGNAL(timeout()), this, SLOT(runTest7()));
	testTimer->setSingleShot(true);
    testTimer->start(8000);
    connect(testTimer, SIGNAL(timeout()), this, SLOT(runTest8()));
	testTimer->setSingleShot(true);
    testTimer->start(9000);
    */
	///////////////////////////

	return;
    
}  

void SV_Window::closeEvent(QCloseEvent *event)
{	
	if (timeTimer){
    	delete timeTimer;
    	timeTimer = NULL;
	}
	if (cmdQueueTimer){
    	delete cmdQueueTimer;
    	cmdQueueTimer = NULL;
	}

	delete cmdAgentd;
	commThread.stopCommWorker();
	
	
	
	backupThread.exit();
	
	addDbItem(Qt::green, "", "監控系統結束");
	if(settings != NULL)
		delete settings;

	
	
    emit svclose();
    event->accept();
}

void SV_Window::paintEvent(QPaintEvent *event)
{

	QPainter painter(this);
	/*
	QPixmap src = QPixmap(":/actions/svbg.jpg");
	
	int numX = width() / src.width();
	int numY = height() / src.height();
	for(int iy=0 ;iy<=numY ;iy++){
		for(int ix=0 ;ix<=numX ;ix++){
			painter.drawPixmap(ix*src.width(), iy*src.height(),src);
		}
	}*/
    
    QWidget::paintEvent(event);
}

	
void SV_Window::setProjectName(QString& name)
{
	projectName = name;
	windowTitle->setText(projectName);
};

void SV_Window::setRtuMap(const QMultiMap<QString, QString>& map)
{
	rtuMap = map;
	//debug
	//QString key;
  	//foreach(key, rtuMap.keys())
    //    qDebug() << key << " = " << rtuMap.value(key);
}

void SV_Window::setMapsFileList(const QStringList& maps)
{
    mapsFileList = maps;
}


void SV_Window::slotTimer()
{
	QDateTime time; 
	QString buf;
    time = QDateTime::currentDateTime();  
    buf = time.toString("yyyy-MM-dd hh:mm"); 
	setCurrentTime(buf);

}

void SV_Window::slotCmdDequeue()
{
	
	QByteArray *cmd = queue.dequeue();
	if(cmd == NULL)
		return;

	
	handleCmd(*cmd);
	delete cmd;
	
}

void SV_Window::connectCheck(const TypeConnectStatus &s)
{
	if(s.status == 0){
		connectionDetatch();
	}else
	if(s.status == 1){
		connectionAttach();
	}
}

void SV_Window::connectionAttach()
{
    qDebug() << "ConnectionAttach";
	connectAction->setEnabled(false);
	displayConnectStatusWindow->setText("已連線");
    addDbItem(Qt::green, "", "已連線");
	resetBtn->setEnabled(true);
	mainAlarmBtn->setEnabled(true);
	checkMapBtn->setEnabled(true);
	setConnectStatus(true);
	
}


void SV_Window::connectionDetatch()
{
    qDebug() << "ConnectionDetatch";
	connectAction->setEnabled(true);
	displayConnectStatusWindow->setText("未連線");
    addDbItem(Qt::green, "", "未連線");
	resetBtn->setEnabled(false);
	mainAlarmBtn->setEnabled(false);
	checkMapBtn->setEnabled(false);
	setConnectStatus(false);
}


void SV_Window::slotWatchDog()
{
	//qDebug() << "watchDogLast:" << watchDogLast << "  watchDog:" << watchDog;
    if(watchDogLast == watchDog){
		watchDogErr++;
		if(watchDogErr > 6){
			if(connectionBreak == false){
                connectionDetatch();
				connectionBreak = true;
			}
		}
	}else{
		watchDogErr = 0;
        watchDogLast = watchDog;
		if(connectionBreak == true){
            connectionAttach();
			connectionBreak = false;
		}
	}
}


void SV_Window::slotComConnect()
{
    openSerialPort();
}

void SV_Window::slotSwitchLogData()
{
	if(switchLogData == false){
		switchLogData = true;
		switchLogDataAction->setIcon(QIcon(":/actions/editundo2.png"));
        QMessageBox::about(NULL,"SCADA" ,"分析資料已開啟");
	}
	else{
		switchLogData = false;
		switchLogDataAction->setIcon(QIcon(":/actions/editundo.png"));
        QMessageBox::about(NULL,"SCADA" ,"分析資料已關閉");
	}
}

void SV_Window::slotComConfig()
{
    configureCom();
}

void SV_Window::slotAutoPrint()
{
	//press test for rtu fault 
	/*
	QList<QByteArray*> list;
    char data[] = {0x02,0x24,0x54,0x30,\
		               0x30,0x30,0x30,0x31,0x30,0x30,0x31,0x31,\
		               0x67,0x68,0x30,0x32,0x31,\
		               0x30,0x30,0x30,0x31,0x30,0x30,0x32,0x30,0x30,0x30,0x30,0x31,0x31,0x30,\
		               0x31,0x30,0x31,0x30};
    QByteArray* cmd;
	QByteArray tmp;
    for(int i=1 ;i<251 ;i++){
		
        tmp = QByteArray::number(i,10);
        if(tmp.size() == 1){
            data[10] = tmp[0];
        }
        else
        if(tmp.size() == 2){
            data[9] = tmp[0];
            data[10] = tmp[1];

        }else
        if(tmp.size() == 3){
            data[8] = tmp[0];
            data[9] = tmp[1];
            data[10] = tmp[2];
        }

	   cmd = new QByteArray(data,sizeof(data));
	   list.append(cmd);
	}
	   
	foreach(QByteArray* item, list){ 
		
		queue.enqueue(item);
	}
	*/
	//if(autoPrintBtn->isChecked()){
    //    mapWindow->slotFilePrintPreview(true);
	//}
}

void SV_Window::slotBackupDb()
{
	//qDebug()<<"From SV_Window::slotBackupDb thread: "<<QThread::currentThreadId();
    QObject::connect(&backupTimer, SIGNAL(timeout()), &backupWorker, SLOT(doWork()));
    backupTimer.start(600000);
    backupTimer.moveToThread(&backupThread);
    backupWorker.moveToThread(&backupThread);
    backupThread.start();
}


void SV_Window::slotExit()
{
	//close socket
	
	if(mapWindow != NULL){
		mapWindow->setEdMode();
		mapWindow->slotFileClose();
		//mapWindow->close();
		delete mapWindow;
	}
	
	close();
	
}

void SV_Window::slotEventLog()
{
	
	logDlg = new SV_logDialog();
	logDlg->setAttribute(Qt::WA_DeleteOnClose, true);
	logDlg->setModal(false);
    logDlg->show();
    
    
}

void SV_Window::slotQC_AppWindowClosed()
{
   qDebug() << "slotQC_AppWindowClosed.";
   closeMapWindow();
}

void SV_Window::getFloorSelectionMapName(const QString &s)
{
    this->floorSelectionMapName = s;
    
}

void SV_Window::slotViewMap()
{
	

    SV_FloorSelection* dlg = new SV_FloorSelection();
	dlg->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dlg, SIGNAL(getMapName(QString)), this, SLOT(getFloorSelectionMapName(QString)));
	dlg->init(redAlertMap,greenAlertMap,blueAlertMap,magentaAlertMap);
	int rc = dlg->exec();
	if(rc == QDialog::Accepted){
        openMap(floorSelectionMapName, Alert_Fire,false);
	}else
		return;

    /*
    QPointer<SV_FloorSelection> dlg = new SV_FloorSelection(this);
	dlg->init(redAlertMap,greenAlertMap,blueAlertMap,magentaAlertMap);
	dlg->exec();
	QString fn = dlg->getMapName();
	if(fn == NULL)
		return;

	delete dlg;
    */
    
	/*
 	mapWindow->slotFileOpen(fn, RS2::FormatDXFRW);
   	mapWindow->show();
	QApplication::setActiveWindow(mapWindow);
    mapWindow->activateWindow();
    double color(5.0); //green
    mapWindow->setSvMode(color,false);
    mapWindow->slotZoomAuto();
	mapWindow->slotViewDraft(false);
	mapWindow->setGridMode(false);
	*/
	
}

void SV_Window::slotResetHRN()
{
    //setHrnReset();
    //enCmdQueue(QString("R0"));
    commThread.setHrnReset();
}

void SV_Window::slotSwitchMainAlarm()
{
    //setMainSpeakerOff();
    //enCmdQueue(QString("S11"));
    commThread.setMainSpeakerOff();
}


void SV_Window::slotCheckMap()
{
	qDebug() << "SV_Window::slotCheckMap ++";

	

	QListWidgetItem* item = eventList->currentItem();
	if(item == NULL)
		return;
	
	QBrush bh = item->foreground();
	QString addr = item->whatsThis();

	if(addr.length() == 0)
		return;
	
	qDebug() << "SV_Window::slotCheckMap 2";

	if(bh.color() == Qt::magenta){
		alertFaultCheck(addr);
	}else
	if(bh.color() == Qt::red){
        alertFireCheck(addr);
	}else
	if(bh.color() == QColor(0x00,0x80,0xff,0xff)){
		alertR4Check(addr);
	}
	
	qDebug() << "SV_Window::slotCheckMap 3";
}

/*
void SV_Window::slotSwitchLocalAlarm()
{
    //setLocalSpeakerOff();
    enCmdQueue(QString("S17"));
}
*/
void SV_Window::setConnectStatus(bool status)
{
	if(status == false){
        connectAction->setEnabled(true);
		displayConnectStatusWindow->setText("未連線");
		addDbItem(Qt::green, "", "未連線");
		
	}
	else{
        connectAction->setEnabled(false);
		displayConnectStatusWindow->setText("已連線");
		addDbItem(Qt::green, "", "已連線");
		
	}
	
}
void SV_Window::setCurrentTime(const QString& txt)    
{
	displayTimeWindow->setText(txt);
}


void SV_Window::initAlertReport()
{
	magentaAlarm = 0;
	redAlarm = 0;
	greenAlarm = 0;
	yellowAlarm = 0;
	blueAlarm = 0;
	updateRedReport(redAlarm);
	updateGreenReport(greenAlarm);
    updateBlueReport(blueAlarm);
    updateYellowReport(yellowAlarm);
    updateMagentaReport(magentaAlarm);
    
}


void SV_Window::svAlertNotification(const QString& fn, Qt::GlobalColor color, int count)
{
	
	emit svFloorAlertNotification(fn, color, count);
	
	if(color == Qt::red){
		redAlertMap.insert(fn,count);
		
	}
	else
	if(color == Qt::green){
		greenAlertMap.insert(fn,count);
	}
	else
	if(color == Qt::blue){
		blueAlertMap.insert(fn,count);
	}
	else
	if(color == Qt::yellow){
		yellowAlertMap.insert(fn,count);
	}
	else
	if(color == Qt::magenta){
		magentaAlertMap.insert(fn,count);
	}
	
}


void SV_Window::updateAlertReport(const QString& address, Qt::GlobalColor color, int count)
{
	QString key = address + "_file";
	QString fn = rtuMap.value(key);
	//update report
	if(color == Qt::red){
		redAlarm += count;
		if(redAlarm < 0)
			redAlarm = 0;
		
		updateRedReport(redAlarm);
		svAlertNotification(fn,color,redAlarm);
		
	}
	else
    if(color == Qt::green){
		greenAlarm += count;
		if(greenAlarm < 0)
			greenAlarm = 0;
		
        updateGreenReport(greenAlarm);
		svAlertNotification(fn,color,greenAlarm);
    }
	else
    if(color == Qt::blue){
		blueAlarm += count;
		if(blueAlarm < 0)
			blueAlarm = 0;
		
		updateBlueReport(blueAlarm);
		svAlertNotification(fn,color,blueAlarm);
    }
	else
    if(color == Qt::yellow){
		yellowAlarm += count;
		if(yellowAlarm < 0)
			yellowAlarm = 0;
		
		updateYellowReport(yellowAlarm);
		svAlertNotification(fn,color,yellowAlarm);
    }
	else
    if(color == Qt::magenta){
		magentaAlarm += count;
		if(magentaAlarm < 0)
			magentaAlarm = 0;
		
		updateMagentaReport(magentaAlarm);
		svAlertNotification(fn,color,magentaAlarm);
    }

	
}

void SV_Window::updateRedReport(int count)
{

    displayEventStatusWindow->setRedMsg(QString::number(count));
}
void SV_Window::updateMagentaReport(int count)
{
	
    displayEventStatusWindow->setMagentaMsg(QString::number(count));
}

void SV_Window::updateGreenReport(int count)
{
	
    displayEventStatusWindow->setGreenMsg(QString::number(count));
}

void SV_Window::updateBlueReport(int count)
{
	
    displayEventStatusWindow->setBlueMsg(QString::number(count));
}

void SV_Window::updateYellowReport(int count)
{
	
    displayEventStatusWindow->setYellowMsg(QString::number(count));
}

void SV_Window::addDbItem(Qt::GlobalColor color, const QString& address, const QString& txt)
{
	QString text;
	QDateTime time; 
	QString timeBuf;
	QString rtuName;
    time = QDateTime::currentDateTime();  
    timeBuf = time.toString("yyyy-MM-dd hh:mm:ss"); 
	rtuName = rtuMap.value(address);

	//get rtu floor name
	QString floorName;
	QString key = address + "_file";
	QString fn = rtuMap.value(key);
	QStringList list1 = fn.split("-");
	if(list1.size() >=2){
		QStringList floorList = list1.at(1).split(".");
		floorName = floorList.at(0);
	}
	if(rtuName.length() != 0)
		text = "<" + txt + ">" + floorName + ":" + rtuName;
	else
		text = "<" + txt + ">";
  
	//add to database
	if(!SV_database::insertRow((int)color,timeBuf,text))
		qDebug("Db:insert failed.");

	
	
	
}
void SV_Window::addEventLogItem(Qt::GlobalColor color, const QString& address, const QString& txt)
{
	QString text;
	QListWidgetItem* pItem;
	QDateTime time; 
	QString timeBuf;
	QString rtuName;

	time = QDateTime::currentDateTime();  
    timeBuf = time.toString("yyyy-MM-dd hh:mm:ss"); 
	rtuName = rtuMap.value(address);
	//get rtu floor name
	QString floorName;
	QString key = address + "_file";
	QString fn = rtuMap.value(key);
	QStringList list1 = fn.split("-");
	if(list1.size() >=2){
		QStringList floorList = list1.at(1).split(".");
		floorName = floorList.at(0);
	}
	
	pItem = new QListWidgetItem();
	pItem->setWhatsThis(address);
	QSize size = pItem->sizeHint();
	size.setHeight(60);
	pItem->setSizeHint(size);
	
	if(rtuName.length() != 0)
		text = "<" + txt + ">" + floorName + ":" + rtuName;
	else
		text = "<" + txt + ">";
	
    pItem->setText(timeBuf+text);
	
	if(color == Qt::blue)
        pItem->setForeground(QColor(0x00,0x80,0xff,0xff));
	else
		pItem->setForeground(color);
	
    
    eventList->addItem(pItem);
	eventList->scrollToBottom();
	//add to database
	//eventLogDB->insertRow((int)color,timeBuf,txt);
	if(!SV_database::insertRow((int)color,timeBuf,text))
		qDebug("Db:insert failed.");

	
}

void SV_Window::eliminateEventLogItem(Qt::GlobalColor color, const QString& address, const QString& txt)
{
	QList<QListWidgetItem *> list;

	list = eventList->findItems(address, Qt::MatchContains);
	if(list.size() == 0)
		return;
	/*
	foreach(QListWidgetItem* item, list){
        if(item->text().contains(txt)){
			delete item;
			//qDebug() << "item delete:" << address << txt;
			break;
		}
	}*/
	QColor colorB;
	
	if(color == Qt::blue)
        colorB = QColor(0x00,0x80,0xff,0xff);
	else
		colorB = color;
	
	foreach(QListWidgetItem* item, list){
		QBrush bh = item->foreground();
		if(bh.color() == colorB){
			//ADD:Harrison 20190428
			alertRTUs.remove(address);
			
			updateAlertReport(address,color,-1);
			delete item;
		}
	}
	eventList->scrollToBottom();
    
	
	
}

void SV_Window::eliminateFireLogItem()
{
	QList<QListWidgetItem *> list;
    list = eventList->findItems("", Qt::MatchContains);
	//qDebug() << "eliminateFireLogItem:" << list.size();
    if(list.size() > 0){
        foreach(QListWidgetItem* item, list){
            QBrush bh = item->foreground();
            if(bh.color() == Qt::red){
                QString address = item->whatsThis();
                alertRTUs.remove(address);
                updateAlertReport(address,Qt::red,-1);
                delete item;
            }
        }
    }
    list = eventList->findItems("監視", Qt::MatchContains);
    //qDebug() << "eliminateFireLogItem2:" << list.size();
    if(list.size() > 0){
        foreach(QListWidgetItem* item, list){
            QBrush bh = item->foreground();
            if(bh.color() == QColor(0x00,0x80,0xff,0xff)){
                QString address = item->whatsThis();
                alertRTUs.remove(address);
                updateAlertReport(address,Qt::blue,-1);
                delete item;
            }
        }
    }
	eventList->scrollToBottom();

	
}

void SV_Window::addEventLog(Qt::GlobalColor color, const QString& address, const QString& txt)
{
	QString text;
	QListWidgetItem* pItem;
	QDateTime time; 
	QString timeBuf;
	QString rtuName;
	QList<QListWidgetItem *> list;

	list = eventList->findItems(address, Qt::MatchContains);
	if(list.size() != 0){ //duplicate
		qDebug() << "Duplicate event:" << address <<":" << txt;
		return;
	}

    time = QDateTime::currentDateTime();  
    timeBuf = time.toString("yyyy-MM-dd hh:mm:ss"); 
	rtuName = rtuMap.value(address);

	//get rtu floor name
	QString floorName;
	QString key = address + "_file";
	QString fn = rtuMap.value(key);
	QStringList list1 = fn.split("-");
	if(list1.size() >=2){
		QStringList floorList = list1.at(1).split(".");
		floorName = floorList.at(0);
	}
	pItem = new QListWidgetItem();
	pItem->setWhatsThis(address);
	
	QSize size = pItem->sizeHint();
	size.setHeight(60);
	pItem->setSizeHint(size);
	
	if(rtuName.length() != 0)
		text = "<" + txt + ">" + floorName + ":" + rtuName;
	else
		text = "<" + txt + ">";

	pItem->setText(timeBuf+text);
	
	if(color == Qt::blue)
        pItem->setForeground(QColor(0x00,0x80,0xff,0xff));
	else
		pItem->setForeground(color);

	pItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	eventList->addItem(pItem);
	eventList->scrollToBottom();
	//add to database
	if(!SV_database::insertRow((int)color,timeBuf,text))
		qDebug("Db:insert failed.");

	//update report
    updateAlertReport(address,color,1);

	
}

void SV_Window::setActive()
{
	
	show();
	QApplication::setActiveWindow(this);
    activateWindow();
}

void SV_Window::notifyMapWindowClosed()
{
	
}
void SV_Window::recvCmd()
{
    QLocalSocket *clientConnection = cmdAgentd->nextPendingConnection();
	while (clientConnection->bytesAvailable() < (int)sizeof(quint32)){
		clientConnection->waitForReadyRead();
	}
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

	QDataStream in(clientConnection);
	if(clientConnection->bytesAvailable() < (int)sizeof(quint16)){
	    return;
	}

	QString cmd;
	in >> cmd;
	if(cmd != NULL){
		enCmdQueue(cmd);
	}
	
	//qDebug() << "SV_Window:localserver recv:" << cmd;
    clientConnection->disconnectFromServer();
}

//communication functions
///////////////////////////////////////////////////////////////////
void SV_Window::configureCom()
{
	settings->show();
}


void SV_Window::openSerialPort()
{
    //SettingsDialog::Settings p = settings->settings();
    
    if (commThread.startCommWorker()){
    	/*
		setConnectStatus(true);
		resetBtn->setEnabled(true);
		mainAlarmBtn->setEnabled(true);
		checkMapBtn->setEnabled(true);
		initToHRN();
		*/
    } else {
    	/*
	    setConnectStatus(false);
        resetBtn->setEnabled(false);
		mainAlarmBtn->setEnabled(false);
		checkMapBtn->setEnabled(false);
        */
    }

}

void SV_Window::closeSerialPort()
{
	commThread.stopCommWorker();
    setConnectStatus(false);
    
}

void SV_Window::runTest1()
{
	QString address="010001010010";
	QString text = "廚房";

	setConnectStatus(true);
	//closeMapWindow();
    text += " 火警";
    addEventLog(Qt::red, address, text);
    addRtu(address,4.0);
    alertFireCheck(address);
}
void SV_Window::runTest2()
{
	QString address="010001010020";
	QString text = "車庫";

	setConnectStatus(true);
	//closeMapWindow();
    text += " 火警";
    addEventLog(Qt::red, address, text);
    addRtu(address,4.0);
    alertFireCheck(address);
}
void SV_Window::runTest3()
{
	QString address="010001010030";
	QString text = "走道";

	setConnectStatus(true);
	//closeMapWindow();
    text += " 火警";
    addEventLog(Qt::red, address, text);
    addRtu(address,4.0);
    alertFireCheck(address);
}
void SV_Window::runTest4()
{
	QString address="010001010040";
	QString text = "客廳";

	setConnectStatus(true);
	//closeMapWindow();
    text += " 火警";
    addEventLog(Qt::red, address, text);
    addRtu(address,4.0);
    alertFireCheck(address);
}
void SV_Window::runTest5()
{
	QString address="010001010050";
	QString text = "機房";

	setConnectStatus(true);
	//closeMapWindow();
    text += " 火警";
    addEventLog(Qt::red, address, text);
    addRtu(address,4.0);
    alertFireCheck(address);
}
void SV_Window::runTest6()
{
	QString address="010001010060";
	QString text = "辦公室";

	setConnectStatus(true);
	//closeMapWindow();
    text += " 火警";
    addEventLog(Qt::red, address, text);
    addRtu(address,4.0);
    alertFireCheck(address);
}
void SV_Window::runTest7()
{
	QString address="010001010100";
	QString text = "辦公室東";

	setConnectStatus(true);
	//closeMapWindow();
    text += " 火警";
    addEventLog(Qt::red, address, text);
    addRtu(address,4.0);
    alertFireCheck(address);
}
void SV_Window::runTest8()
{
	QString address="010001010120";
	QString text = "辦公室西";

	setConnectStatus(true);
	//closeMapWindow();
    text += " 火警";
    addEventLog(Qt::red, address, text);
    addRtu(address,4.0);
    alertFireCheck(address);
}


void SV_Window::readData(const TypeMceEventLog &data)
{
	//qDebug() << "SV_Window::readData:" << QTime::currentTime().toString("hh:mm:ss:zzz") << ":" << data.toHex();
	QString address;
	QString text;

	/*
	qDebug() << "---------------------------------\n";
    qDebug() << "messageType=" << data.messageType << "\n";
    qDebug() << "serialNumber=" << data.serialNumber << "\n";
    qDebug() << "eventGroup=" << eventGroup[data.eventGroup] << "\n";
    qDebug() << "eventType=" << eventType[data.eventType] << "\n";
    qDebug() << "eventCode=" << eventCode[data.eventCode] << "\n";
    qDebug() << "node=" << data.node << "\n";
    qDebug() << "addressType=" << AddrType[data.addressType] << "\n";
    qDebug() << "address=" << data.address << "\n";
    qDebug() << "subAddress=" << data.subAddress << "\n";
    qDebug() << "loop=" << data.loop << "\n";
    qDebug() << "zone=" << data.zone << "\n";
    qDebug() << "length=" << data.length << "\n";
    qDebug() << "InputAction=" << InputAction[data.InputAction] << "\n";
    qDebug() << "timeStamp=" << data.timeStamp << "\n";
    qDebug() << "locationText=" << data.locationText << "\n";
    qDebug() << "panelText=" << data.panelText << "\n";
    qDebug() << "zoneText=" << data.zoneText << "\n";
    qDebug() << "---------------------------------\n";
    */

	/*
	//check the log's timestamp
	time_t timer;
	double seconds;
	QString str;

	time(&timer);
	qDebug() << "timestamp:" << str.number(data.timeStamp);
    seconds = difftime(timer, data.timeStamp);
	qDebug() << "time diff:" << str.number(seconds);
	*/

	//RTU address format
	//node:zone:loop:addr:subaddr
	// 2  :  4 :  2 :  3 :   1
	//check R2ML or DIMM
    if(data.locationText.contains("DIMM") || data.locationText.contains("R2ML")){
    	qDebug() << "DIMM or R2ML detected:";
    	int subaddr = data.subAddress;
    	if(subaddr == 0){
    		qDebug() << "DIMM or R2ML detected:" << "subaddr=0";
    		subaddr = 1;
    	}
    	address.sprintf("%02d%04d%02d%03d%d", data.node, data.zone, data.loop, data.address, subaddr);
    }else{
    	address.sprintf("%02d%04d%02d%03d%d", data.node, data.zone, data.loop, data.address, data.subAddress);
	
    }

	// test only
    QString testLog;
    QString msg;
	msg.sprintf("%d, %d  ", data.messageType, data.serialNumber);
	testLog = msg;
	testLog += eventType.at(data.eventType);
    testLog += ":";
    testLog += eventCode.at(data.eventCode);
    testLog += ":";
    testLog += InputAction.at(data.InputAction);
    testLog += "  ";
    testLog += address;
    testLog += "  ";
    testLog += data.locationText;
    qDebug() << testLog;
    
    //add to log file
    if(switchLogData == true){
	    errorLog(testLog);
    }
    ///////////////////////////////////////////////////
    text = data.locationText;
    text += "  ";
    text += address;
    
    if(data.messageType == 0x85){

    	switch(data.eventType){
    		case 0: //event_type_fire; sensor 1 , 5,8
    		    //closeMapWindow();
    		    text += " 火警";
    			addEventLog(Qt::red, address, text);
    			addRtu(address,4.0);
    			//ADD:20190825
    			alertFireCheck(address);
    		break;

    		case 1: //event_type_evacuate
    		if(data.eventCode == 48 &&                      //event_input_activated
    		    data.InputAction == 5){                     //action_evacuate 
    			addEventLog(Qt::red, address, text);
    			addRtu(address,4.0);
    		}
    		break;

    		case 5: //event_type_fault; sensor 2,3
	    		if(data.eventCode == 16 ||    //event_disconnected_fault
	    		   data.eventCode == 1){      //event_ps_fault
	    			text += " 故障";
	    			addEventLog(Qt::magenta, address, text);
                    addRtu(address,7.0);
	    		}else
	    		if(data.eventCode == 22){ //event_initialising_device
	    			eliminateEventLogItem(Qt::magenta, address, text);
	    		}else
	    		if(data.eventCode == 156){ //event_class_wiring_fault 
	    			text += " 故障";
	    			addEventLog(Qt::magenta, address, text);
                    addRtu(address,7.0);
	    		}else
	    		if(data.eventCode == 32){ //event_battery_disconnected_fault (32),
	    			QString tmp;
	    			tmp.sprintf("%s:%d","電池未連接",data.node);
	    			addEventLog(Qt::magenta, tmp, tmp);
	    		}else
	    		if(data.eventCode == 26){ //event_earth_fault (26)
	    			addEventLog(Qt::magenta, "接地錯誤", "接地錯誤");
	    		}else
	    		if(data.eventCode == 17){ //event_double_address_fault (17)
	    			addEventLog(Qt::magenta, address, "地址重複: " + address);
	    		}else
	    		if(data.eventCode == 20){ //event_unexpected_device_fault (20)
	    			addEventLog(Qt::magenta, address, "非預期設備: "+address);
	    		}else
	    		if(data.eventCode == 21){ //event_wrong_device_fault (21)
	    			addEventLog(Qt::magenta, address, "裝置錯誤,設備種別變更: "+address);
	    		}else
	    		if(data.eventCode == 30){ //event_mains_failed_fault (30)
	    			QString tmp;
	    			tmp.sprintf("%s:%d","AC主電源故障",data.node);
                    addEventLog(Qt::magenta, tmp, tmp);
	    		}else   
                if(data.eventCode == 89){ //event_general_disablement (89)
	    			QString tmp;
	    			tmp.sprintf("%s:%d","暫停Zone",data.zone+1);
	    			addEventLog(Qt::magenta, tmp, tmp);
	    		}else
	    		if(data.eventCode == 151){ //event_psu_earth_fault (151)
	    			QString tmp;
	    			tmp.sprintf("%s:%d","電源供給器接地故障",data.node);
                    addEventLog(Qt::magenta, tmp, tmp);
	    		}
	    	break;

    		case 7: //event_type_tech_alarm; sensor 4-1,4-2
    			if(data.InputAction == 4){  //action_tech_alarm (4),
    				addEventLog(Qt::blue, address, text);
    				addRtu(address, 6.0); //blue
    				break;
    			}

	    		if(data.eventCode == 48){ //event_input_activated
	    			addEventLog(Qt::green, address, text);
	    		}
    		break;

    		case 9: //event_type_status
	    		if(data.InputAction == 9){  //action_reset
					addEventLog(Qt::green, "消防主機系統重置", "消防主機系統重置");
					break;
				}
				if(data.eventCode == 22){  //event_initialising_device
					text += " 啟動";
					addDbItem(Qt::blue, address,text);
					
				} 
				
    		break;

    		case 10: //event_type_ceaction
	    		if(data.InputAction == 53){  //event_ce_active
					addEventLog(Qt::blue, address, text);
    				addRtu(address, 6.0); //blue
    				break;
				}
    		break;

    		default:
    			//addEventLog(Qt::green, address, text);
    		break;
    	}
    	
    }else
    if(data.messageType == 0x87){
    	
    	switch(data.eventType){
    		case 0: //event_type_fire; sensor 1 , 5,8
    		    text += " 火警";
    		case 1: //event_type_evacuate
    			eliminateEventLogItem(Qt::red, address, text);
    		break;

    		case 5: //event_type_fault; sensor 2,3
	    		if(data.eventCode == 16 ||      //event_disconnected_fault
	    		   data.eventCode == 1 ){        //event_ps_fault
	    			eliminateEventLogItem(Qt::magenta, address, text);
	    			
	    		}else
	    		if(data.eventCode == 22){ //event_initialising_device
	    			eliminateEventLogItem(Qt::magenta, address, text);
	    		}else
	    		if(data.eventCode == 156){ //event_class_wiring_fault 
	    			eliminateEventLogItem(Qt::magenta, address, text);
            	}else
	    		if(data.eventCode == 32){ //event_battery_disconnected_fault (32),
	    			QString tmp;
	    			tmp.sprintf("%s:%d","電池未連接",data.node);
	    			eliminateEventLogItem(Qt::magenta, tmp, tmp);
	    		}else
	    		if(data.eventCode == 26){ //event_earth_fault (26)
	    			eliminateEventLogItem(Qt::magenta, "接地錯誤", "接地錯誤");
	    		}else
	    		if(data.eventCode == 17){ //event_double_address_fault (17)
	    			eliminateEventLogItem(Qt::magenta, address, "地址重複: " + address);
	    		}else
	    		if(data.eventCode == 20){ //event_unexpected_device_fault (20)
	    			eliminateEventLogItem(Qt::magenta, address, "非預期設備: " + address);
                }else
	    		if(data.eventCode == 21){ //event_wrong_device_fault (21)
	    			eliminateEventLogItem(Qt::magenta, address, "裝置錯誤,設備種別變更: "+address);
	    		}else
	    		if(data.eventCode == 30){ //event_mains_failed_fault (30)
	    			QString tmp;
	    			tmp.sprintf("%s:%d","AC主電源故障",data.node);
	    			eliminateEventLogItem(Qt::magenta, tmp, tmp);
	    		}else   
                if(data.eventCode == 89){ //event_general_disablement (89)
	    			QString tmp;
	    			tmp.sprintf("%s:%d","暫停Zone",data.zone+1);
	    			eliminateEventLogItem(Qt::magenta, tmp, tmp);
	    		}else
	    		if(data.eventCode == 151){ //event_psu_earth_fault (151)
	    			QString tmp;
	    			tmp.sprintf("%s:%d","電源供給器接地故障",data.node);
                    eliminateEventLogItem(Qt::magenta, tmp, tmp);
	    		}
	    	break;

    		case 7: //event_type_tech_alarm; sensor 4-1,4-2
    			if(data.InputAction == 4){  //action_tech_alarm (4),
    				eliminateEventLogItem(Qt::blue, address, text);
    				break;
    			}
	    		if(data.eventCode == 48){ //event_input_activated
	    			eliminateEventLogItem(Qt::green, address, text);
	    			
	    		}
    		break;

    		case 9: //event_type_status
	    		if(data.InputAction == 9){  //action_reset
					eliminateEventLogItem(Qt::green, "消防主機系統重置", "消防主機系統重置清除");
					break;
				}
				if(data.eventCode == 22){  //event_initialising_device
					//eliminateEventLogItem(Qt::blue, address, text);
				}  
    		break;

    		case 10: //event_type_ceaction
	    		if(data.InputAction == 53){  //event_ce_active
					eliminateEventLogItem(Qt::blue, address, text);
    				break;
				}
    		break;

    		default:
    			
    		break;
    	}
    }
	
   
}

void SV_Window::initToHRN()
{
    //QByteArray cmd = QByteArray(1,DC1);
    //commThread.writeData(cmd);
		
    queryFaultTimer = new QTimer(this);
    connect(queryFaultTimer, SIGNAL(timeout()), this, SLOT(queryFaultReportHRN()));
	queryFaultTimer->setSingleShot(true);
    queryFaultTimer->start(60000);
	//qDebug() << "cmd:" + cmd.toHex();
}

void SV_Window::queryFaultReportHRN()
{
	//queryFaultTimer->stop();
	enCmdQueue("Q1");
    //qDebug() << "queryFaultReportHRN:send Q1:stop queryFaultTimer.";

}

bool SV_Window::checkBCC(QByteArray& cmdTxt)
{
	char bccToChecked;
	int len = cmdTxt.length();
	char bcc = cmdTxt.indexOf(len-1);

	bccToChecked = cmdTxt.indexOf(1);
	for(int i=2;i<len-2;i++){
		bccToChecked ^= cmdTxt.indexOf(i);
	}
	if(bccToChecked == bcc)
		return true;
	else
		return false;
}


void SV_Window::enCmdQueue(QString cmd)
{
	emit this->addCmdQueue(cmd);
/*
	muxCmdToSend.lock();
	qCmdToSend.append(var);
	muxCmdToSend.unlock();
*/
}
/*
void SV_Window::deCmdQueue()
{
	QString cmd;
	muxCmdToSend.lock();
    cmd = qCmdToSend.dequeue();
	if(cmd.contains("C1_ON")){
            QStringList data = cmd.split(":");
			QByteArray address = data[1].toLatin1();
			sendCmdC1(address,1);
			
	}else
	if(cmd.contains("C1_OFF")){
            QStringList data = cmd.split(":");
			QByteArray address = data[1].toLatin1();
			sendCmdC1(address,0);
	}else
	if(cmd.contains("R0")){ //reset
		setHrnReset();
	}else
	if(cmd.contains("S11")){ 
		setMainSpeakerOff();
	}else
	if(cmd.contains("S17")){ 
		setLocalSpeakerOff();
	}else
	if(cmd.contains("Q1")){ 
		sendCmdQ1();
	}else
	if(cmd.contains("Q2")){ 
		sendCmdQ2();
	}else
	if(cmd.contains("Q4")){ 
		sendCmdQ6();
	}
	
	muxCmdToSend.unlock();
	
}
bool SV_Window::checkCmdQueueEmpty()
{	
	bool ret;
	muxCmdToSend.lock();
	ret = qCmdToSend.isEmpty();
	muxCmdToSend.unlock();
	return ret;
}
*/

void SV_Window::handleCmd(QByteArray& cmdTxt)
{
	//qDebug() << "handleCmd:" + cmdTxt;
    
	char txtType;

	//if(!checkBCC(cmdTxt))
	//	return;
	txtType = cmdTxt.at(2);
	switch(txtType){
		case 0x42:
			//HRN faiulre message
			handleCmdB(cmdTxt);
			break;
			
		case 0x43:
			//control status
			handleCmdC(cmdTxt);
			break;
			
		case 0x46:
			//the status of loop(fire,HRN,gas,monitor,confirm,etc.) message
			handleCmdF(cmdTxt);
			break;

		case 0x4a:
			//the status of loop  message
			handleCmdJ(cmdTxt);
			break;

		case 0x4b:
			handleCmdB(cmdTxt);
			break;
			
		case 0x4e:
			//the status of loop(fire,HRN,gas,monitor,confirm,etc.) message
			handleCmdN(cmdTxt);
			break;

		case 0x50:
			handleCmdP(cmdTxt);
			break;
			
		case 0x51:
			//Fault status report
			handleCmdQ(cmdTxt);
			break;
			
		case 0x52:
			//reset
			handleCmdR(cmdTxt);
			break;
			
		case 0x53:
			//SW status
			handleCmdS(cmdTxt);
			break;

		case 0x54:
			handleCmdT(cmdTxt);
			break;

		case 0x56:
			handleCmdV(cmdTxt);
			break;
			
		case 0x5a:
			//buffer full
			handleCmdZ(cmdTxt);
			break;
		default:
			break;
	}
}

void SV_Window::handleCmdB(QByteArray& cmdTxt)
{
	QString code;
	QString sw;
	QString statusContext = "";
	int type;
	
	QStringList B1_CODE = {
		tr("B1_CODE_100"),
		tr("B1_CODE_101"),
		tr("B1_CODE_110"),
		tr("B1_CODE_111"),
		tr("B1_CODE_001"),
		tr("B1_CODE_002"),
		tr("B1_CODE_003"),
		tr("B1_CODE_004"),
			
	};
	
	QStringList B1_INDEX = {
		"100",
		"101",
		"110",
		"111",
		"001",
		"002",
		"003",
		"004",
	};

	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;
		

	
	type = QString(cmdTxt.mid(3,1)).toInt();
	code = QString(cmdTxt.mid(8,3));
	
	if(type == 0){
		statusContext = B0_CODE.at(code.toInt()-1);
	}else
	if(type == 1){
        for(int i=0; i<B1_INDEX.size(); i++){
			if(code == B1_INDEX[i]){
				statusContext = B1_CODE[i];
			}
		}
	}
	
    sw = QString(cmdTxt.mid(11,1));
	if(sw.toInt() == 0){
		eliminateEventLogItem(Qt::magenta, statusContext, "");
		statusContext += ":" + tr("B0_SW_0");
        addDbItem(Qt::magenta, "", statusContext);

		//addEventLogItem(Qt::green, "", statusContext);
		
		
	}else
	if(sw.toInt() == 1){
		statusContext += ":" + tr("B0_SW_1");
		if(code.toInt() == 48){
			addDbItem(Qt::magenta, "", statusContext);
		}else
		addEventLog(Qt::magenta, "", statusContext);
	}

	//qDebug() << "CMD:B:";
	//qDebug() << cmdTxt.toHex();
	//qDebug() << statusContext;
}


void SV_Window::handleCmdC(QByteArray& cmdTxt)
{
	
	int type;
	QString address;
	int TR,SW;
	QString statusContext = "";
	QString rtuType;
	
	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;

	
	address = QString(cmdTxt.mid(4,8));
	
		
	type = QString(cmdTxt.mid(3,1)).toInt();
	TR = QString(cmdTxt.mid(15,1)).toInt();
	SW = QString(cmdTxt.mid(16,1)).toInt();
	rtuType = QString(cmdTxt.mid(12,1));
	statusContext += address + ":";
	int rtuIndex = rtuType[0].toLatin1() - 0x61;
	statusContext += C0_RTU_TYPE.at(rtuIndex) + ":";
	if(type == 2){ //RTU alert test 
		statusContext = tr("C2_TXT") ;
	}else
	if(type == 0){// || type == 1){ //RTU control status report 
		if(TR == 0){
			if(SW == 0){
				//qDebug() << "C:TR=SW=0 +" ;
				statusContext += tr("C0_TR0_SW0");
				addDbItem(Qt::green, address,statusContext);
				eliminateEventLogItem(Qt::green, address,tr("C0_TR0_SW1"));
				alertRTUs.remove(address);
				eliminateAlert(address,Qt::red);
				//qDebug() << "C:TR=SW=0 -" ;
			}
			else
			if(SW == 1){
				statusContext += tr("C0_TR0_SW1");
                if(rtuType[0].toLatin1() == 0x067 || rtuType[0].toLatin1() == 0x06c){ //RTU:l,g , ignore
					if(rtuMap.contains(address))
						addDbItem(Qt::green, address,statusContext);
					
				}else{
					if(rtuMap.contains(address))
					 	addEventLog(Qt::green, address,statusContext);
				}
				
			}
		}else
		if(TR == 1){
			if(SW == 1){
				statusContext += tr("C0_TR1_SW1");
				addEventLog(Qt::green, address,statusContext);
			}
		}else
		if(TR == 2){
			if(SW == 1){
				//statusContext += tr("C0_TR2_SW1");
				//addEventLog(Qt::green, address,statusContext);
			}
		}else
		if(TR == 3){
			if(SW == 0){
				statusContext += tr("C0_TR3_SW0");
                addEventLog(Qt::green, address,statusContext);
			}
			else
			if(SW == 1){
				statusContext += tr("C0_TR3_SW1");
				addEventLog(Qt::green, address,statusContext);
			}
		}
		
	}
	
	//startAlert();
	//qDebug() << "CMD:C:";
	//qDebug() << cmdTxt.toHex();
	//qDebug() << statusContext;
	
	
}

void SV_Window::handleCmdF(QByteArray& cmdTxt)
{

	int type;
	QString address;
	int sw;
	QString rtuType;
	QString statusContext;
	QString RTU;

	
	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;

	
	
	type = QString(cmdTxt.mid(3,1)).toInt();
	sw = QString(cmdTxt.mid(16,1)).toInt();
	
	if(type == 0){ 
		address = QString(cmdTxt.mid(4,8));
		rtuType = QString(cmdTxt.mid(12,2));
		RTU = C0_RTU_TYPE.at(rtuType[0].toLatin1() - 0x61);
		
		if(sw == 0){
			
			statusContext = address + ":" + RTU + ":" + F0_CODE.at(0);
			//addEventLog(Qt::blue, address, statusContext);
			addDbItem(Qt::blue, address, statusContext);
			if(rtuType[0].toLatin1() == 0x66 ||
			   rtuType[0].toLatin1() == 0x67 ||
			   rtuType[0].toLatin1() == 0x6b
			 ){
			 	//supervising
			 	addDbItem(Qt::blue, address, F0_CODE.at(1));
			 	eliminateEventLogItem(Qt::blue, address, F0_CODE.at(1));
                alertRTUs.remove(address);
                //eliminateAlert(address,Qt::blue);
			 	
			}else
			if(rtuType[0].toLatin1() == 0x65){
				//gas
				eliminateEventLogItem(Qt::yellow, address, F0_CODE.at(3));
				alertRTUs.remove(address);
				//eliminateAlert(address,Qt::yellow);
				
				
			}else{
				//fire
				eliminateEventLogItem(Qt::red, address, F0_CODE.at(2));
				alertRTUs.remove(address);
				//eliminateAlert(address,Qt::red);
			}
			
			
            
		}
		else
		if(sw == 1){
			
			if(rtuType[0].toLatin1() == 0x6b ||
			   rtuType[0].toLatin1() == 0x6c ||
               rtuType[0].toLatin1() == 0x6d ){
			   //do nothing
			}else
			if(rtuType[0].toLatin1() == 0x66 ||
			   rtuType[0].toLatin1() == 0x67 
			 ){
			 	if(rtuMap.contains(address)){
				 	//supervising
				 	
				 	statusContext = address + ":" + RTU + ":" + F0_CODE.at(1);
					addEventLog(Qt::blue, address,statusContext);
					addRtu(address,6.0);
					
					//alertR4(address);
			 	}
				
			}else
			if(rtuType[0].toLatin1() == 0x65){
				//gas
				
				statusContext = address + ":" + RTU + ":" + F0_CODE.at(3);
				addEventLog(Qt::yellow, address,statusContext);
				addRtu(address,4.0);
 				alertFire(address);
				
				
			}else{
				//fire
				
				statusContext = address + ":" + RTU + ":" + F0_CODE.at(2);
				addEventLog(Qt::red, address,statusContext);
				addRtu(address,4.0);
				alertFire(address);
				
			}
			
		}
	}else
	if(type == 3){ 
		//statusContext = F3_CODE.at(sw);
		//addEventLog(Qt::green, address,statusContext);
	}
	//alertRTUs.append(address);
	
	
}


void SV_Window::handleCmdJ(QByteArray& cmdTxt)
{

    int type;
	QString address;
	int sw;
	QString rtuType;
	int state;
	QString statusContext;
	QString RTU = "";

	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;
	
	type = QString(cmdTxt.mid(3,1)).toInt();
	address = QString(cmdTxt.mid(4,8));
	rtuType = QString(cmdTxt.mid(12,2));
	int index;
	index = 	rtuType[0].toLatin1() - 0x61;
	if(index < C0_RTU_TYPE.size())
		RTU = C0_RTU_TYPE.at(index);
	
	state = QString(cmdTxt.mid(14,1)).toInt();
	sw = QString(cmdTxt.mid(16,1)).toInt();
	statusContext += address + ":" +  RTU +":";
    index = state-1;
	if(index < J_CODE.size()){
		statusContext += J_CODE.at(index);
	}
		
	if(state == 1){
		if(sw == 0){
			statusContext += ":" + tr("J_SW_0");
			addDbItem(Qt::yellow, address,statusContext);
			eliminateEventLogItem(Qt::yellow, address,statusContext);
		}else
		if(sw == 1){
			statusContext += ":" + tr("J_SW_1");
			addEventLog(Qt::yellow, address,statusContext);
		}
		
		/*
		if(rtuType[0].toLatin1() == 0x6e ||
		   rtuType[0].toLatin1() == 0x6f ){
			//statusContext +=":" + F0_CODE.at(2);
			//addEventLog(Qt::red, address,statusContext);
			//alertFire(address);
			addDbItem(Qt::red, address,statusContext);
		}else{
			addDbItem(Qt::green, address,statusContext);
		}*/
	}else
	if(state == 2){
		if(sw == 0){
			//qDebug() << "J:SW=0 +" ;
			eliminateEventLogItem(Qt::red, address,statusContext);
			statusContext += ":" + tr("J_SW_0");
			addDbItem(Qt::red, address,statusContext);
			alertRTUs.remove(address);
			eliminateAlert(address,Qt::red);
			//qDebug() << "J:SW=0 -" ;
			
			
		}else
		if(sw == 1){
			eliminateEventLogItem(Qt::yellow, address,statusContext);
			statusContext += ":" + tr("J_SW_1");
			addEventLog(Qt::red, address,statusContext);
			addRtu(address,4.0);
			alertFire(address);
			
		}
		
	}else
	if(state == 3){
		//addEventLog(Qt::blue, address,statusContext);
	}
	
	
	
	//qDebug() << "CMD:J:";
	//qDebug() << cmdTxt.toHex();
	//qDebug() << statusContext;
}

void SV_Window::handleCmdK(QByteArray& cmdTxt)
{
	
	int type;
	int TR,SW;
	QString statusContext = "";
	QStringList K0_CODE = {
		tr("K0_SW_0"),
		tr("K0_SW_1"),
		tr("K0_SW_2"),
		tr("K0_TR_1"),
		tr("K0_TR_2"),
		tr("K0_TR_3"),
	};

	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;

	
	type = QString(cmdTxt.mid(3,1)).toInt();
	TR = QString(cmdTxt.mid(4,1)).toInt();
	SW = QString(cmdTxt.mid(5,1)).toInt();

	if(type == 0){
		statusContext = K0_CODE[TR+2] + ":" + K0_CODE[SW];
	}

	//qDebug() << "CMD:K:";
	//qDebug() << cmdTxt.toHex();
	//qDebug() << statusContext;
	
	
}

void SV_Window::handleCmdN(QByteArray& cmdTxt)
{

	int type;
    int TR;
    QString statusContext = "";

    QStringList	N0_CODE = {
        tr("N0_0"),
        tr("N0_1"),
        tr("N0_2")
    };

	QStringList	N1_CODE = {
		tr("N1_0"),
		tr("N1_1"),
		tr("N1_2"),
	};
	QStringList	N5_CODE = {
		tr("N5_0"),
		tr("N5_1"),
        tr("N5_2")
	};
	QStringList	N6_CODE = {
		tr("N6_0"),
		tr("N6_1"),
        tr("N6_2")
    };

	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;

	
	type = QString(cmdTxt.mid(3,1)).toInt();
    TR = QString(cmdTxt.mid(4,1)).toInt();
	
	if(type == 0){ 
        statusContext = N0_CODE.at(TR);
		addDbItem(Qt::magenta, "", statusContext);
	}else
	if(type == 1){ 
        statusContext = N1_CODE.at(TR);
		addDbItem(Qt::magenta, "",statusContext);
	
	}else
	if(type == 5){ 
        statusContext = N5_CODE.at(TR);
		addDbItem(Qt::magenta, "",statusContext);
	}else
	if(type == 6){ 
        statusContext = N6_CODE.at(TR);
		addDbItem(Qt::magenta, "",statusContext);
	}
	
	
	//alertRTUs.append(address);
	
	//qDebug() << "CMD:N:";
	//qDebug() << cmdTxt.toHex();
	//qDebug() << statusContext;
}

void SV_Window::handleCmdP(QByteArray& cmdTxt)
{

	int type;
    int pan;
	int addr;
	int swno;
	int sw;
    QString statusContext;

    QStringList	P0_CODE = {
        tr("P0_0"),
        tr("P0_1"),
        
    };

	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;

	
	type = QString(cmdTxt.mid(3,1)).toInt();
	if(type != 0)
		return;
	
    pan = QString(cmdTxt.mid(4,2)).toInt();
	addr = QString(cmdTxt.mid(6,2)).toInt();
	swno = QString(cmdTxt.mid(8,4)).toInt();
	sw = QString(cmdTxt.mid(12,1)).toInt();
	statusContext += tr("sv_pan:") + QString::number(pan);
	statusContext += ":" + tr("sv_annunciator") + ":";
	statusContext += QString::number(addr) + ":" + QString::number(swno) +":";
	if(sw == 0){
        statusContext += P0_CODE[0];
	}else{
        statusContext += P0_CODE[1];
	}
	addDbItem(Qt::green, "", statusContext);
	
	
	//alertRTUs.append(address);
	
	//qDebug() << "CMD:P:";
	//qDebug() << cmdTxt.toHex();
	//qDebug() << statusContext;
}


void SV_Window::handleCmdQ(QByteArray& cmdTxt)
{
    bool faultFound;
	int type;
    int state;
    int sw;
	QString RTU;
	QString address = "";
	QString rtuType;
    QString statusContext = "";
    QString tmpTxt;
	QStringList	Q6_CODE = {
		tr("Q6_0"),
		tr("Q6_1"),
		tr("Q6_2"),
		tr("Q6_3"),
		tr("Q6_4"),
		tr("Q6_5"),
		tr("Q6_?")
	};
			
	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;

	
	type = QString(cmdTxt.mid(3,1)).toInt();
	switch(type){
		case 0:
			enCmdQueue("Q2");
			break;
		case 1:
			address = QString(cmdTxt.mid(4,8));
			if( (address.size() == 8) && (address[7].toLatin1() == 0x30) ){
				address.replace(7,1,"1");
			}
			
			rtuType = QString(cmdTxt.mid(12,2));
            state = QString(cmdTxt.mid(14,1)).toInt();
            sw = QString(cmdTxt.mid(16,1)).toInt();
			if(rtuType[0].toLatin1() < 0x61 ||
               rtuType[0].toLatin1() > 0x7a){ //RTU type error
                statusContext +=  address + ":種別異常";
                addEventLog(Qt::magenta, address,statusContext);
				break;
			}
			
			RTU = C0_RTU_TYPE.at(rtuType[0].toLatin1() - 0x61);
			statusContext += address + ":" + RTU ;

			if(rtuType[0].toLatin1() == 0x6c){
				break;
			}
			
			if(state==2){  
                statusContext += ":火災報";
				addEventLog(Qt::red, address,statusContext);
				addRtu(address,4.0);
				alertFire(address);
				break;
			}
			
			
            faultFound = false;
            for(int i=31;i < cmdTxt.size()-3;i++){  //dont care ETX,bcc
                if(cmdTxt[i] == (char)0x31){
					faultFound = true;
					int index = i-31;
					if(index >= Q1_CODE.size())
						break;
					
					tmpTxt = statusContext + ":" + Q1_CODE[index];
					if(index == 31 || index == 32){  //control or response
						if(sw == 0){
							addEventLog(Qt::green, address,tmpTxt);
							addRtu(address,5.0);
						}else{
							addEventLog(Qt::blue, address,tmpTxt);
							addRtu(address,6.0);
						}
						
					}else{ //fault
						addEventLog(Qt::magenta, address,tmpTxt);
						addRtu(address,7.0);
					}
					break;
				}
			}
			if(faultFound == true)
				break;
			//not fault , it's control or status report
			if(sw == 0){
                statusContext += ":控制中";
                addEventLog(Qt::blue, address,statusContext);
				addRtu(address,6.0);
			}else{
				if(rtuType[0].toLatin1() == 0x61 ||
				   rtuType[0].toLatin1() == 0x62 ||
				   rtuType[0].toLatin1() == 0x63 ||
				   rtuType[0].toLatin1() == 0x65 ||
				   rtuType[0].toLatin1() == 0x6e ||
				   rtuType[0].toLatin1() == 0x6f ||
				   rtuType[0].toLatin1() == 0x70 ||
				   rtuType[0].toLatin1() == 0x79 ||
                   rtuType[0].toLatin1() == 0x7a   ){   //fire
					addEventLog(Qt::red, address,statusContext);
					addRtu(address,4.0);
					alertFire(address);
				}else{
	                statusContext += ":動作";
	                addEventLog(Qt::blue, address,statusContext);
					addRtu(address,6.0);
				}
			}
			
			break;
		case 2:
            for(int i=4;i<cmdTxt.size()-3;i++){  //dont care ETX,bcc
                if(cmdTxt[i] == (char)0x31){
					int index = i-4;
					if(index >= Q2_CODE.size())
						break;
					tmpTxt = statusContext + Q2_CODE[index];
					addDbItem(Qt::magenta, address,tmpTxt);
				}
			}
			enCmdQueue("Q4");
			break;
		case 3:
			break;
		case 4:
            for(int i=4;i<cmdTxt.size()-3;i++){  //dont care ETX,bcc
                if(cmdTxt[i] == (char)0x31){
					int index = i-4;
					if(index >= Q4_CODE.size())
						break;
					tmpTxt = statusContext + Q4_CODE[index];
					addDbItem(Qt::magenta, address,tmpTxt);
				}
			}
			enCmdQueue("Q6");
			break;
		case 6:
			/*
            for(int i=4;i<cmdTxt.size()-3;i++){  //dont care ETX,bcc
				statusContext = tr("Pan") + QString::number(i-3, 10) + ":"; 
                if(cmdTxt[i] == (char)0x3f){
					tmpTxt = statusContext + Q6_CODE[6];
                    addEventLog(Qt::magenta, address,tmpTxt);
				}else
                if(cmdTxt[i] >= (char)0x31 &&
                    cmdTxt[i] <= (char)0x35){
                    tmpTxt = statusContext + Q6_CODE[cmdTxt[i] - (char)0x31];
					addEventLog(Qt::magenta, address,tmpTxt);
				}
			}*/
			break;
		case 8:
			break;
		case 9:
			break;
		default:
			break;
	}
	
	//addEventLog(Qt::blue, statusContext);
	
	//qDebug() << "CMD:Q:";
	//qDebug() << cmdTxt.toHex();
	//qDebug() << statusContext;
}

void SV_Window::handleCmdR(QByteArray& cmdTxt)
{
	/*
	QString hour;
	QString min;
	QDateTime time;
	QString buf;

	hour = QString(cmdTxt.mid(8,2));
	min = QString(cmdTxt.mid(10,2));
	time = QDateTime::currentDateTime(); 
	buf = time.toString("yyyy-MM-dd hh:mm:ss  ");  
	buf += QString(tr("HRN reset"));
	*/
	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;

	QString statusContext;
	int type;
	type = QString(cmdTxt.mid(3,1)).toInt();
	if(type == 0 || type == 1){
		statusContext = tr("R0_TXT");
		addDbItem(Qt::magenta, "", statusContext);
		eliminateFireLogItem();
        
		//stopAlert();
		//resetSvInfo();
		//QTimer::singleShot(10000, this, SLOT(queryFaultReportHRN()));
	}
	//clear CMD:J state:1 events
	eliminateEventLogItem(Qt::yellow, "","");
	
}

void SV_Window::handleCmdS(QByteArray& cmdTxt){

	QString swNo;
	QString code;
	QString sw;
	int type;
	QString statusContext = "";

	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;

	
	type = QString(cmdTxt.mid(3,1)).toInt();
	swNo = QString(cmdTxt.mid(4,3));
	code = QString(cmdTxt.mid(7,2));
	sw = QString(cmdTxt.mid(9,1));
	
	//qDebug() << "CMD:S:" << type <<":" << swNo << ":" <<code << ":" << sw;
	
	if(type == 0){
		statusContext = S0_CODE.at(code.toInt()-1);
	}else
	if(type == 2){
		statusContext = swNo;
		statusContext += S2_CODE.at(code.toInt()-1);
	}else{
		goto S_EXIT;
	}
	
	if(sw.toInt() == 0){
		statusContext += " : " + tr("S0_SW_0");
	}else
	if(sw.toInt() == 1){
		statusContext += " : " + tr("S0_SW_1");
	}
    
    addDbItem(Qt::magenta, "", statusContext);
	if(code.toInt() == 16){ //reset test
		eliminateFireLogItem();
		redAlarm = 0;
        QString addr = "";
        updateAlertReport(addr,Qt::red,-999); //set to zero
	}
	
	/*
	if(type == 0 && sw.toInt() == 0){
		if(code.toInt() == 9 || code.toInt() == 10 || code.toInt() == 11){
			QTimer::singleShot(5000, this, SLOT(queryFaultReportHRN()));
		}
	}*/
	
S_EXIT:	
    return;
	//qDebug() << "CMD:S:";
	//qDebug() << cmdTxt.toHex();
	//qDebug() << statusContext;
}

void SV_Window::handleCmdT(QByteArray& cmdTxt)
{
	
	int type;
	QString address;
	int SW;
	int code;
	QString statusContext = "";
	QString rtuType;
	QString RTU = "";
	QStringList T0_CODE = {
		tr("T0_CODE_1"),
		tr("T0_CODE_2"),
		tr("T0_CODE_3"),
		tr("T0_CODE_4"),
		tr("T0_CODE_5"),
		tr("T0_CODE_6"),
		tr("T0_CODE_7"),
		tr("T0_CODE_8"),
		tr("T0_CODE_9"),
		tr("T0_CODE_10"),
		tr("T0_CODE_11"),
		tr("T0_CODE_12"),
		tr("T0_CODE_13"),
		tr("T0_CODE_14"),
		tr("T0_CODE_15"),
		tr("T0_CODE_16"),
		tr("T0_CODE_17"),
		tr("T0_CODE_18"),
		tr("T0_CODE_19"),
		tr("T0_CODE_20"),
		tr("T0_CODE_21"),
		tr("T0_CODE_22"),
		tr("T0_CODE_23"),
		tr("T0_CODE_24"),
		tr("T0_CODE_25"),
		tr("T0_CODE_26"),
		tr("T0_CODE_27"),
		tr("T0_CODE_28"),
		tr("T0_CODE_29"),
		tr("T0_CODE_30"),
		tr("T0_CODE_31"),
		tr("T0_CODE_32"),
	};
	QStringList T0_SW_CODE = {
		tr("T0_SW_0"),
        tr("T0_SW_1"),
    };

	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;

	
	address = QString(cmdTxt.mid(4,8));
	if( (address.size() == 8) && (address[7].toLatin1() == 0x30) ){
		address.replace(7,1,"1");
	}
	//address.replace(7,1,"1");

	type = QString(cmdTxt.mid(3,1)).toInt();
	if(type != 0)
		return;
	
	code = QString(cmdTxt.mid(14,2)).toInt();
	if(code < 1 || code > 32)
		return;

	rtuType = QString(cmdTxt.mid(12,2));
	if(rtuType[0].toLatin1() >= 0x61 &&
       rtuType[0].toLatin1() <= 0x7a){ 
       RTU = C0_RTU_TYPE.at(rtuType[0].toLatin1() - 0x61);
	}
		
	SW = QString(cmdTxt.mid(16,1)).toInt();

	statusContext += address + ":" + RTU + ":";
	statusContext += T0_CODE[code-1];// + ":" + T0_SW_CODE[SW];

	if(SW == 1){
		addEventLog(Qt::magenta, address, statusContext);
		addRtu(address,7.0);
	}else
	if(SW == 0){
        statusContext += ":回復";
        addDbItem(Qt::magenta, address, statusContext);
		eliminateEventLogItem(Qt::magenta, address, T0_CODE[code-1]);
		alertRTUs.remove(address);
	}
	

    return;
	
}


void SV_Window::handleCmdV(QByteArray& cmdTxt)
{
	int type;
	
	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;
	
	
	type = QString(cmdTxt.mid(3,1)).toInt();
	if(type == 0){
		QString statusContext = tr("V0_MSG");
		addEventLog(Qt::magenta, "", statusContext);
	}
	//qDebug() << "CMD:V:";
	//qDebug() << cmdTxt.toHex();
	
}

void SV_Window::handleCmdZ(QByteArray& cmdTxt)
{
	int type;

	QByteArray typecmd = cmdTxt.mid(2,2);
    if(!cmdContext.contains(typecmd))
        return ;
    if(cmdContext.value(typecmd) > cmdTxt.length()   ) //4  //4 == 2 start bytes + 2 end bytes
    	return;

	
	type = QString(cmdTxt.mid(3,1)).toInt();
	if(type == 0){
		QTimer::singleShot(3000, this, SLOT(queryFaultReportHRN()));
	}
	//qDebug() << "CMD:Z:";
	//qDebug() << cmdTxt.toHex();
	
}

void SV_Window::parseResp(QByteArray& resp){
	QByteArray cmd;
	QByteArray txt;
  
    switch(resp.at(0)){
		/*
        case 0x05: //ENQ
            //txt = resp.mid(0,1);
			resp.remove(0,1);
            //qDebug() << "resp:" + txt.toHex();
            //send Q1 cmd
            //sendCmdQ1();
			
        break;

        case 0x11: //DC1:
			//txt = resp.mid(0,1);
			//qDebug() << "resp:" + txt.toHex();
			cmd = QByteArray(1,DC1);
            commThread.writeData(cmd);
			//qDebug() << "cmd:" + cmd.toHex();
			resp.remove(0,1);
			break;

		case 0x15: //NAK:
			//txt = resp.mid(0,1);
			//qDebug() << "resp:" + txt.toHex();
			//cmd = QByteArray(1,EOT);
            commThread.writeData(cmdEOT);
			//qDebug() << "cmd:" + cmd.toHex();
			resp.remove(0,1);
			break;
	   	case 0x20:
            if(resp.size() >= 2){
				//txt = resp.mid(0,2);
				//qDebug() << "resp:" + txt.toHex();
				
                //cmd = QByteArray(1,ACK);
				commThread.writeData(cmdACK);
                resp.remove(0,2);
                
			}
			break;
			
		case 0x40:
            if(serialBuf.size() >= 2){
				if(!checkCmdQueueEmpty()){
					deCmdQueue();
				}
				
				//watchDog++;
				//if(watchDog > 20000)
				//	watchDog = 0;
				
				//cmd = QByteArray(1,EOT);
				commThread.writeData(cmdEOT);
                resp.remove(0,2);
				
			}
			break;
		*/	
		case 0x02:
			//got STX, find ETX
			int index;
			int lenCmd;
            index = resp.indexOf(ETX);
			if(index == -1)
				break;
			
			lenCmd = index +2;
			if(resp.size() >= lenCmd){ 
                
				txt = resp.mid(0,lenCmd);
                //qDebug() << "resp:" + txt.toHex();
                //if(switchLogData == true){
                //    errorLog(txt.toHex().data());
                //}
                
				//cmd = QByteArray(1,ACK);
				//commThread.writeData(cmd);

				
                if(queryFaultTimer->isActive()){ //stop queryFaultTimer except B cmd ,
                	if(txt.at(2) != 0x42){
						queryFaultTimer->stop();
						//qDebug() << "parseResp:stop queryFaultTimer.";
                	}
                }
			
				
                if(checkMultiCmds(txt)){
					QList<QByteArray*> list = partCmds(txt);
					foreach(QByteArray* item, list){ 
                        //qDebug() << "resp:multi:" + item->toHex();
						queue.enqueue(item);
						//qDebug() << "resp:multi:-";
					}
                }else{
                	QByteArray *item = new QByteArray(txt);
                	queue.enqueue(item);
                }
                resp.remove(0,lenCmd);
			}
			break;
		/*	
		case 0x04:
			//txt = resp.mid(0,1);
			//qDebug() << "resp:" + txt.toHex();
			//cmd = QByteArray(1,ACK);
			commThread.writeData(cmdACK);
            //qDebug() << "cmd:" + cmd.toHex();
            resp.remove(0,1);
			break;

		case 0x06: //ACK
			txt = resp.mid(0,1);
			//qDebug() << "resp:" + txt.toHex();
	        resp.remove(0,1);
			//cmd = QByteArray(1,EOT);
            commThread.writeData(cmdEOT);
            //qDebug() << "cmd:" + cmd.toHex();
			break;
		*/	
		default:
            txt = resp.mid(0,1);
            qDebug() << "resp:default:" + txt.toHex();
			//cmd = QByteArray(1,EOT);
			//commThread.writeData(cmdEOT);
			//resp.remove(0,1);
			break;
   	}
}

bool SV_Window::checkMultiCmds(QByteArray& txt)
{
    
	QByteArray type = txt.mid(2,2);
	
    if(!cmdContext.contains(type))
        return false;

    if(txt.length() > (cmdContext.value(type)+4)) //4  //4 == 2 start bytes + 2 end bytes
		return true;
	else
		return false;
}

QList<QByteArray*> SV_Window::partCmds(QByteArray txtStr)
{
	QList<QByteArray*> list;
	QByteArray tmpCmd;
    QByteArray* cmd;
	QByteArray type;
    QByteArray* txt = new QByteArray(txtStr);
	char header[2] = {0x02,0x24};

    txt->remove(0,2);
	
    while(txt->length() != 0){
        type = txt->mid(0,2);
        //qDebug() << "SV_Window::partCmds:type:" << type.toHex();
		if(!cmdContext.contains(type))
			break;
		
        tmpCmd = txt->mid(0,cmdContext.value(type));
		tmpCmd.prepend(header,2);
		cmd = new QByteArray(tmpCmd);
		
		list.append(cmd);
        //qDebug() << "SV_Window::partCmds:cmd" << cmd->toHex();
        txt->remove(0,cmdContext.value(type));
        //qDebug() << "SV_Window::partCmds:txt" << txt->toHex();
	}
	//qDebug() << "SV_Window::partCmds:exit";
	return list;
}



void SV_Window::errorLog(QString& text)
{
    //QString text = r);
	QDateTime time; 
	QString timeBuf;
	time = QDateTime::currentDateTime();  
    timeBuf = time.toString("yyyy-MM-dd hh:mm:ss"); 
	
	QString fileName = MW_Window::getScadaSystemFolder()  + "error.txt";
    QFile fOut(fileName);
    if(fOut.open(QFile::Text |QIODevice::WriteOnly | QIODevice::Append)){
		QTextStream s(&fOut);
        s << timeBuf << " " << text  << "\n";
	}else{

      return ;
    }
	fOut.flush();
    fOut.close();
    
}

/*
void SV_Window::errorLog(QString& text)
{
	
	QDateTime time; 
	QString timeBuf;
	time = QDateTime::currentDateTime();  
    timeBuf = time.toString("yyyy-MM-dd hh:mm:ss"); 
	
	QString fileName = MW_Window::getScadaSystemFolder()  + "error.txt";
    QFile fOut(fileName);
    if(fOut.open(QFile::Text |QIODevice::WriteOnly | QIODevice::Append)){
		QTextStream s(&fOut);
        s << timeBuf << ":" << text  << "\n";
	}else{

      return ;
    }
	fOut.flush();
    fOut.close();
    
}
*/
void SV_Window::voiceAlert()
{
	//if(playSound){
	//	playSound();
	//}
	QSound::play("onfire.wav");
}

void SV_Window::addRtu(QString& addr,double type)
{
	alertRTUs.insert(addr, type);
}

void SV_Window::alertFire(QString& addr)
{
	//playSoundThread* thread = new playSoundThread();
	//thread->start();
	
	voiceAlert();

	if(!rtuMap.contains(addr)){
		
        addDbItem(Qt::magenta,addr,tr("sv_can not find rtu in map:")+addr);
		return;
	}
	fireAlertRTUs.clear();
	fireAlertRTUs.insert(addr,4.0);
    QString str = addr + "_file";
    activateMapFileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + rtuMap.value(str);
	openMap(activateMapFileName,Alert_Fire,true);
	
}

void SV_Window::alertR4(QString& addr)
{
    
	if(!rtuMap.contains(addr)){
        addDbItem(Qt::magenta,addr,tr("sv_can not find rtu in map:")+addr);
		return;
	}
	QString str = addr + "_file";
    activateMapFileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + rtuMap.value(str);
	updateMap(activateMapFileName,Alert_R4);

	
}

void SV_Window::alertFault(QString& addr)
{
	if(!rtuMap.contains(addr)){
        addDbItem(Qt::magenta,addr,tr("sv_can not find rtu in map:")+addr);
		return;
	}
	QString str = addr + "_file";
    activateMapFileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + rtuMap.value(str);
	updateMap(activateMapFileName,Alert_Fault);
}

void SV_Window::alertFireCheck(QString& addr)
{
	qDebug() << "alertFireCheck++";
	if(!rtuMap.contains(addr)){
		qDebug() << "alertFireCheck 2";
        addDbItem(Qt::magenta,addr,tr("sv_can not find rtu in map:")+addr);
		return;
    }
    QString str = addr + "_file";
    activateMapFileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + rtuMap.value(str);
    qDebug() << "alertFireCheck:" << activateMapFileName;
	openMap(activateMapFileName,Alert_Fire,false);
}

void SV_Window::alertControlCheck(QString& addr)
{
	if(!rtuMap.contains(addr)){
        addDbItem(Qt::magenta,addr,tr("sv_can not find rtu in map:")+addr);
		return;
	}
	QString str = addr + "_file";
    activateMapFileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + rtuMap.value(str);
	openMap(activateMapFileName,Alert_R4,false);
	
}


void SV_Window::alertR4Check(QString& addr)
{
	/*
    qDebug() << "SV_Window::alertR4Check";
    QString addr2 = addr;
    if(addr2.length() == 12){  //last char is 0 or 1 
    	qDebug() << "SV_Window::alertR4Check:addr2";
        addr2[addr.length()-1] = '0';
    }
	if(!rtuMap.contains(addr) && !rtuMap.contains(addr2)){
		qDebug() << "sv can not find rtu in map";
        addDbItem(Qt::magenta,addr,tr("sv can not find rtu in map:")+addr);
		return;
	}*/
	if(!rtuMap.contains(addr)){
		qDebug() << "sv can not find rtu in map";
        addDbItem(Qt::magenta,addr,tr("sv can not find rtu in map:")+addr);
		return;
	}
	
	QString str = addr + "_file";
    activateMapFileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + rtuMap.value(str);
	openMap(activateMapFileName,Alert_R4,false);
	
}

void SV_Window::alertFaultCheck(QString& addr)
{
	if(!rtuMap.contains(addr)){
        addDbItem(Qt::magenta,addr,tr("sv_can not find rtu in map:")+addr);
		return;
	}
	QString str = addr + "_file";
    activateMapFileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + rtuMap.value(str);
	openMap(activateMapFileName,Alert_Fault,false);
	
}



void SV_Window::eliminateAlert(QString& addr,Qt::GlobalColor color)
{
/*
	if(!rtuMap.contains(addr)){
		addDbItem(Qt::magenta,"",tr("sv_can not find rtu in map:")+addr);
		return;
	}
	QString str = addr + "_file";
    activateMapFileName = MW_Window::getScadaSystemFolder() + MW_Window::getProjectFolder() + rtuMap.value(str);
	
	if(color == Qt::blue)
        updateMap(activateMapFileName,Alert_R4);
	else
	if(color == Qt::red)
        updateMap(activateMapFileName,Alert_Fire);
	else
	if(color == Qt::magenta)
        updateMap(activateMapFileName,Alert_Fault);
*/	
	
	
}



void SV_Window::stopAlert()
{ 
	if(mapWindow == NULL)
		return;
	
	mapWindow->stopAlert();
	double color(5.0); //green
	mapWindow->setSvMode(color,false, eventList);
	mapWindow->activateMapWindow(activateMapFileName);
		
  
	this->show();
	QApplication::setActiveWindow(this);
	this->activateWindow();
 
	
}

void SV_Window::slotPrintPreviewChanged(bool state)
{
	if(mapWindow == NULL)
		return;
	
    if(state == false){
		mapWindow->activateMapWindow(activateMapFileName);
		//qDebug() << "SV_Window::slotPrintPreviewChanged";
	
	}
}

void SV_Window::openMap(QString& filename, AlertType type,bool print)
{
	
	qDebug() << "openMap";
	closeMapWindow();

	if(mapWindow == NULL){
		createMapWindow();
	}

	//if(mapWindow->isVisible()){
	//	return;
	//}
		
	mapWindow->setSensorDock(false);
	mapWindow->slotFileOpen(filename, RS2::FormatDXFRW);
	mapWindow->show();
	QApplication::setActiveWindow(mapWindow);
	mapWindow->setGridMode(false);
	mapWindow->activateWindow();
	double color(5.0);
    mapWindow->setSvMode(color,true, eventList);
	mapWindow->slotViewDraft(false);
	mapWindow->slotZoomAuto();
	//mapWindow->setGridMode(false);
	
	
	if(type == Alert_Fire){
		qDebug() << "openMap:Alert_Fire.";
		mapWindow->startAlert(alertRTUs);
	}
	else
	if(type == Alert_R4){
		qDebug() << "openMap:Alert_R4.";
		mapWindow->startAlert(alertRTUs);
	}
	else
	if(type == Alert_Fault){
		qDebug() << "openMap:Alert_Fault.";
		mapWindow->startAlert(alertRTUs);
	}
	//check if auto print is true
	
	if(print && autoPrintBtn->isChecked()){
		mapWindow->slotFilePrint(fireAlertRTUs);
		//mapWindow->slotFilePrintPreview(false);
	}
	//mapWindow->showMaximized();
	//mapWindow->activateMapWindow(filename);
}

void SV_Window::updateMap(QString& filename, AlertType type)
{
	if(mapWindow == NULL){
		createMapWindow();
		return;
	}


	mapWindow->setSensorDock(false);
	mapWindow->slotFileOpen(filename, RS2::FormatDXFRW);
	//mapWindow->show();
	//QApplication::setActiveWindow(mapWindow);
	mapWindow->setGridMode(false);
	//mapWindow->activateWindow();
	double color(5.0);
    mapWindow->setSvMode(color,true, eventList);
	mapWindow->slotZoomAuto();
	mapWindow->slotViewDraft(false);
	mapWindow->setGridMode(false);
	
	
	if(type == Alert_Fire){
		//qDebug() << "openMap:Alert_Fire.";
		mapWindow->startAlert(alertRTUs);
	}
	else
	if(type == Alert_R4){
		//qDebug() << "openMap:Alert_R4.";
		mapWindow->startAlert(alertRTUs);
	}
	else
	if(type == Alert_Fault){
		//qDebug() << "openMap:Alert_Fault.";
		mapWindow->startAlert(alertRTUs);
	}
	//mapWindow->showMaximized();
	//check if auto print is true
	
	//if(autoPrintBtn->isChecked()){
	//	mapWindow->slotFilePrintPreview(true);
	//	mapWindow->slotFilePrintPreview(false);
	//}
	//mapWindow->activateMapWindow(filename);
}

void SV_Window::slotLwDoubleClicked(QListWidgetItem* item)
{
	qDebug() << "SV_Window::slotLwDoubleClicked";

	QBrush bh = item->foreground();
	QString addr = item->whatsThis();

	

	if(addr.length() == 0)
		return;
	
	if(bh.color() == Qt::red){
        alertFireCheck(addr);
	}else
	if(bh.color() == QColor(0x00,0x80,0xff,0xff)){
		//qDebug() << "alertR4Check";
		alertR4Check(addr);
	}else
	if(bh.color() == Qt::green){
		alertControlCheck(addr);
	}else
	if(bh.color() == Qt::magenta){
		alertFaultCheck(addr);
	}
}

void SV_Window::createMapWindow()
{
	//create mapwindow
	mapWindow = new QC_ApplicationWindow();
	connect(mapWindow,SIGNAL(qc_AppWindowlose()),this,SLOT(slotQC_AppWindowClosed()));
    mapWindow->setWindowTitle("SCADA");
	mapWindow->setSvWindow(this);
	mapWindow->slotRunStartScript();
	mapWindow->hide();
	//mapWindow->setWindowFlags(Qt::CustomizeWindowHint);
	//mapWindow->setWindowFlags(Qt::WindowCloseButtonHint);
}

void SV_Window::closeMapWindow()
{
	if(mapWindow != NULL){

		QDialog * dlg = mapWindow->findChild<QDialog *>();
	  	if(dlg != NULL){
			qDebug() << "closeMapWindow:QDialog found";
			dlg->reject();
	  	}
	  	
	  	QList<QDockWidget *> docks = mapWindow->findChildren<QDockWidget *>();
	  	if(docks.size() > 0){
			qDebug() << "closeMapWindow:docks found";
            foreach (QDockWidget* wgt,docks){
            	qDebug() << "closeMapWindow:wgt found";
            	wgt->hide();
            	wgt->destroyed();
            }
            
	  	}
	  		  	
	  	mapWindow->hide();
		mapWindow->setEdMode();
		mapWindow->slotFileClose();
		update();
		//mapWindow->close();
		//mapWindow->deleteLater();
		//delete mapWindow;
		//mapWindow = NULL;
	}
}
//////////////////////////////////////////////////////
//slots for commthread
void SV_Window::commError(const QString &s)
{
    qDebug() << s;

}

void SV_Window::commTimeout(const QString &s)
{
    //qDebug() << "Status: Running, %1." << s;
    
}



