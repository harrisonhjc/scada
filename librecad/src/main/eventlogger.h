#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE


class EventLogger : public QMainWindow
{
    Q_OBJECT

public:
    void addLog(QString& txt);
    EventLogger(QWidget *parent = 0);

public slots:
    void about();
    void newFile();
    void openFile(const QString &path = QString());

private:
    void setupEditor();
    void setupFileMenu();
    void setupHelpMenu();

    QTextEdit *editor;
    
};


#endif 
