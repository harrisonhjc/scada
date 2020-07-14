#include <QDebug>
#include <QTimer>
#include <QHostAddress>
#include <QDataStream>
#include <QtEndian>

#include "nclient.h"
#include "mce.h"


NClient::NClient()
{
    mStop = false;
    SERVER_IP = "192.168.0.162";
    SERVER_PORT =100;

    connect(&m_socket, SIGNAL(connected()), this, SLOT(onNewConnection()));
    connect(&m_socket, SIGNAL(readyRead()), this, SLOT(onReceive()));
    connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onConnectionError()));
    connect(&m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
    connect(this, SIGNAL(pushPacket(int*, int)), this, SLOT(writePacket(int*, int)));

}

NClient::~NClient()
{
}

bool NClient::init()
{
    qDebug() << "init" << "\n";
    m_socket.connectToHost(SERVER_IP, SERVER_PORT);
    bool isInitialized = m_socket.state() == QAbstractSocket::ConnectingState;

    if (!isInitialized)
    {
        m_clientStatus = CLIENT_STATUS_NOK;
        qDebug() << "Failed to connect to the server\n";
    }

    start();
    return true;

}


void NClient::stop()
{
    QMutexLocker locker(&mMutex);
    mStop=true;
}



void NClient::run()
{

    while(1){

        //mMutex.lock();
        //if(mStop) break;
        //mMutex.unlock();

        sync.lock();
        cond.wait(&sync);
        if(!queue.isEmpty()){
            QByteArray data = queue.takeFirst();
            parseData(data);
        }
        sync.unlock();
    }

}

void NClient::parseData(QByteArray buf)
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

void NClient::packetHandler(QByteArray& packet)
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
        eventStart(packet);
        break;

    case PACKET_TYPE_EVENT_ID:
        //qDebug() << "packetHandler:PACKET_TYPE_EVENT_ID" << "\n";
        eventEventId(packet);
        sendAck();
        break;

    case PACKET_TYPE_ACK:
        //qDebug() << "packetHandler:PACKET_TYPE_ACK" << "\n";
        break;
    }
}

void NClient::eventStart(QByteArray& packet)
{
    TypeStart ps;
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
    qDebug() << "length=" << ps.length << "\n";
    qDebug() << "messageType=" << ps.messageType << "\n";
    qDebug() << "serialNumber=" << ps.serialNumber << "\n";
    qDebug() << "eventGroup=" << eventGroup[ps.eventGroup] << "\n";
    qDebug() << "eventType=" << eventType[ps.eventType] << "\n";
    qDebug() << "eventCode=" << eventCode[ps.eventCode] << "\n";
    qDebug() << "node=" << ps.node << "\n";
    qDebug() << "addressType=" << AddrType[ps.addressType] << "\n";
    qDebug() << "address=" << ps.address << "\n";
    qDebug() << "subAddress=" << ps.subAddress << "\n";
    qDebug() << "loop=" << ps.loop << "\n";
    qDebug() << "zone=" << ps.zone << "\n";
    //qDebug() << "length=" << ps.length << "\n";
    qDebug() << "InputAction=" << InputAction[ps.InputAction] << "\n";
    qDebug() << "timeStamp=" << ps.timeStamp << "\n";
    sendEventAck(ps.serialNumber);
}

void NClient::eventEventId(QByteArray& packet)
{
    int length;
    int messageType;
    int serialNumber;

    QDataStream ds(packet);
    ds >> length
       >> messageType
       >> serialNumber;

    qDebug() << "length=" << length << "\n";
    qDebug() << "messageType=" << messageType << "\n";
    qDebug() << "serialNumber=" << serialNumber << "\n";
    sendAck();
}

void NClient::enQueue(QByteArray req)
{
   sync.lock();
   //qDebug() << "enQueue" << "\n";
   queue.append(req);
   sync.unlock();
   cond.wakeAll();
}

void NClient::onReceive()
{

    QByteArray data = m_socket.readAll();

    enQueue(data);
    //qDebug() << "onReceive:packet len=" << data.length() <<"\n";
    q//Debug() << data.toHex() << "\n";



}


void NClient::sendHeartbeat()
{
    //QByteArray ba(MSG_PACKET_TYPE_HEARTBEAT);
    //int len = sizeof(MSG_PACKET_TYPE_HEARTBEAT);
    //qDebug() << "MSG_PACKET_TYPE_HEARTBEAT len=" << len <<"\n";
    sendPacket(MSG_PACKET_TYPE_HEARTBEAT, sizeof(MSG_PACKET_TYPE_HEARTBEAT)/sizeof(int));

}

void NClient::sendStartConnectionMonitoring()
{
    //QByteArray ba(MSG_PACKET_TYPE_START_CONNECTION_MONITORING);
    //int len = sizeof(MSG_PACKET_TYPE_START_CONNECTION_MONITORING);
    //qDebug() << "MSG_PACKET_TYPE_START_CONNECTION_MONITORING len=" <<len <<"\n";
     sendPacket(MSG_PACKET_TYPE_START_CONNECTION_MONITORING,
                sizeof(MSG_PACKET_TYPE_START_CONNECTION_MONITORING)/sizeof(int));

}

void NClient::sendStopConnectionMonitoring()
{
    //QByteArray ba(MSG_PACKET_TYPE_STOP_CONNECTION_MONITORING);
    //int len = sizeof(MSG_PACKET_TYPE_STOP_CONNECTION_MONITORING);
    //qDebug() << "MSG_PACKET_TYPE_STOP_CONNECTION_MONITORING len=" << len <<"\n";
    sendPacket(MSG_PACKET_TYPE_STOP_CONNECTION_MONITORING,
               sizeof(MSG_PACKET_TYPE_STOP_CONNECTION_MONITORING)/sizeof(int));

}

void NClient::sendAck()
{
    //int len = sizeof(MSG_PACKET_TYPE_ACK);
   // qDebug() << "MSG_PACKET_TYPE_ACK len=" << len <<"\n";
    sendPacket(MSG_PACKET_TYPE_ACK,
               sizeof(MSG_PACKET_TYPE_ACK)/sizeof(int));
}

void NClient::sendEventAck(int serialNum)
{
    //int len = sizeof(MSG_PACKET_TYPE_EVENT_ACK);
   // qDebug() << "MSG_PACKET_TYPE_EVENT_ACK len=" << len <<"\n";
    MSG_PACKET_TYPE_EVENT_ACK[2] = serialNum;
    sendPacket(MSG_PACKET_TYPE_EVENT_ACK,
               sizeof(MSG_PACKET_TYPE_EVENT_ACK)/sizeof(int));

}

void NClient::getCurrentTime()
{
    //int len = sizeof(MSG_PACKET_TYPE_REQUEST_CURRENT_TIME);
    //qDebug() << "PACKET_TYPE_REQUEST_CURRENT_TIME len=" << len <<"\n";
    sendPacket(MSG_PACKET_TYPE_REQUEST_CURRENT_TIME,
               sizeof(MSG_PACKET_TYPE_REQUEST_CURRENT_TIME)/sizeof(int));

}

void NClient::reqActiveEvent()
{
    //int len = sizeof(MSG_PACKET_TYPE_REQUEST_ACTIVE_EVENTS);
    //qDebug() << "MSG_PACKET_TYPE_REQUEST_ACTIVE_EVENTS len=" << len <<"\n";
    sendPacket(MSG_PACKET_TYPE_REQUEST_ACTIVE_EVENTS,
               sizeof(MSG_PACKET_TYPE_REQUEST_ACTIVE_EVENTS)/sizeof(int));

}

void NClient::reqEventLog()
{
    int len = sizeof(MSG_PACKET_TYPE_REQUEST_EVENT_LOG);
    qDebug() << "MSG_PACKET_TYPE_REQUEST_EVENT_LOG len=" << len <<"\n";
    sendPacket(MSG_PACKET_TYPE_REQUEST_EVENT_LOG,
               sizeof(MSG_PACKET_TYPE_REQUEST_EVENT_LOG)/sizeof(int));


}

void NClient::getNodeNum()
{
    //int len = sizeof(MSG_PACKET_TYPE_GET_NODE_NUM);
    //qDebug() << "MSG_PACKET_TYPE_GET_NODE_NUM len=" << len <<"\n";
    sendPacket(MSG_PACKET_TYPE_GET_NODE_NUM,
               sizeof(MSG_PACKET_TYPE_GET_NODE_NUM)/sizeof(int));


}

void NClient::writePacket(int* data, int len)
{
    for(int i=0;i<len;i++){
        int t = *(data+i);
        t = qToBigEndian(t);
        m_socket.write((char*)&t, sizeof(int));
    }
    m_socket.flush();

}

void NClient::sendPacket(int* data, int len)
{
    //qDebug() << "sendPacket:len=" << len << "\n";

    emit pushPacket(data,len);

    //qDebug() << "\n";
}

void NClient::onNewConnection()
{
    m_clientStatus = CLIENT_STATUS_OK;
    qDebug() << "Connected to the server\n";
}



void NClient::onConnectionError()
{
    m_clientStatus = CLIENT_STATUS_NOK;
    qDebug() << "connection error\n";
}

void NClient::onDisconnection()
{
    m_clientStatus = CLIENT_STATUS_DISCONNECTED;
    qDebug() << "Server disconnected\n";
}


