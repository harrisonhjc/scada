/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2011 Rallaz (rallazz@gmail.com)
**
**
** This file is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** This copyright notice MUST APPEAR in all copies of the script!  
**
**********************************************************************/
#include "qg_dlgimage.h"

#include <QDoubleValidator>
#include "rs_image.h"
#include "rs_graphic.h"
#include <QMessageBox>
/*
 *  Constructs a QG_DlgImage as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
QG_DlgImage::QG_DlgImage(QWidget* parent, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setModal(modal);
    setupUi(this);
    setWindowTitle("感測器地址");
    qDebug() << "QG_DlgImage++++++++++++++++++++++";
    /*
    connect(leNode, SIGNAL(textChanged(const QString &)), this, SLOT(nodeInput(const QString &)));
    connect(leZone, SIGNAL(textChanged(const QString &)), this, SLOT(zoneInput(const QString &)));
    connect(leLoop, SIGNAL(textChanged(const QString &)), this, SLOT(loopInput(const QString &)));
    connect(leAddr1, SIGNAL(textChanged(const QString &)), this, SLOT(addr1Input(const QString &)));
    */
}
/*
void QG_DlgImage::nodeInput(const QString &)
{
   if(leNode->hasAcceptableInput() && leNode->text().length() == 2)
        leZone->setFocus();
}
void QG_DlgImage::zoneInput(const QString &)
{
    if(leZone->hasAcceptableInput() && leZone->text().length() == 4)
        leLoop->setFocus();
}
void QG_DlgImage::loopInput(const QString &)
{
    if(leLoop->hasAcceptableInput() && leLoop->text().length() == 2)
        leAddr1->setFocus();
}
void QG_DlgImage::addr1Input(const QString &)
{
    if(leAddr1->hasAcceptableInput() && leAddr1->text().length() == 3)
        leAddr2->setFocus();
}
*/

/*
 *  Destroys the object and frees any allocated resources
 */
QG_DlgImage::~QG_DlgImage()
{
    delete val;

    // no need to delete child widgets, Qt does it all for us
}

void QG_DlgImage::closeEvent(QCloseEvent *bar)
{
    qDebug() << "QG_DlgImage::closeEvent !!!";
    bar->accept();
}
/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QG_DlgImage::languageChange()
{
    retranslateUi(this);
}

void QG_DlgImage::setImage(RS_Image& e) {
    image = &e;
    val = new QDoubleValidator(leScale);
	
    RS_Graphic* graphic = image->getGraphic();
    leScale->setValidator(val);
    scale = image->getUVector().magnitude();
    leScale->setText(QString("%1").arg(scale));
    /*
    QValidator *val1 = new QIntValidator(0,9,this);
    QValidator *val2 = new QIntValidator(0,99,this);
    QValidator *val3 = new QIntValidator(0,999,this);
    QValidator *val4 = new QIntValidator(0,9999,this);
    leNode->setValidator(val2);
    leZone->setValidator(val4);
    leLoop->setValidator(val2);
    leAddr1->setValidator(val3);
    leAddr2->setValidator(val1);
    */
    QString rtuAddress = image->getRtuAddress();
    /*
    leNode->setText(rtuAddress.mid(0,2));
    leZone->setText(rtuAddress.mid(2,4));
    leLoop->setText(rtuAddress.mid(6,2));
    leAddr1->setText(rtuAddress.mid(8,3));
    leAddr2->setText(rtuAddress.mid(11));
    */
    leAddr->setText(rtuAddress);
    leLocation->setText(image->getLocation());
}


void QG_DlgImage::changeWidth() {
	/*
    double width = leWidth->text().toDouble();
    scale = width / image->getWidth();
    leHeight->setText(QString("%1").arg(image->getHeight() * scale));
    leScale->setText(QString("%1").arg(scale));
    */
}
void QG_DlgImage::changeHeight() {
	/*
    double height = leHeight->text().toDouble();
    scale = height / image->getHeight();
    leWidth->setText(QString("%1").arg(image->getWidth() * scale));
    leScale->setText(QString("%1").arg(scale));
    */
}
void QG_DlgImage::changeScale() {
	
    scale = leScale->text().toDouble();
    //leWidth->setText(QString("%1").arg(image->getWidth() * scale));
    //leHeight->setText(QString("%1").arg(image->getHeight() * scale));
    //leDPI->setText(QString("%1").arg(RS_Units::scaleToDpi(scale, image->getGraphicUnit())));
}

void QG_DlgImage::changeDPI(){
	/*
    scale = RS_Units::dpiToScale(leDPI->text().toDouble(), image->getGraphicUnit());
    leScale->setText(QString("%1").arg(scale));
    leWidth->setText(QString("%1").arg(image->getWidth() * scale));
    leHeight->setText(QString("%1").arg(image->getHeight() * scale));    
    */
    //id = leDPI->text().toInt();
}

void QG_DlgImage::updateImage() {
    //image->setPen(wPen->getPen());
    //image->setLayer(cbLayer->currentText());
    //image->setInsertionPoint(RS_Vector(leInsertX->text().toDouble(), leInsertY->text().toDouble()) );
    double orgScale = image->getUVector().magnitude();
    scale /= orgScale;
    //double orgAngle = image->getUVector().angle();
    //double angle = RS_Math::deg2rad( leAngle->text().toDouble() );
    image->scale(image->getInsertionPoint(), RS_Vector(scale, scale));
    //image->rotate(image->getInsertionPoint(), angle - orgAngle);
	//id = leDPI->text().toInt();
    image->setLocation(leLocation->text());
    QString rtuAddress = leAddr->text();
    image->setRtuAddress(rtuAddress);
    image->setFade(0);
    image->update();
}




void QG_DlgImage::on_ok_clicked()
{
    if(leAddr->text().length() != 12){
        QMessageBox::information(this, "Info", "地址長度需為12位數", QMessageBox::Ok);
        return;
    }
    accept();
}

void QG_DlgImage::on_cancel_clicked()
{
    reject();
}
