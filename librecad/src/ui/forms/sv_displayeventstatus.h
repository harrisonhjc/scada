#ifndef SV_DISPLAYEVENTSTATUS_H
#define SV_DISPLAYEVENTSTATUS_H

#include <QWidget>

namespace Ui {
class SV_DisplayEventStatus;
}

class SV_DisplayEventStatus : public QWidget
{
    Q_OBJECT

public:
    explicit SV_DisplayEventStatus(QWidget *parent = 0);
    ~SV_DisplayEventStatus();
	void setRedMsg(const QString& text);
	void setMagentaMsg(const QString& text);
	void setGreenMsg(const QString& text);
	void setBlueMsg(const QString& text);
	void setYellowMsg(const QString& text);

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::SV_DisplayEventStatus *ui;
};

#endif // SV_DISPLAYEVENTSTATUS_H
