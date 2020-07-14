#include "respworker.h"
#include <QDebug>

void RespWorker::setSerial(QSerialPort* serial)
{
		this->serial = serial;
        cmd[0] = 0x04;
        cmd[1] = 0x00;
};

void RespWorker::onTimeout(){
        //qDebug()<<"RespWorker::onTimeout get called from?: "<<QThread::currentThreadId();
		//QByteArray cmd = QByteArray(1,EOT);
        serial->write(cmd);
    }

