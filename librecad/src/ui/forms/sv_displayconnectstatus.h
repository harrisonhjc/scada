#ifndef SV_DISPLAYCONNECTSTATUS_H
#define SV_DISPLAYCONNECTSTATUS_H

#include <QWidget>

namespace Ui {
class SV_DisplayConnectStatus;
}

class SV_DisplayConnectStatus : public QWidget
{
    Q_OBJECT

public:
    explicit SV_DisplayConnectStatus(QWidget *parent = 0);
    ~SV_DisplayConnectStatus();
	void setText(const QString& );
	
protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::SV_DisplayConnectStatus *ui;
};

#endif // SV_DISPLAYCONNECTSTATUS_H
