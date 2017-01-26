#include "showtimewidget.h"
#include "isetmanager.h"
#include "step.h"
#include <QPainter>

static const int TimerInterval  = 1000; // 1 second;
static const int MarginWidth    = 30;

ShowTimeWindow::ShowTimeWindow(QWidget *parent)
    : QWidget(parent, Qt::Window)
{

}

void ShowTimeWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0x20, 0x20, 0x20));
}


ShowTimeWidget::ShowTimeWidget(ISetManager *setManager, IFontAwesome *fontAwesome, QWidget *parent)
    : QWidget(parent)
    , m_setManager(setManager)
    , m_fontAwesome(fontAwesome)
    , m_nowPlaying(nullptr)
    , m_upNext(nullptr)
    , m_secsRemaining(0)
    , m_running(false)
    , m_timerId(-1)
{
    Q_ASSERT(setManager && fontAwesome);

    m_nowPlaying = new NowPlayingWidget(m_fontAwesome, this);
    m_nowPlaying->show();

    m_upNext = new UpNextWidget(m_fontAwesome, this);
    m_upNext->show();

    adjustLayout();
}

void ShowTimeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0xff, 0xff, 0xff));
}

void ShowTimeWidget::onIntervalStarted()
{
    if (!m_setManager || !m_setManager->currentInterval() || !m_nowPlaying || !m_upNext)
        return;

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
    //m_description->setText("-Paused-");
}

void ShowTimeWidget::onSetResumed()
{
    m_running = true;
    m_timerId = startTimer(TimerInterval);
    //m_description->setText(m_descriptionText);
}

void ShowTimeWidget::onSetComplete()
{
    m_running = false;
    if (m_timerId > 0)
    {
        killTimer(m_timerId);
        m_timerId = -1;
    }

    //m_description->setText("Set Complete");
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

    //m_description->setText(error);
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

void ShowTimeWidget::adjustLayout()
{
    int topHeight = ((height() * 3) / 4) - (MarginWidth * 2);
    m_nowPlaying->setGeometry(MarginWidth, MarginWidth, width() - (MarginWidth * 2), topHeight);
    m_upNext->setGeometry(MarginWidth, topHeight + (MarginWidth * 2), width() - (MarginWidth * 2), height() - topHeight - (MarginWidth * 3));
}
