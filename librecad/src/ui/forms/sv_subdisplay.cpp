#include "sv_subdisplay.h"
#include "ui_sv_subdisplay.h"

SV_SubDisplay::SV_SubDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SV_SubDisplay)
{
    ui->setupUi(this);
}

SV_SubDisplay::~SV_SubDisplay()
{
    delete ui;
}
