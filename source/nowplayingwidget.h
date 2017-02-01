/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef NOWPLAYINGWIDGET_H
#define NOWPLAYINGWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QLabel>
#include <QFont>
#include <QPixmap>
#include "step.h"
#include "showtimestepwidget.h"

/*! Widget representing the current interval in progress */
class NowPlayingWidget : public ShowTimeStepWidget
{
    Q_OBJECT
public:
    explicit NowPlayingWidget(IFontAwesome *fontAwesome, QWidget *parent = nullptr);

    void setInterval(Interval *nowPlaying);
    void setIterations(const bool loop, const uint32_t currentIteration, const uint32_t totalIterations);
    void setTimeRemaining(const uint secondsRemaining);

protected: // ShowTimeStepWidget
    void adjustLayout() override;
    int headerSize() override;

protected:
    QLabel      *m_iteration;
    QFont       m_typeFont;
    QFont       m_textFont;
    QFont       m_timeFont;
};

#endif // NOWPLAYINGWIDGET_H
