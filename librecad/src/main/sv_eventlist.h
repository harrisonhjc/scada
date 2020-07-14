#ifndef SV_EVENTLIST_H
#define SV_EVENTLIST_H

#include <QWidget>
#include <QPainter>
#include <QListWidget>

class sv_eventlist : public QWidget
{
    Q_OBJECT
public:
    explicit sv_eventlist(QWidget *parent = 0);
   

protected:
    void paintEvent(QPaintEvent *);

private:
	int itemHeight;
   

signals:

public slots:

};

#endif // SV_EVENTLIST_H
