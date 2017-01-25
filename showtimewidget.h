#ifndef SHOWTIMEWIDGET_H
#define SHOWTIMEWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QTime>
#include <QLabel>
#include <QResizeEvent>
#include "nowplayingwidget.h"
#include "upnextwidget.h"
#include "ifontawesome.h"

class ISetManager;

class ShowTimeWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ShowTimeWindow(QWidget *parent = nullptr);

signals:

public slots:
    void paintEvent(QPaintEvent *event) override;
};

class ShowTimeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowTimeWidget(ISetManager *setManager, IFontAwesome *fontAwesome, QWidget *parent = nullptr);

signals:

public slots:
    void paintEvent(QPaintEvent *event) override;
    void onSetStarted();
    void onIntervalStarted();
    void onSetPaused();
    void onSetResumed();
    void onSetComplete();
    void onSetStopped();
    void onPlaybackError(const QString &error);

protected slots:
    void timerEvent(QTimerEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

protected:
    void adjustLayout();

protected:
    ISetManager         *m_setManager;
    IFontAwesome        *m_fontAwesome;
    NowPlayingWidget    *m_nowPlaying;
    UpNextWidget        *m_upNext;
    uint                m_secsRemaining;
    bool                m_running;
    int                 m_timerId;
};

#endif // SHOWTIMEWIDGET_H
