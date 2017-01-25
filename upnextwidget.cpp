#include "upnextwidget.h"
#include "stepresources.h"
#include <QPainter>
#include <QTime>

static const int IconBoxSize = 50;
static const int Margin = 5;

UpNextWidget::UpNextWidget(IFontAwesome *fontAwesome, QWidget *parent)
    : ShowTimeStepWidget(fontAwesome, parent)
    , m_upNext(nullptr)
    , m_topRowFont(FontName, 12, QFont::Bold)
    , m_textFont(FontName, 9, QFont::Bold)

{
    Q_ASSERT(m_topRow && m_topRowLayout);

    m_upNext = new QLabel("Up next:", m_topRow);
    m_upNext->setFont(m_topRowFont);
    m_upNext->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_topRowLayout->addWidget(m_upNext);

    m_icon = new QLabel(m_topRow);
    m_icon->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_topRowLayout->addWidget(m_icon);

    m_type = new QLabel(m_topRow);
    m_type->setFont(m_topRowFont);
    m_type->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_topRowLayout->addWidget(m_type);

    m_time = new QLabel(m_topRow);
    m_time->setFont(m_topRowFont);
    m_time->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    m_topRowLayout->addWidget(m_time);

    m_topRowLayout->setContentsMargins(Margin, Margin, Margin, Margin);

    m_topRow->setLayout(m_topRowLayout);

    m_text = new QLabel(this);
    m_text->setFont(m_textFont);

    adjustLayout();
}

void UpNextWidget::setInterval(Interval *upNext)
{
    m_interval = upNext;

    if (!upNext)
        return;

    m_text->setText(upNext->text());
    m_type->setText(TypeToString(upNext->type()));

    QTime time(0, 0, 0);
    time = time.addSecs(upNext->duration());
    m_time->setText(time.toString("hh:mm:ss"));

    DrawFaIconToLabel(m_icon, TypeToFaIcon(upNext->type()), m_fontAwesome);

    adjustLayout();
    repaint();
}

void UpNextWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (!m_interval)
        return;

    QPainter painter(this);

    painter.setPen(QColor(0x33, 0x33, 0x33));
    painter.setBrush(Qt::NoBrush);

    int headerHeight = headerSize();

    QRect bgrRect(0, 0, width(), headerHeight);

    painter.fillRect(bgrRect, TypeToBgColour(m_interval->type()));
    painter.drawRect(bgrRect);

    bgrRect.setRect(0, headerHeight, width(), height() - headerHeight);

    painter.fillRect(bgrRect, m_bgrColour);
    painter.drawRect(bgrRect);
}

void UpNextWidget::adjustLayout()
{
    if (!m_topRow || !m_text)
    {
        Q_ASSERT(false);
        return;
    }

    int headerHeight = headerSize();

    m_topRow->setGeometry(0, 0, width() , headerHeight);
    m_text->setGeometry(Margin, headerHeight + (Margin * 2), width() - (Margin * 2), height() - headerHeight - (Margin * 2));

    UpdateLabelFontSize(m_upNext);
    UpdateLabelFontSize(m_icon);
    UpdateLabelFontSize(m_type);
    UpdateLabelFontSize(m_time);
    UpdateLabelFontSize(m_text);
}

int UpNextWidget::headerSize()
{
    return height() / 2;
}

