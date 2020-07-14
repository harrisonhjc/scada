#include "sv_label.h"
#include <QPainter>

void SV_Label::paintEvent(QPaintEvent *aEvent)
{
    QLabel::paintEvent(aEvent);
    _displayImage();
}

void SV_Label::setPixmap(QPixmap aPicture)
{
	
    _qpSource = _qpCurrent = aPicture;
    repaint();
}

void SV_Label::_displayImage()
{
    if (_qpSource.isNull())
        return;
	
	QPainter paint(this);
	_qpCurrent = _qpSource.scaled(width(),height());	
    paint.drawPixmap(0, 0, _qpCurrent);

    /*
    if(_text == NULL)
		return;

	QRect rect = QRect(800, 0, width()-800, height());
    paint.drawText(rect ,alignment() ,_text);
    */
	
}

