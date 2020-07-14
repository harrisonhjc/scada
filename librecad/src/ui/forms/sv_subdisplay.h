#ifndef SV_SUBDISPLAY_H
#define SV_SUBDISPLAY_H

#include <QWidget>

namespace Ui {
class SV_SubDisplay;
}

class SV_SubDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit SV_SubDisplay(QWidget *parent = 0);
    ~SV_SubDisplay();

private:
    Ui::SV_SubDisplay *ui;
};

#endif // SV_SUBDISPLAY_H
