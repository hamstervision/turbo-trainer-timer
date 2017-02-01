/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#include "stagingarea.h"
#include "stepwidget.h"

#include <QResizeEvent>
#include <QPainter>

StagingArea::StagingArea(TurboSetModel *model, IFontAwesome *fontAwesome, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_fontAwesome(fontAwesome)
{
    Q_ASSERT(model && fontAwesome);
}

void StagingArea::clearView()
{
    for (auto step :  m_steps)
    {
        step->deleteLater();
    }
    m_steps.clear();
}

int StagingArea::idealHeight() const
{
    return m_idealHeight;
}

void StagingArea::onSetChanged()
{
    clearView();

    auto intervals = m_model->getIntervals();
    for (auto step : intervals)
    {
        addStep(step, this);
    }

    adjustLayout();

    emit sizeChanged();
}

bool StagingArea::event(QEvent *event)
{
    if (event->type() == QEvent::LayoutRequest)
    {
        event->accept();
        return true;
    }
    return QWidget::event(event);
}

void StagingArea::resizeEvent(QResizeEvent *event)
{
    m_idealWidth = event->size().width();
    adjustLayout();
}

void StagingArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(QRectF(QPointF(0.0f, 0.0f), size()), QColor(0x80, 0x80, 0x80));
}

void StagingArea::adjustLayout()
{
    static const int Spacing = 10;
    int y = Spacing;
    int width = m_idealWidth - (Spacing * 2);

    for (auto step : m_steps)
    {
        step->setVisible(true);
        step->setGeometry(Spacing, y, width, step->idealHeight());
        y += step->idealHeight() + Spacing;
    }

    m_idealHeight = y;
}

void StagingArea::addStep(Step *step, QWidget *parent)
{
    StepWidget *newStep = nullptr;
    if (step->type() == StepType::Loop)
    {
        newStep = new LoopStepWidget(step, m_fontAwesome, parent);
        size_t children = step->getChildCount();
        for (size_t i = 0; i < children; i++)
        {
            Step *child = step->getChild(i);
            addStep(child, newStep);
        }
    }
    else
    {
        newStep = new IntervalWidget(step, m_fontAwesome, parent);
    }

    if (!newStep)
    {
        Q_ASSERT(false);
        return;
    }

    LoopStepWidget *loopParent = dynamic_cast<LoopStepWidget*>(parent);
    if (loopParent)
    {
        loopParent->addChild(newStep);
        QObject::connect(newStep, SIGNAL(deleted(Step*)), loopParent, SLOT(onStepDeleted(Step*)), Qt::DirectConnection);
        QObject::connect(newStep, SIGNAL(movedUp(Step*)), loopParent, SLOT(onStepMovedUp(Step*)), Qt::DirectConnection);
        QObject::connect(newStep, SIGNAL(movedDown(Step*)), loopParent, SLOT(onStepMovedDown(Step*)), Qt::DirectConnection);
        QObject::connect(newStep, SIGNAL(typeChanged(Step*,StepType)), loopParent, SLOT(onTypeChanged(Step*,StepType)), Qt::DirectConnection);

    }
    else
    {
        m_steps.push_back(newStep);
        QObject::connect(newStep, SIGNAL(deleted(Step*)), m_model, SLOT(onStepDeleted(Step*)), Qt::DirectConnection);
        QObject::connect(newStep, SIGNAL(movedUp(Step*)), m_model, SLOT(onStepMovedUp(Step*)), Qt::DirectConnection);
        QObject::connect(newStep, SIGNAL(movedDown(Step*)), m_model, SLOT(onStepMovedDown(Step*)), Qt::DirectConnection);
        QObject::connect(newStep, SIGNAL(typeChanged(Step*,StepType)), m_model, SLOT(onTypeChanged(Step*,StepType)), Qt::DirectConnection);
    }

}
