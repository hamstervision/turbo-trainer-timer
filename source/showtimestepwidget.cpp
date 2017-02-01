/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#include "showtimestepwidget.h"
#include "stepresources.h"

ShowTimeStepWidget::ShowTimeStepWidget(IFontAwesome *fontAwesome, QWidget *parent)
    : QWidget(parent)
    , m_fontAwesome(fontAwesome)
    , m_interval(nullptr)
    , m_topRow(nullptr)
    , m_topRowLayout(nullptr)
    , m_icon(nullptr)
    , m_type(nullptr)
    , m_time(nullptr)
    , m_text(nullptr)
    , m_bgrColour(0xa0, 0xa0, 0xa0)
{
    Q_ASSERT(fontAwesome);

    m_topRow = new QWidget(this);
    m_topRowLayout = new QHBoxLayout(m_topRow);
}

void ShowTimeStepWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    adjustLayout();
}

void ShowTimeStepWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (!m_interval)
        return;

    QPainter painter(this);

    painter.setPen(QColor(0x33, 0x33, 0x33));
    painter.setBrush(Qt::NoBrush);

    int headerHeight = headerSize();

    QRect bgrRect(0, 0, width() - 1, headerHeight);

    painter.fillRect(bgrRect, TypeToBgColour(m_interval->type()));
    painter.drawRect(bgrRect);

    bgrRect.setRect(0, headerHeight, width() - 1, height() - headerHeight - 1);

    painter.fillRect(bgrRect, m_bgrColour);
    painter.drawRect(bgrRect);
}

