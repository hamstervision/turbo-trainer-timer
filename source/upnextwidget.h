/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef UPNEXTWIDGET_H
#define UPNEXTWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QFont>
#include "step.h"
#include "showtimestepwidget.h"

/*! Represents the up-coming interval when running the turbo set */
class UpNextWidget : public ShowTimeStepWidget
{
    Q_OBJECT
public:
    explicit UpNextWidget(IFontAwesome *fontAwesome, QWidget *parent = nullptr);

    void setInterval(Interval *upNext);

protected: // ShowTimeStepWidget
    void adjustLayout() override;
    int headerSize() override;

protected:
    QLabel      *m_upNext;
    QFont       m_topRowFont;
    QFont       m_textFont;
};

#endif // UPNEXTWIDGET_H
