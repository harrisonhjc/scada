#ifndef PACKETTYPE_H
#define PACKETTYPE_H

#include <QMetaType>

typedef struct tagPACKET_TYPE_START
{
    int length;
    int messageType;
    int serialNumber;
    int eventGroup;
    int eventType;
    int eventCode;
    int node;
    int addressType;
    int address;
    int subAddress;
    int loop;
    int zone;
    int InputAction;
    int timeStamp;
    char locationText[80];
    char panelText[32];
    char zoneText[80];
} PACKET_TYPE_START;

typedef struct tagEVENT_START{
    int length;
    int messageType;
    int serialNumber;
    int eventGroup;
    int eventType;
    int eventCode;
    int node;
    int addressType;
    int address;
    int subAddress;
    int loop;
    int zone;
    int InputAction;
    int timeStamp;
} TypeStart;



typedef struct tagEVENT_LOG{
    int length;
    int messageType;
    int serialNumber;
    int eventGroup;
    int eventType;
    int eventCode;
    int node;
    int addressType;
    int address;
    int subAddress;
    int loop;
    int zone;
    int InputAction;
    int timeStamp;
    QByteArray  locationText;
    QByteArray  panelText;
    QByteArray  zoneText;
} TypeMceEventLog;

Q_DECLARE_METATYPE(TypeMceEventLog); 


typedef struct tagCONNECT_STATUS{
    int status;
    
} TypeConnectStatus;
Q_DECLARE_METATYPE(TypeConnectStatus); 
#endif // PACKETTYPE_H
