#ifndef RESPWORKER_H
#define RESPWORKER_H
#include <QObject>
#include <QSerialPort>

class RespWorker : public QObject
{
    Q_OBJECT
public:
	QSerialPort *serial;
    char cmd[2];

	void setSerial(QSerialPort* serial);
		
private slots:
    void onTimeout();
};

#endif // RESPWORKER_H
