#ifndef COMMTHREAD_H
#define COMMTHREAD_H
#include "qextserialport.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
//#include <QtSerialPort/QSerialPort>
#include <QIODevice>
#include <QFlags>
#include "settingsdialog.h"
#include <QQueue>
#include <QList>
#include <QTcpSocket>
#include "packetType.h"


class CommThread : public QThread
{
    Q_OBJECT

public:
    int latestSerialNumber;
    int node;
     enum ClientStatus
    {
        CLIENT_STATUS_IDLE = 0,
        CLIENT_STATUS_OK,
        CLIENT_STATUS_NOK,
        CLIENT_STATUS_DISCONNECTED,
        CLIENT_STATUS_IN_REQUEST_EVENT,
        CLIENT_STATUS_IN_EVENT_LOG
    };

    CommThread(QObject *parent = 0);
    ~CommThread();

    bool startCommWorker();
	bool stopCommWorker();
    void writeData(int* data, int len);
    void run();
    
	//cmd queue
	QQueue<QString> qCmdToSend;
	QMutex muxCmdToSend;

    void enQueue(QByteArray req);
	void enCmdQueue(QString var);
    void packetHandler(QByteArray& packet);
    void sendAck();
    void sendEventAck(int);
    void eventStart(QByteArray& packet);
    void eventClear(QByteArray& packet);
    void eventEventId(QByteArray& packet);
	void reqActiveEvent();
    void reqEventLog();
    void reqResetMGW();
    void reqGetTime();
    void reqSetTime();
    void reqMainSpeakerOff();
    void setMainSpeakerOff();
	void setHrnReset();
	void setLocalSpeakerOff();
    void setDeviceOff(int state,int loop,int addr,int subAddr, int zone);

	void sendCmdQ1(QextSerialPort *serial);
	void sendCmdQ2(QextSerialPort *serial);
	void sendCmdQ4(QextSerialPort *serial);
	void sendCmdQ6(QextSerialPort *serial);
	void sendCmdC1(QextSerialPort *serial,QByteArray& rtuAddress, int sw);

signals:
    void request(const TypeMceEventLog &s);
    void error(const QString &s);
    void timeout(const QString &s);
    void pushPacket(int* data, int len);
    void toConnectMG();
    void connectNotify(TypeConnectStatus &s);
    void updateConnectStatus(ClientStatus s);
    void typeEventId();
    

public slots:
    void typeEventIdHandler(); 
    void sendReqEventLog(); 
	void slotGetCmdQueue(const QString &cmd);
    void onNewConnection();
    void onSndConnection();
    void onReceive();
    void onConnectionError();
    void writePacket(int* data, int len);
    void onDisconnection();
    bool connectMG();
    void onHrnReset();
    void onMainSpeakerOff();
    void updateSocketStatus(ClientStatus s);

	
private:
    QTimer *eventLogTimer;
    QTimer *typeEventTimer;
    int initStatus;
    void parseData(QByteArray);

    QList<QByteArray> queue;
    QTcpSocket m_socket;
    ClientStatus m_clientStatus;
    QWaitCondition cond;
    char SERVER_IP[128];
    unsigned short SERVER_PORT;
    QextSerialPort *serial;
    void parseResp(QByteArray& resp);
    QString portName;
    QString response;
    int waitTimeout;
    QMutex mutex;
    QMutex sync;
    bool quit;
	char DC1;
	char STX;
	char ETX;
    char EOT;
	char ENQ;
	char ACK;
	char NAK;
	char POL;
	char cmdEOT[2];
	char cmdACK[2];
	
	struct Settings {
        QString name;
        BaudRateType baudRate;
        QString stringBaudRate;
        DataBitsType dataBits;
        QString stringDataBits;
        ParityType parity;
        QString stringParity;
        StopBitsType stopBits;
        QString stringStopBits;
        FlowType flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    } settings;
    
    QString mgwIP;
    void sendPacket(int* data, int len);
    void readIP();

};

#endif // COMMTHREAD_H
