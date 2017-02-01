/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#include "nowplayingwidget.h"
#include "stepresources.h"
#include <QPainter>
#include <QTime>
#include <QFontMetrics>

static const int IconBoxSize = 50;
static const int Margin = 5;
static const int TimeMargin = 15;

NowPlayingWidget::NowPlayingWidget(IFontAwesome *fontAwesome, QWidget *parent)
    : ShowTimeStepWidget(fontAwesome, parent)
    , m_iteration(nullptr)
    , m_typeFont(FontName, 18, QFont::Bold)
    , m_textFont(FontName, 12, QFont::Bold)
    , m_timeFont(FontName, 18, QFont::Bold)
{
    Q_ASSERT(m_topRow && m_topRowLayout);

    m_icon = new QLabel(m_topRow);
    m_icon->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_topRowLayout->addWidget(m_icon);

    m_type = new QLabel(m_topRow);
    m_type->setFont(m_typeFont);
    m_type->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_topRowLayout->addWidget(m_type);

    m_iteration = new QLabel(m_topRow);
    m_iteration->setFont(m_typeFont);
    m_iteration->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    m_topRowLayout->addWidget(m_iteration);

    m_time = new QLabel(this);
    m_time->setFont(m_timeFont);
    m_time->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_time->setStyleSheet("color: #a9da88");

    m_text = new QLabel(this);
    m_text->setFont(m_textFont);
    m_text->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    m_text->setWordWrap(true);

    m_topRow->setLayout(m_topRowLayout);

    adjustLayout();
}

void NowPlayingWidget::setInterval(Interval *nowPlaying)
{
    m_interval = nowPlaying;
    m_text->setText(nowPlaying->text());
    m_type->setText(TypeToString(nowPlaying->type()));

    adjustLayout();
    repaint();
}

void NowPlayingWidget::setIterations(const bool loop, const uint32_t currentIteration, const uint32_t totalIterations)
{
    if (!m_iteration)
    {
        Q_ASSERT(false);
        return;
    }

    if (loop)
    {
        QString text = QString("Loop %1/%2").arg(QString::number(currentIteration), QString::number(totalIterations));
        m_iteration->setText(text);
        m_iteration->show();
    }
    else
    {
        m_iteration->setText(QString());
        m_iteration->hide();
    }

    adjustLayout();
    repaint();
}

void NowPlayingWidget::setTimeRemaining(const uint secondsRemaining)
{
    QTime time(0, 0, 0);
    time = time.addSecs(secondsRemaining);
    m_time->setText(time.toString("hh:mm:ss"));
    m_time->repaint();
}

void NowPlayingWidget::adjustLayout()
{
    if (!m_topRow || !m_time || !m_text)
    {
        Q_ASSERT(false);
        return;
    }

    int headerHeight = headerSize();

    m_topRow->setGeometry(0, 0, width(), headerHeight);

    // The following *should* be unneccessary but contents of the layout have not yet been updated at this point (async?)
    m_topRowLayout->setGeometry(m_topRow->rect());

    m_time->setGeometry(Margin, headerHeight, width() - (Margin * 2), ((height() - headerHeight) * 2) / 3);
    m_text->setGeometry(Margin, m_time->geometry().bottom() + TimeMargin, width() - (Margin * 2), height() - m_time->geometry().bottom() - (TimeMargin * 2));

    if (m_interval)
        DrawFaIconToLabel(m_icon, TypeToFaIcon(m_interval->type()), m_fontAwesome);

    UpdateLabelFontSize(m_type);
    if (!m_iteration->isHidden())
        UpdateLabelFontSize(m_iteration);
    UpdateLabelFontSize(m_time);
    UpdateLabelFontSize(m_text);
}

int NowPlayingWidget::headerSize()
{
    return height() / 5;
}
