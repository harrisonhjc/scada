#ifndef SV_DISPLAYTIME_H
#define SV_DISPLAYTIME_H

#include <QWidget>

namespace Ui {
class SV_DisplayTime;
}

class SV_DisplayTime : public QWidget
{
    Q_OBJECT

public:
    explicit SV_DisplayTime(QWidget *parent = 0);
    ~SV_DisplayTime();
	void setText(const QString& );

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::SV_DisplayTime *ui;
};

#endif // SV_DISPLAYTIME_H
