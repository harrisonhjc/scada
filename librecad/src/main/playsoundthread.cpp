#include "playsoundthread.h"
#include <QMediaPlayer>
#include <QDir>
#include <QSound>

playSoundThread::playSoundThread(QObject *parent) :
    QThread(parent)
{
}

void playSoundThread::run()
{
	
	QMediaPlayer *player = new QMediaPlayer;
	QString fn = QDir::currentPath() + "/" + "onfire.wav";
	player->setMedia(QUrl::fromLocalFile(fn));
	player->setVolume(90);
	player->play();
	
	
    QSound::play("C:\\Program Files\\SCADA\\onfire.wav");
}

	
	
