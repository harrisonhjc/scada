#include "sv_displaytime.h"
#include "ui_sv_displaytime.h"
#include <QPainter>
#include <QPaintEvent>

SV_DisplayTime::SV_DisplayTime(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SV_DisplayTime)
{
    ui->setupUi(this);
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAutoFillBackground(true);
}

SV_DisplayTime::~SV_DisplayTime()
{
    delete ui;
}

void SV_DisplayTime::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event); 
	/*
    QPainter painter;
	
	QRect rect = event->rect();
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect,QColor(50,51,46,120));
    painter.end();
*/
}

void SV_DisplayTime::setText(const QString& time)
{
    ui->lb_time->setText(time);

}


