#ifndef UPNEXTWIDGET_H
#define UPNEXTWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QFont>
#include "step.h"
#include "showtimestepwidget.h"

class UpNextWidget : public ShowTimeStepWidget
{
    Q_OBJECT
public:
    explicit UpNextWidget(IFontAwesome *fontAwesome, QWidget *parent = nullptr);
    void setInterval(Interval *upNext);

protected slots:
    void paintEvent(QPaintEvent *event) override;

protected:
    void adjustLayout() override;
    int headerSize() override;

protected:
    QLabel      *m_upNext;
    QFont       m_topRowFont;
    QFont       m_textFont;
};

#endif // UPNEXTWIDGET_H
