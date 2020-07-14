#ifndef SV_LABEL_H
#define SV_LABEL_H

#include <QLabel>

class SV_Label : public QLabel
{
    Q_OBJECT
public:
     SV_Label(QWidget *aParent) : QLabel(aParent) { }
	 void setPixmap(QPixmap aPicture);
     void paintEvent(QPaintEvent *aEvent);
     void setText(QString& text){ _text = text;};

private:
	QPixmap _qpSource; //preserve the original, so multiple resize events won't break the quality
    QPixmap _qpCurrent;
    void _displayImage();
	QString _text;
	
signals:

public slots:

};

#endif // SV_LABEL_H
