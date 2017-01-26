#include "showtimestepwidget.h"

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

