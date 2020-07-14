#include "sv_displayconnectstatus.h"
#include "ui_sv_displayconnectstatus.h"
#include <QPainter>
#include <QPaintEvent>

SV_DisplayConnectStatus::SV_DisplayConnectStatus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SV_DisplayConnectStatus)
{
    ui->setupUi(this);
    ui->lb_status->setText("未連線");
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
	setAutoFillBackground(true);
}

SV_DisplayConnectStatus::~SV_DisplayConnectStatus()
{
    delete ui;
}

void SV_DisplayConnectStatus::setText(const QString& text)
{
    ui->lb_status->setText(text);

}

void SV_DisplayConnectStatus::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event); 
	/*
    QPainter painter;
	
	QRect rect = event->rect();
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect,QColor(0,0,0,150));
    painter.end();
    */
}

