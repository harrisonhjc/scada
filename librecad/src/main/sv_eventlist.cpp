#include "sv_eventlist.h"
#include <QVBoxLayout>

sv_eventlist::sv_eventlist(QWidget *parent) :
    QWidget(parent)
{
	setStyleSheet("background: rgba(15,58,67);border-radius: 16px;");
	itemHeight = 60;
	
}


void sv_eventlist::paintEvent(QPaintEvent *){
    QPainter painter(this);

    QRect rect = contentsRect();

    int y = 0;
    for(int i=0;i< 40;i++){
        y = i*itemHeight;
        if(i%2){
            painter.fillRect(rect.left(), y, width(), itemHeight, QColor(15,58,67));
        }else{
            painter.fillRect(rect.left(), y, width(), itemHeight, QColor(12,46,55));
        }

    }
}

