#include "sv_progressbar.h"
#include "ui_sv_progressbar.h"

SV_ProgressBar::SV_ProgressBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SV_ProgressBar)
{
    ui->setupUi(this);

    ui->label->setText("載入樓層圖檔,請稍後 ...");
    
    setWindowFlags(Qt::WindowStaysOnTopHint);
}

SV_ProgressBar::~SV_ProgressBar()
{
    delete ui;
}

void SV_ProgressBar::setProgress(int val)
{
    ui->progressBar->setValue(val);
}

void SV_ProgressBar::setRange(int min, int max)
{
    ui->progressBar->setRange(min,max);
}

