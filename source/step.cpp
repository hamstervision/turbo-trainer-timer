#include "step.h"
#include "turbosetmodel.h"

void Interval::serialise(QDomDocument &file, QDomElement &parent) const
{
    QDomElement el = file.createElement(TurboSetModel::IntervalTag);
    el.setAttribute(TurboSetModel::TypeAttr, (uint)type());
    el.setAttribute(TurboSetModel::DurationAttr, duration());
    el.setAttribute(TurboSetModel::TextAttr, text());
    parent.appendChild(el);
}

Interval *Interval::nextInterval()
{
    if (m_played)
        return nullptr;

    m_played = true;
    return this;
}

Interval *Interval::queryNext(bool reset /*= false*/)
{
    if (m_played && !reset)
        return nullptr;

    return this;
}

Interval *Interval::fromXml(QDomElement &node, IStepManager *manager)
{
    if (node.tagName().compare(TurboSetModel::IntervalTag, Qt::CaseInsensitive) != 0)
    {
        Q_ASSERT(false);
        return nullptr;
    }

    if (node.attributeNode(TurboSetModel::TypeAttr).isNull())
    {
        Q_ASSERT(false);
        return nullptr;
    }

    StepType type = (StepType)node.attribute(TurboSetModel::TypeAttr).toUInt();
    Interval *interval = new Interval(type, manager);

    if (!interval || !interval->populateFromXml(node))
    {
        delete interval;
        return nullptr;
    }

    return interval;
}

bool Interval::populateFromXml(QDomElement &node)
{
    if (node.attributeNode(TurboSetModel::DurationAttr).isNull()
            || node.attributeNode(TurboSetModel::TextAttr).isNull())
    {
        Q_ASSERT(false);
        return false;
    }
    m_duration = node.attribute(TurboSetModel::DurationAttr).toUInt();
    m_text = node.attribute(TurboSetModel::TextAttr);

    return true;
}

LoopStep::LoopStep(IStepManager *manager)
    : Step(StepType::Loop, manager)
    , m_iterations(0)
    , m_playbackIteration(0)
    , m_playbackStep(nullptr)
{

}

LoopStep::~LoopStep()
{
    for (auto * step : m_children)
    {
        delete step;
    }
    m_children.clear();
}

unsigned int LoopStep::duration() const
{
    unsigned int duration = 0;
    for (auto child : m_children)
    {
        duration += child->duration();
    }
    return duration;
}

void LoopStep::addChild(Step *child)
{
    m_children.push_back(child);
    notifyChange();
}

Step *LoopStep::getChild(size_t index) const
{
    if (index >= m_children.size())
    {
        Q_ASSERT(false);
        return nullptr;
    }
    return m_children.at(index);
}

void LoopStep::serialise(QDomDocument &file, QDomElement &parent) const
{
    QDomElement el = file.createElement(TurboSetModel::LoopTag);
    el.setAttribute(TurboSetModel::IterationsAttr, m_iterations);

    for (auto child : m_children)
    {
        child->serialise(file, el);
    }

    parent.appendChild(el);
}

Interval *LoopStep::nextInterval()
{
    if (m_children.empty())
        return nullptr;

    if (!m_playbackStep)
        m_playbackStep = m_children.front();

    Interval *next = m_playbackStep->nextInterval();
    if (next)
        return next;

    auto it = m_children.begin();
    while (it != m_children.end())
    {
        if (*it == m_playbackStep)
            break;

        ++it;
    }

    if (it == m_children.end() || ++it == m_children.end())
    {
        ++m_playbackIteration;
        if (m_playbackIteration < m_iterations)
        {
            m_playbackStep = nullptr;
            for (auto child : m_children)
            {
                child->resetPlayback();
            }
            return nextInterval();
        }
        return false;
    }

    m_playbackStep = *it;
    return m_playbackStep->nextInterval();
}

Interval *LoopStep::queryNext(bool reset /*= false*/)
{
    if (m_children.empty())
        return nullptr;

    if (!m_playbackStep)
        return m_children.front()->queryNext(reset);

    Interval *next = m_playbackStep->queryNext(reset);
    if (next)
        return next;

    auto it = m_children.begin();
    while (it != m_children.end())
    {
        if (*it == m_playbackStep)
            break;

        ++it;
    }

    if (it == m_children.end() || ++it == m_children.end())
    {
        if (m_playbackIteration < m_iterations)
            return m_children.front()->queryNext(true);

        return nullptr;
    }

    return (*it)->queryNext();
}

void LoopStep::resetPlayback()
{

    m_playbackStep = nullptr;
    m_playbackIteration = 0;

    for (auto child : m_children)
    {
        child->resetPlayback();
    }
}

Interval *LoopStep::currentInterval()
{
    if (m_children.empty())
        return nullptr;

    Step *step = m_playbackStep != nullptr ? m_playbackStep : m_children.front();
    return step->currentInterval();
}

bool LoopStep::deleteStep(Step *step)
{
    auto it = m_children.begin();
    while (it != m_children.end())
    {
        Step *child = *it;
        if (child == step)
        {
            delete step;
            m_children.erase(it);

            if (m_manager)
                m_manager->notifyChange(true);

            return true;
        }

        ++it;
    }
    return false;
}

bool LoopStep::moveStepUp(Step *step)
{
    if (m_children.empty())
        return false;

    size_t cur = 0;
    size_t prev = SIZE_MAX;
    size_t max = m_children.size();

    while(cur < max && m_children.at(cur) != step)
    {
        prev = cur;
        ++cur;
    }

    if (cur == 0)
        return false; // Nothing to do

    if (cur >= max)
        return false; // Not a child of this Loop

    Step *temp = m_children.at(cur);
    m_children[cur] = m_children.at(prev);
    m_children[prev] = temp;

    if (m_manager)
        m_manager->notifyChange();

    return true;
}

bool LoopStep::moveStepDown(Step *step)
{
    auto it = m_children.begin();
    while (it != m_children.end() && *it != step)
    {
        ++it;
    }

    if (it == m_children.end())
        return false; // Not a child of this Loop

    auto itNext = std::next(it);

    if (itNext == m_children.end())
        return true; // It's a child but the last element, nothing to do

    std::iter_swap(it, itNext);

    if (m_manager)
        m_manager->notifyChange();

    return true;
}

bool LoopStep::changeType(Step *step, const StepType newType)
{
    auto it = m_children.begin();
    while (it != m_children.end() && *it != step)
    {
        ++it;
    }

    if (it == m_children.end())
        return false; // Not a child of this Loop

    if (newType == StepType::Loop)
    {
        delete *it;
        *it = new LoopStep(m_manager);
    }
    else
    {
        (*it)->setType(newType);
    }

    if (m_manager)
        m_manager->notifyChange(true);

    return true;
}
