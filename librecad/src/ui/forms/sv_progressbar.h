#ifndef SV_PROGRESSBAR_H
#define SV_PROGRESSBAR_H

#include <QWidget>

namespace Ui {
class SV_ProgressBar;
}

class SV_ProgressBar : public QWidget
{
    Q_OBJECT

public:
    explicit SV_ProgressBar(QWidget *parent = 0);
    ~SV_ProgressBar();
	void setProgress(int val);
	void setRange(int min, int max);

private:
    Ui::SV_ProgressBar *ui;
};

#endif // SV_PROGRESSBAR_H
