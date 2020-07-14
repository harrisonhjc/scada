#ifndef PLAYSOUNDTHREAD_H
#define PLAYSOUNDTHREAD_H

#include <QThread>

class playSoundThread : public QThread
{
    Q_OBJECT
	void run();
	
public:
    explicit playSoundThread(QObject *parent = 0);

signals:

public slots:

};

#endif // PLAYSOUNDTHREAD_H
