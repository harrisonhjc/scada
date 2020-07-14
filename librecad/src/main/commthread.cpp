#include "commthread.h"
#include <QTime>
#include <QIODevice>
#include <QDebug>
#include <QHostAddress>
#include <QDataStream>
#include <QtEndian>
#include <QTimer>
#include <time.h>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include "mw_window.h"
#include "mce.h"

QT_USE_NAMESPACE

CommThread::CommThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
    node = 1;
    mgwIP = "";
    readIP();
}

CommThread::~CommThread()
{
    mutex.lock();
    quit = true;
    mutex.unlock();
    wait();
}

bool CommThread::startCommWorker()
{
    //get mgw ip

	connect(this, SIGNAL(pushPacket(int*, int)), this, SLOT(writePacket(int*, int)));
    connect(this, SIGNAL(updateConnectStatus(ClientStatus)), this, SLOT(updateSocketStatus(ClientStatus)) );
	
    connect(&m_socket, SIGNAL(readyRead()), this, SLOT(onReceive()));
    connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onConnectionError()));
    connect(&m_socket, SIGNAL(connected()), this, SLOT(onNewConnection()));
    connect(&m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
    connect(this, SIGNAL(typeEventId()), this, SLOT(typeEventIdHandler()));
	connectMG();
    
    if (!isRunning())
        start();

	return true;
}

bool CommThread::stopCommWorker()
{
	mutex.lock();
    quit = true;
    mutex.unlock();
    m_socket.disconnectFromHost();
    m_socket.waitForDisconnected(1000);
    wait();
	
	
	return true;
    
}

void CommThread::readIP()
{
    QString fileName = MW_Window::getScadaSystemFolder() + "mgw.profile";
    
    QFile fIn(fileName);
    if (fIn.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream sIn(&fIn);
        mgwIP = sIn.readLine();
    }
    fIn.close();
    if(mgwIP.length() == 0){
        mgwIP = "192.168.0.162";
    }
}

bool CommThread::connectMG()
{
	qDebug() << "connectMG" << "\n";
    //strcpy(SERVER_IP, mgwIP);
    SERVER_PORT =100;

/*
    connect(&m_socket, SIGNAL(connected()), this, SLOT(onNewConnection()));
    connect(&m_socket, SIGNAL(readyRead()), this, SLOT(onReceive()));
    connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onConnectionError()));
    connect(&m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
 */   
   
    m_socket.connectToHost(mgwIP.toLocal8Bit().constData(), SERVER_PORT);
    bool isInitialized = m_socket.state() == QAbstractSocket::ConnectingState;

    if (!isInitialized)
    {
        m_clientStatus = CLIENT_STATUS_NOK;
        qDebug() << "Failed to connect to the server\n";
    }

    return true;
}


void CommThread::writeData(int* data, int len)
{
	for(int i=0;i<len;i++){
        int t = *(data+i);
        t = qToBigEndian(t);
        m_socket.write((char*)&t, sizeof(int));
    }
    m_socket.flush();
   
}


void CommThread::run()
{
    int count = 0;
    while(!quit){

    	sync.lock();
        //cond.wait(&sync);
        if(!queue.isEmpty()){
            QByteArray data = queue.takeFirst();
            parseResp(data);
        }
        /*
        else{
        	msleep(1000);
            continue;
        }*/
        sync.unlock();
        msleep(500);
        //qDebug() << "CommThread loop " << count++ << "\n";
    }
    
	qDebug() << "CommThread end.";


}

void CommThread::parseResp(QByteArray& buf)
{
	int len;
    char packet[255];

    //parse packets if buf contains not only one packet.

    while(buf.size() > 0){
        unsigned char ch = buf.at(3);

        len = (int)ch;
        //qDebug() << "parseData:len=" << len << "\n";
        QByteArray packet = buf.left(len);
        buf.remove(0,len);
        packetHandler(packet);

    }
}

void CommThread::packetHandler(QByteArray& packet)
{
    //qDebug() << "packet len=" << packet.length() <<"\n";
    //qDebug() << packet.toHex() << "\n";
    int len;
    int msgType;

    QDataStream ds(packet);
    ds >> len;
    ds >> msgType;

    switch(msgType){
    case PACKET_TYPE_EVENT_START:
        //qDebug() << "packetHandler:PACKET_TYPE_EVENT_START" << "\n";
    	initStatus = 2;
        eventStart(packet);
        break;

     case PACKET_TYPE_EVENT_CLEAR:
        //qDebug() << "packetHandler:PACKET_TYPE_EVENT_START" << "\n";
        eventClear(packet);
        break;

    case PACKET_TYPE_EVENT_ID:
        //qDebug() << "packetHandler:PACKET_TYPE_EVENT_ID" << "\n";
        eventEventId(packet);
        //sendAck();
        emit typeEventId();
        break;

    case PACKET_TYPE_ACK:
        //qDebug() << "packetHandler:PACKET_TYPE_ACK" << "\n";
        break;
    }
}

void CommThread::typeEventIdHandler()
{
    qDebug() << "typeEventIdHandler:close socket";
    m_socket.close();
}

void CommThread::enQueue(QByteArray req)
{
   //sync.lock();
   //qDebug() << "enQueue" << "\n";
   queue.append(req);
   //sync.unlock();
   //cond.wakeAll();
}


void CommThread::enCmdQueue(QString var)
{
	//qDebug() << "CommThread::enCmdQueue:" << QTime::currentTime().toString("hh:mm:ss:zzz") << ":" << var;
	muxCmdToSend.lock();
	qCmdToSend.append(var);
	muxCmdToSend.unlock();
	//cond.wakeAll();
}



void CommThread::writePacket(int* data, int len)
{
    for(int i=0;i<len;i++){
        int t = *(data+i);
        t = qToBigEndian(t);
        m_socket.write((char*)&t, sizeof(int));
    }
    m_socket.flush();

}

void CommThread::sendPacket(int* data, int len)
{
    //qDebug() << "sendPacket:len=" << len << "\n";

    emit pushPacket(data,len);

    //qDebug() << "\n";
}

void CommThread::onReceive()
{
     sync.lock();
    QByteArray data = m_socket.readAll();

    enQueue(data);
    sync.unlock();
    cond.wakeAll();
    //qDebug() << "onReceive:packet len=" << data.length() <<"\n";
    //qDebug() << data.toHex() << "\n";



}


void CommThread::onNewConnection()
{
    qDebug() << "onNewConnection";
	
    initStatus = 0;
    
	reqActiveEvent();
    
    m_clientStatus = CLIENT_STATUS_IN_REQUEST_EVENT;
    emit this->updateConnectStatus(m_clientStatus);
    qDebug() << "onNewConnection:emit updateConnectStatus";
    TypeConnectStatus s;
    s.status = 1;
    emit this->connectNotify(s);
		
}
void CommThread::onSndConnection()
{
    

	initStatus = 0;
   
    m_clientStatus = CLIENT_STATUS_IN_EVENT_LOG;
    emit this->updateConnectStatus(m_clientStatus);
    qDebug() << "onSndConnection:emit updateConnectStatus";
   
    qDebug() << "onSndConnectionn:" << m_clientStatus;
   	//reqEventLog();
    eventLogTimer = new QTimer(this);
    connect(eventLogTimer, SIGNAL(timeout()), this, SLOT(sendReqEventLog()));
    eventLogTimer->setSingleShot(true);
    eventLogTimer->start(5000);
    TypeConnectStatus s;
    s.status = 1;
    emit this->connectNotify(s);
}

void CommThread::sendReqEventLog()
{
    qDebug() << "sendReqEventLog++++++++++++++++++++++";
    reqEventLog();
}

void CommThread::onConnectionError()
{
    //m_clientStatus = CLIENT_STATUS_NOK;
    qDebug() << "connection error:" << m_clientStatus;
}

void CommThread::updateSocketStatus(ClientStatus s)
{

    
    m_clientStatus = s;
     qDebug() << "updateSocketStatus:" << m_clientStatus;
    
}

void CommThread::onDisconnection()
{
   
    qDebug() << "onDisconnection:" << m_clientStatus;
    
    
    if(m_clientStatus == CLIENT_STATUS_IN_EVENT_LOG){
        
        qDebug() << "onDisconnection abnornally";
        m_clientStatus = CLIENT_STATUS_IDLE;
        emit this->updateConnectStatus(m_clientStatus);
        m_socket.disconnectFromHost();
        TypeConnectStatus s;
        s.status = 0;
        emit this->connectNotify(s);
        return;
    }
    
    TypeConnectStatus s;
    s.status = 0;
    emit this->connectNotify(s);
    disconnect(&m_socket, SIGNAL(connected()), 0, 0);
    connect(&m_socket, SIGNAL(connected()), this, SLOT(onSndConnection()));
    //connect(&m_socket, SIGNAL(readyRead()), this, SLOT(onReceive()));
    //connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onConnectionError()));
    //connect(&m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnection()));

    if(!quit){
        QTimer::singleShot(2000, this, SLOT(connectMG()));
         m_clientStatus = CLIENT_STATUS_DISCONNECTED;
    }
    
}




void CommThread::eventStart(QByteArray& packet)
{
    TypeMceEventLog ps;
    QDataStream ds(packet);
    ds >> ps.length
       >> ps.messageType
       >> ps.serialNumber
       >> ps.eventGroup
       >> ps.eventType
       >> ps.eventCode
       >> ps.node
       >> ps.addressType
       >> ps.address
       >> ps.subAddress
       >> ps.loop
       >> ps.zone
       >> ps.InputAction
       >> ps.timeStamp;

    ps.loop++; //0 base   
    latestSerialNumber =  ps.serialNumber;  
    node = ps.node;
    ps.locationText.resize(80);
    ps.panelText.resize(32);
    ps.zoneText.resize(80);
    ds.readRawData(ps.locationText.data(), 80);
    ds.readRawData(ps.panelText.data(), 32);
    ds.readRawData(ps.zoneText.data(), 80);
    
    sendEventAck(ps.serialNumber);
    emit this->request(ps);
}

void CommThread::eventClear(QByteArray& packet)
{
    TypeMceEventLog ps;
    QDataStream ds(packet);
    ds >> ps.length
       >> ps.messageType
       >> ps.serialNumber
       >> ps.eventGroup
       >> ps.eventType
       >> ps.eventCode
       >> ps.node
       >> ps.addressType
       >> ps.address
       >> ps.subAddress
       >> ps.loop
       >> ps.zone
       >> ps.InputAction
       >> ps.timeStamp;

    ps.loop++; //0 base
    ps.locationText.resize(80);
    ps.panelText.resize(32);
    ps.zoneText.resize(80);
    ds.readRawData(ps.locationText.data(), 80);
    ds.readRawData(ps.panelText.data(), 32);
    ds.readRawData(ps.zoneText.data(), 80);

    
    sendEventAck(ps.serialNumber);
    emit this->request(ps);
}


void CommThread::eventEventId(QByteArray& packet)
{
    int length;
    int messageType;
    int serialNumber;

    QDataStream ds(packet);
    ds >> length
       >> messageType
       >> serialNumber;

    qDebug() << "eventEventId:length=" << length << "\n";
    qDebug() << "eventEventId:messageType=" << messageType << "\n";
    qDebug() << "eventEventId:serialNumber=" << serialNumber << "\n";
    if(latestSerialNumber < serialNumber)
        latestSerialNumber = serialNumber;

    qDebug() << "latestSerialNumber:" << latestSerialNumber << "\n";
    //sendAck();
}

void CommThread::sendAck()
{
    int len = sizeof(MSG_PACKET_TYPE_ACK);
    qDebug() << "MSG_PACKET_TYPE_ACK len=" << len <<"\n";
    sendPacket(MSG_PACKET_TYPE_ACK,
               sizeof(MSG_PACKET_TYPE_ACK)/sizeof(int));
}

void CommThread::sendEventAck(int serialNum)
{
    int len = sizeof(MSG_PACKET_TYPE_EVENT_ACK);
    //qDebug() << "MSG_PACKET_TYPE_EVENT_ACK len=" << len <<"\n";
    MSG_PACKET_TYPE_EVENT_ACK[2] = serialNum;
    sendPacket(MSG_PACKET_TYPE_EVENT_ACK,
               sizeof(MSG_PACKET_TYPE_EVENT_ACK)/sizeof(int));

}

void CommThread::reqActiveEvent()
{
    int len = sizeof(MSG_PACKET_TYPE_REQUEST_ACTIVE_EVENTS);
    qDebug() << "MSG_PACKET_TYPE_REQUEST_ACTIVE_EVENTS len=" << len <<"\n";
    sendPacket(MSG_PACKET_TYPE_REQUEST_ACTIVE_EVENTS,
               sizeof(MSG_PACKET_TYPE_REQUEST_ACTIVE_EVENTS)/sizeof(int));

}

void CommThread::reqEventLog()
{
    int len = sizeof(MSG_PACKET_TYPE_REQUEST_EVENT_LOG);
    qDebug() << "MSG_PACKET_TYPE_REQUEST_EVENT_LOG len=" << len <<"\n";
    //start get log from latest serial number
    MSG_PACKET_TYPE_REQUEST_EVENT_LOG[2] = latestSerialNumber;
    sendPacket(MSG_PACKET_TYPE_REQUEST_EVENT_LOG,
               sizeof(MSG_PACKET_TYPE_REQUEST_EVENT_LOG)/sizeof(int));


}

void CommThread::reqGetTime()
{
    int len = sizeof(MSG_PACKET_TYPE_REQUEST_CURRENT_TIME);
    qDebug() << "MSG_PACKET_TYPE_REQUEST_CURRENT_TIME len=" << len <<"\n";
    sendPacket(MSG_PACKET_TYPE_REQUEST_CURRENT_TIME,
               sizeof(MSG_PACKET_TYPE_REQUEST_CURRENT_TIME)/sizeof(int));
}

void CommThread::reqSetTime()
{
    int len = sizeof(MSG_PACKET_TYPE_SET_TIME);
    qDebug() << "MSG_PACKET_TYPE_SET_TIME len=" << len <<"\n";

    time_t secondsY2K = 946684800; 
    time_t unixEpochTime;
    time(&unixEpochTime);
    double seconds = difftime(unixEpochTime , secondsY2K);
    qDebug() << "reqSetTime:" << seconds << "\n";

    MSG_PACKET_TYPE_SET_TIME[2] = (int)seconds;
    sendPacket(MSG_PACKET_TYPE_SET_TIME,
               sizeof(MSG_PACKET_TYPE_SET_TIME)/sizeof(int));
}

void CommThread::reqResetMGW()
{
    
    int len = sizeof(MSG_PACKET_TYPE_REQUEST_RESET);
    qDebug() << "MSG_PACKET_TYPE_REQUEST_RESET len=" << len <<"\n";
    MSG_PACKET_TYPE_REQUEST_RESET[2] = node;
    sendPacket(MSG_PACKET_TYPE_REQUEST_RESET,
               sizeof(MSG_PACKET_TYPE_REQUEST_RESET)/sizeof(int));
}

void CommThread::reqMainSpeakerOff()
{
    
    int len = sizeof(MSG_PACKET_TYPE_REQUEST_SILENCE);
    qDebug() << "MSG_PACKET_TYPE_REQUEST_SILENCE len=" << len <<"\n";
    MSG_PACKET_TYPE_REQUEST_SILENCE[2] = node;
    sendPacket(MSG_PACKET_TYPE_REQUEST_SILENCE,
               sizeof(MSG_PACKET_TYPE_REQUEST_SILENCE)/sizeof(int));
    
}


void CommThread::setHrnReset()
{
    qDebug() << "setHrnReset" <<"\n";
    disconnect(&m_socket, SIGNAL(connected()), 0, 0);
    disconnect(&m_socket, SIGNAL(disconnected()), 0, 0);
    connect(&m_socket, SIGNAL(connected()), this, SLOT(onHrnReset()));
    m_socket.close();
    connectMG();
}

void CommThread::onHrnReset()
{
    initStatus = 0;
    m_clientStatus = CLIENT_STATUS_OK;
    qDebug() << "Connected:onHrnReset\n";
    reqResetMGW();
    disconnect(&m_socket, SIGNAL(connected()), 0, 0);
    //connect(&m_socket, SIGNAL(connected()), this, SLOT(onNewConnection()));
    connect(&m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
    reqActiveEvent();
    //m_socket.close();
    //QTimer::singleShot(5000, this, SLOT(connectMG()));
    
}

void CommThread::setMainSpeakerOff()
{
    qDebug() << "setMainSpeakerOff" <<"\n";
    disconnect(&m_socket, SIGNAL(connected()), 0, 0);
    disconnect(&m_socket, SIGNAL(disconnected()), 0, 0);
    connect(&m_socket, SIGNAL(connected()), this, SLOT(onMainSpeakerOff()));
    m_socket.close();
    connectMG();

    
}
void CommThread::onMainSpeakerOff()
{
    initStatus = 0;
    m_clientStatus = CLIENT_STATUS_OK;
    qDebug() << "Connected:onMainSpeakerOff\n";
    reqMainSpeakerOff();
    //setDeviceOff(0,1,6,1,1);
    disconnect(&m_socket, SIGNAL(connected()), 0, 0);
    //connect(&m_socket, SIGNAL(connected()), this, SLOT(onNewConnection()));
    connect(&m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
    reqActiveEvent();
    //m_socket.close();
    //QTimer::singleShot(5000, this, SLOT(connectMG()));
}
void CommThread::setLocalSpeakerOff()
{
    //set local speaker off
    
    //qDebug() << "CMD:local speaker off>>:";
    //qDebug() << cmd.toHex();
}

void CommThread::setDeviceOff(int state,int loop,int addr,int subAddr, int zone)
{
    int len = sizeof(MSG_PACKET_TYPE_CONTROL);
    qDebug() << "MSG_PACKET_TYPE_CONTROL len=" << len <<"\n";
    MSG_PACKET_TYPE_CONTROL[2] = state;
    MSG_PACKET_TYPE_CONTROL[3] = 0;
    MSG_PACKET_TYPE_CONTROL[4] = loop;
    MSG_PACKET_TYPE_CONTROL[5] = addr;
    MSG_PACKET_TYPE_CONTROL[6] = subAddr;
    MSG_PACKET_TYPE_CONTROL[7] = zone;
    sendPacket(MSG_PACKET_TYPE_CONTROL,
               sizeof(MSG_PACKET_TYPE_CONTROL)/sizeof(int));

}



void CommThread::sendCmdQ1(QextSerialPort *serial)
{
	//system fault inquiry
	
	//qDebug() << "CMD:Inquiry system fault>>:";
	//qDebug() << cmd.toHex();
}
void CommThread::sendCmdQ2(QextSerialPort *serial)
{
	//system fault inquiry
	
	//qDebug() << "CMD:Inquiry system fault>>:";
	//qDebug() << cmd.toHex();
}

void CommThread::sendCmdQ4(QextSerialPort *serial)
{
	//system fault inquiry
	
	//qDebug() << "CMD:Inquiry system fault>>:";
	//qDebug() << cmd.toHex();
}
void CommThread::sendCmdQ6(QextSerialPort *serial)
{
	//system fault inquiry
	
	//qDebug() << "CMD:Inquiry system fault>>:";
	//qDebug() << cmd.toHex();
}



void CommThread::sendCmdC1(QextSerialPort *serial, QByteArray& rtuAddress, int sw)
{
	// on the supervising mode , press the RTU icon, and send cmd to HRN to enable/disable RTU. 
	
	//qDebug() << "CMD:set RTU state>>:";
	//qDebug() << cmd.toHex();
}

void CommThread::slotGetCmdQueue(const QString &cmd)
{
    //qDebug() << "CommThread::slotGetCmdQueue:" << QTime::currentTime().toString("hh:mm:ss:zzz") <<":" << cmd;
    enCmdQueue(cmd);
}

