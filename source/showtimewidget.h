/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef SHOWTIMEWIDGET_H
#define SHOWTIMEWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QTime>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include "nowplayingwidget.h"
#include "upnextwidget.h"
#include "ifontawesome.h"

class ISetManager;

/*! Full-screen window container */
class ShowTimeWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ShowTimeWindow(QWidget *parent = nullptr);

    void setWidget(QWidget *widget);

public slots:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

protected:
    void adjustLayout();

protected:
    QWidget *m_widget;
};

/*! Widget used for displaying the set when it's running */
class ShowTimeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowTimeWidget(ISetManager *setManager, IFontAwesome *fontAwesome, QWidget *parent = nullptr);

signals:
    void toggleFullscreen();
    void closeFullScreen();
    void playPauseToggle();

public slots:
    void onSetStarted();
    void onIntervalStarted();
    void onSetPaused();
    void onSetResumed();
    void onSetComplete();
    void onSetStopped();
    void onPlaybackError(const QString &error);

protected: // Event handlers
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

protected slots:
    void onHideStatus();

protected:
    void adjustLayout();
    void showStatus(const QString &statusMsg, bool autoHide);

protected:
    ISetManager         *m_setManager;
    IFontAwesome        *m_fontAwesome;
    NowPlayingWidget    *m_nowPlaying;
    UpNextWidget        *m_upNext;
    QLabel              *m_status;
    uint                m_secsRemaining;
    bool                m_running;
    int                 m_timerId;
};

#endif // SHOWTIMEWIDGET_H
