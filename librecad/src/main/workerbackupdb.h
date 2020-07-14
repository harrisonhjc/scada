#ifndef WORKERBACKUPDB_H
#define WORKERBACKUPDB_H
#include <QThread>
#include "sv_database.h"


class workerBackupDb : public QObject
{
    Q_OBJECT

public slots:
    void doWork();

signals:
    void resultReady();
};

#endif // WORKERBACKUPDB_H
