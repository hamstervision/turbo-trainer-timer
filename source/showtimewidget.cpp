/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#include "showtimewidget.h"
#include "isetmanager.h"
#include "step.h"
#include "stepresources.h"
#include <QPainter>
#include <QTimer>

static const int TimerInterval  = 1000; // 1 second;
static const int MarginWidth    = 30;

ShowTimeWindow::ShowTimeWindow(QWidget *parent)
    : QWidget(parent, Qt::Window)
{

}

void ShowTimeWindow::setWidget(QWidget *widget)
{
    m_widget = widget;
    adjustLayout();
}

void ShowTimeWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0x20, 0x20, 0x20));
}

void ShowTimeWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    adjustLayout();
}

void ShowTimeWindow::adjustLayout()
{
    if (m_widget)
        m_widget->setGeometry(rect());
}


ShowTimeWidget::ShowTimeWidget(ISetManager *setManager, IFontAwesome *fontAwesome, QWidget *parent)
    : QWidget(parent)
    , m_setManager(setManager)
    , m_fontAwesome(fontAwesome)
    , m_nowPlaying(nullptr)
    , m_upNext(nullptr)
    , m_status(nullptr)
    , m_secsRemaining(0)
    , m_running(false)
    , m_timerId(-1)
{
    Q_ASSERT(setManager && fontAwesome);

    m_nowPlaying = new NowPlayingWidget(m_fontAwesome, this);
    m_nowPlaying->show();

    m_upNext = new UpNextWidget(m_fontAwesome, this);
    m_upNext->show();

    m_status = new QLabel(this);
    m_status->hide();

    setFocusPolicy(Qt::StrongFocus);

    adjustLayout();
}

void ShowTimeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0xff, 0xff, 0xff));
}

void ShowTimeWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        emit closeFullScreen();
        break;
    case Qt::Key_F11:
        emit toggleFullscreen();
        break;
    case Qt::Key_Space:
        emit playPauseToggle();
        break;
    }
}

void ShowTimeWidget::onIntervalStarted()
{
    if (!m_setManager || !m_setManager->currentInterval() || !m_nowPlaying || !m_upNext)
        return;

    if (m_status)
        m_status->hide();

    Interval *current = m_setManager->currentInterval();
    m_nowPlaying->setInterval(current);
    m_secsRemaining = current->duration();
    m_nowPlaying->setTimeRemaining(m_secsRemaining);

    uint32_t currentIteration = 0, totalIterations = 0;
    bool loop = m_setManager->currentIteration(currentIteration, totalIterations);
    m_nowPlaying->setIterations(loop, currentIteration, totalIterations);

    m_upNext->setInterval(m_setManager->nextInterval());
}

void ShowTimeWidget::onSetStarted()
{
    m_running = true;
    m_timerId = startTimer(TimerInterval);
}

void ShowTimeWidget::onSetPaused()
{
    m_running = false;
    killTimer(m_timerId);
    showStatus("-Paused-", false);
}

void ShowTimeWidget::onSetResumed()
{
    m_running = true;
    m_timerId = startTimer(TimerInterval);
    showStatus("-Resumed-", true);
}

void ShowTimeWidget::onSetComplete()
{
    m_running = false;
    if (m_timerId > 0)
    {
        killTimer(m_timerId);
        m_timerId = -1;
    }

    showStatus("Set Complete", false);
}

void ShowTimeWidget::onSetStopped()
{
    m_running = false;
    if (m_timerId > 0)
    {
        killTimer(m_timerId);
        m_timerId = -1;
    }
}

void ShowTimeWidget::onPlaybackError(const QString &error)
{
    m_running = false;
    if (m_timerId > 0)
    {
        killTimer(m_timerId);
        m_timerId = -1;
    }

    showStatus(error, false);
}

void ShowTimeWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    if (m_nowPlaying && (m_secsRemaining > 0))
    {
        --m_secsRemaining;
        m_nowPlaying->setTimeRemaining(m_secsRemaining);
    }
}

void ShowTimeWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    adjustLayout();
}

void ShowTimeWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit toggleFullscreen();
}

void ShowTimeWidget::onHideStatus()
{
    if (!m_status)
    {
        Q_ASSERT(false);
        return;
    }

    m_status->hide();
}

void ShowTimeWidget::adjustLayout()
{
    if (!m_nowPlaying || !m_upNext)
    {
        Q_ASSERT(false);
        return;
    }

    int topHeight = ((height() * 3) / 4) - (MarginWidth * 2);
    m_nowPlaying->setGeometry(MarginWidth, MarginWidth, width() - (MarginWidth * 2), topHeight);
    m_upNext->setGeometry(MarginWidth, topHeight + (MarginWidth * 2), width() - (MarginWidth * 2), height() - topHeight - (MarginWidth * 3));

    if (m_status && !m_status->isHidden())
    {
        float vMargin = height() / 5.0f;
        float hMargin = width() / 5.0f;

        m_status->setGeometry(hMargin, vMargin, width() - (hMargin * 2), height() - (vMargin * 2));
        UpdateLabelFontSize(m_status, false);
    }
}

void ShowTimeWidget::showStatus(const QString &statusMsg, bool autoHide)
{
    if (!m_status)
    {
        Q_ASSERT(false);
        return;
    }

    m_status->show();
    m_status->raise();
    m_status->setText(statusMsg);
    m_status->setStyleSheet("background-color: rgba(200, 200, 200, 100);");
    m_status->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    adjustLayout();

    if (autoHide)
        QTimer::singleShot(1000, this, SLOT(onHideStatus()));
}
