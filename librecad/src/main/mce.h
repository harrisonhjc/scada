#ifndef MCE_H
#define MCE_H

#include <QtGlobal>

const int MCE_CLIENT_ID = 0x00000003;

const int PACKET_TYPE_NACK = 0x00000000;
const int PACKET_TYPE_ACK = 0x00000001;
const int PACKET_TYPE_EVENT_ACK = 0x00000086;
const int PACKET_TYPE_EVENT_START = 0x00000085;
const int PACKET_TYPE_EVENT_CLEAR = 0x00000087;
const int PACKET_TYPE_START_CONNECTION_MONITORING = 231;
const int PACKET_TYPE_STOP_CONNECTION_MONITORING = 244;
const int PACKET_TYPE_HEARTBEAT = 86;
const int PACKET_TYPE_REQUEST_CURRENT_TIME = 0x000000b8;
const int PACKET_TYPE_REQUEST_ACTIVE_EVENTS = 0x0000004f;
const int PACKET_TYPE_REQUEST_EVENT_LOG = 0x0000004e;
const int PACKET_TYPE_REQUEST_ACTIVE_EVENT = 0x0000004f;
const int PACKET_TYPE_GET_NODE_NUM = 0x0000006b;
const int PACKET_TYPE_EVENT_ID = 0x00000002;
const int PACKET_TYPE_DEV_ISOLATE_STATUS = 0x00000059;
const int PACKET_TYPE_CONTROL_RESET = 0x0000004d;
const int PACKET_TYPE_CONTROL_SILENCE = 0x0000004c;
const int PACKET_TYPE_SET_TIME = 0x00000058;
const int PACKET_TYPE_CONTROL = 0x000000c5;


int MSG_PACKET_TYPE_ACK[] = {8,
                             PACKET_TYPE_ACK & 0x000000FF};
int MSG_PACKET_TYPE_EVENT_ACK[3] = {12,
                             PACKET_TYPE_EVENT_ACK & 0x000000FF,
                             1};

int MSG_PACKET_TYPE_START_CONNECTION_MONITORING[] = {12,
                                                     PACKET_TYPE_START_CONNECTION_MONITORING & 0x000000FF,
                                                     MCE_CLIENT_ID};
int MSG_PACKET_TYPE_STOP_CONNECTION_MONITORING[] = {12,
                                                    PACKET_TYPE_STOP_CONNECTION_MONITORING & 0x000000FF,
                                                    MCE_CLIENT_ID};
int MSG_PACKET_TYPE_HEARTBEAT[] = {12,
                                   PACKET_TYPE_HEARTBEAT & 0x000000FF,
                                   MCE_CLIENT_ID};

int MSG_PACKET_TYPE_REQUEST_CURRENT_TIME[] = {8,
                                   PACKET_TYPE_REQUEST_CURRENT_TIME & 0x000000FF};

int MSG_PACKET_TYPE_REQUEST_ACTIVE_EVENTS[] = {8,
                                   PACKET_TYPE_REQUEST_ACTIVE_EVENTS & 0x000000FF};

int MSG_PACKET_TYPE_REQUEST_EVENT_LOG[] = {12,
                                           PACKET_TYPE_REQUEST_EVENT_LOG & 0x000000FF,
                                           0x00000000};

int MSG_PACKET_TYPE_GET_NODE_NUM[] = {12,
                                           PACKET_TYPE_GET_NODE_NUM & 0x000000FF};

int MSG_PACKET_TYPE_REQUEST_RESET[] = {12,
                                       PACKET_TYPE_CONTROL_RESET & 0x000000FF,
                                       0x00000000};

int MSG_PACKET_TYPE_REQUEST_SILENCE[] = {12,
                                       PACKET_TYPE_CONTROL_SILENCE & 0x000000FF,
                                       0x00000000};

int MSG_PACKET_TYPE_SET_TIME[] = {12,
                                  PACKET_TYPE_SET_TIME & 0x000000FF,
                                  0x00000000};

int MSG_PACKET_TYPE_CONTROL[] = {32,
                                PACKET_TYPE_CONTROL & 0x000000FF,
                                0x00000000, //state 0=device off
                                0x00000000,
                                0x00000000,
                                0x00000000,
                                0x00000000,
                                0x00000000 };                              


#endif // MCE_H
