#include "sv_displayeventstatus.h"
#include "ui_sv_displayeventstatus.h"
#include <QPainter>
#include <QPaintEvent>

SV_DisplayEventStatus::SV_DisplayEventStatus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SV_DisplayEventStatus)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(true);
}

SV_DisplayEventStatus::~SV_DisplayEventStatus()
{
    delete ui;
}

void SV_DisplayEventStatus::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event); 
/*
	QPainter painter;
	QRect rect = event->rect();
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect,QColor(0,0,0,120));
    painter.end();
*/
}

void SV_DisplayEventStatus::setRedMsg(const QString& text)
{
    ui->lb1->setText(text);
}
void SV_DisplayEventStatus::setMagentaMsg(const QString& text)
{
    ui->lb2->setText(text);
}
void SV_DisplayEventStatus::setGreenMsg(const QString& text)
{
    ui->lb3->setText(text);
}
void SV_DisplayEventStatus::setBlueMsg(const QString& text)
{
    ui->lb4->setText(text);
}
void SV_DisplayEventStatus::setYellowMsg(const QString& text)
{
    ui->lb5->setText(text);
}



