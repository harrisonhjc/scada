#ifndef NCLIENT_H
#define NCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <iostream>
#include <string>
#include <sstream>
#include <QTimer>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QList>
#include "packetType.h"



class NClient : public QThread
{
    Q_OBJECT
public:
    enum ClientStatus
    {
        CLIENT_STATUS_IDLE = 0,
        CLIENT_STATUS_OK,
        CLIENT_STATUS_NOK,
        CLIENT_STATUS_DISCONNECTED
    };

    NClient();
    ~NClient();
    bool init();
    void sendHeartbeat();
    void sendStartConnectionMonitoring();
    void sendStopConnectionMonitoring();
    void getCurrentTime();
    void reqActiveEvent();
    void reqEventLog();
    void getNodeNum();
    void sendAck();
    void sendEventAck(int);
    void enQueue(QByteArray);

public slots:
    void onNewConnection();
    void onReceive();
    void onConnectionError();
    void stop();
    void parseData(QByteArray);
    void writePacket(int* data, int len);
    void onDisconnection();

protected:
    void run();

private:
    PACKET_TYPE_START MsgStart;
    const char* SERVER_IP;
    unsigned short SERVER_PORT;
    QTcpSocket m_socket;
    ClientStatus m_clientStatus;
    quint16 m_blockSize;

    QMutex mMutex;
    bool mStop;
    QList<QByteArray> queue;
    QMutex sync;
    QWaitCondition cond;
    void packetHandler(QByteArray&);
    void eventStart(QByteArray& packet);
    void eventEventId(QByteArray& packet);
    void sendPacket(int* data, int len);

signals:
    void pushPacket(int* data, int len);



};




#endif // NCLIENT_H
