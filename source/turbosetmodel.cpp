#include "turbosetmodel.h"
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QTimer>
#include <QTimerEvent>
#include <climits>

const QString TurboSetModel::TurboSetTag    = "TurboSet";
const QString TurboSetModel::IntervalTag    = "interval";
const QString TurboSetModel::LoopTag        = "loop";
const QString TurboSetModel::TypeAttr       = "type";
const QString TurboSetModel::TextAttr       = "text";
const QString TurboSetModel::DurationAttr   = "duration";
const QString TurboSetModel::IterationsAttr = "iterations";

TurboSetModel::TurboSetModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_dirty(false)
    , m_currentStep(nullptr)
    , m_currentInterval(nullptr)
    , m_nextInterval(nullptr)
    , m_playbackState(PlaybackState::Ready)
    , m_timeRemaining(-1)
{
    m_timer = new QTimer(this);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(onStepFinished()));
}

std::vector<Step *> TurboSetModel::getIntervals() const
{
    return m_steps;
}

void TurboSetModel::addStep(StepType type, Step *parent)
{
    Step *step = nullptr;
    if (type == StepType::Loop)
    {
        step = new LoopStep(this);
    }
    else
    {
        step = new Interval(type, this);
    }

    if (!step)
    {
        Q_ASSERT(false);
        return;
    }

    if (parent && parent->type() == StepType::Loop)
    {
        LoopStep *loop = dynamic_cast<LoopStep*>(parent);
        if (!loop)
        {
            Q_ASSERT(false);
            return;
        }
        loop->addChild(step);
    }
    else
    {
        m_steps.push_back(step);
    }

    m_dirty = true;
    emit setChanged();
}

int TurboSetModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return (int)m_steps.size();
}

QVariant TurboSetModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);
    return QVariant();
}

bool TurboSetModel::serialise(const QString &file)
{
    if (file.isEmpty())
        return true;

    QFile xmlFile(file);
    if (!xmlFile.open(QIODevice::WriteOnly))
    {
        Q_ASSERT(false);
        return false;
    }

    QDomDocument doc(TurboSetTag);
    QDomElement root = doc.createElement(TurboSetTag);
    for (auto step : m_steps)
    {
        step->serialise(doc, root);
    }
    doc.appendChild(root);

    QTextStream ts(&xmlFile);
    ts << doc.toString();

    xmlFile.close();

    m_dirty = false;
    return true;
}

bool TurboSetModel::deserialise(const QString &file)
{
    if (file.isEmpty())
        return true;

    clearSet();

    QFile xmlFile(file);
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        Q_ASSERT(false);
        return false;
    }

    QDomDocument doc(TurboSetTag);
    if (!doc.setContent(&xmlFile))
    {
        xmlFile.close();
        Q_ASSERT(false);
        return false;
    }

    xmlFile.close();

    QDomElement root = doc.documentElement();

    QDomNode child = root.firstChild();

    while (!child.isNull())
    {
        processXmlNode(child, nullptr);
        child = child.nextSibling();
    }

    m_dirty = false;

    emit setChanged();

    return true;
}

bool TurboSetModel::newSet()
{
    clearSet();
    m_dirty = true;
    emit setChanged();
    return true;
}

bool TurboSetModel::dirty() const
{
    return m_dirty;
}

void TurboSetModel::startSet()
{
    if (m_steps.empty())
    {
        Q_ASSERT(false);
        return;
    }

    m_playbackState = PlaybackState::Playing;
    if (!m_currentStep)
    {
        m_currentStep = m_steps.front();
        m_currentInterval = nullptr;
        m_nextInterval = nullptr;

        emit setStarted();
    }
    else
        emit setResumed();

    startCurrentStep();
}

void TurboSetModel::pauseSet()
{
    m_playbackState = PlaybackState::Paused;
    if (!m_timer)
    {
        Q_ASSERT(false);
        return;
    }
    m_timeRemaining = m_timer->remainingTime();
    m_timer->stop();
    emit setPaused();
}

void TurboSetModel::stopSet()
{
    m_playbackState = PlaybackState::Ready;
    emit setStopped();

    if (!m_timer)
    {
        Q_ASSERT(false);
        return;
    }

    m_timer->stop();
    resetPlaybackStates();
}

void TurboSetModel::notifyChange(bool redrawNeeded /*= false*/)
{
    m_dirty = true;
    if (redrawNeeded)
        emit setChanged();
}

Interval* TurboSetModel::currentInterval()
{
    return m_currentInterval;
}

Interval* TurboSetModel::nextInterval()
{
    return m_nextInterval;
}

bool TurboSetModel::currentIteration(uint32_t &current, uint32_t &total)
{
    return (m_currentStep) ? m_currentStep->currentIteration(current, total) : false;
}

void TurboSetModel::onStepDeleted(Step *step)
{
    auto it = m_steps.begin();
    while (it != m_steps.end())
    {
        Step *child = *it;
        if (child == step)
        {
            delete child;
            m_steps.erase(it);
            emit setChanged();
            return;
        }

        ++it;
    }
}

void TurboSetModel::onStepMovedUp(Step *step)
{
    if (m_steps.empty())
    {
        Q_ASSERT(false);
        return;
    }

    size_t cur = 0;
    size_t prev = SIZE_MAX;
    size_t max = m_steps.size();

    while(cur < max && m_steps.at(cur) != step)
    {
        prev = cur;
        ++cur;
    }

    if (cur == 0)
        return; // Nothing to do

    if (cur >= max)
    {
        Q_ASSERT(false);
        return;
    }

    std::iter_swap(m_steps.begin() + cur, m_steps.begin() + prev);

    emit setChanged();
}

void TurboSetModel::onStepMovedDown(Step *step)
{
    if (m_steps.empty())
    {
        Q_ASSERT(false);
        return;
    }

    auto it = m_steps.begin();
    while (it != m_steps.end() && *it != step)
    {
        ++it;
    }

    if (it == m_steps.end())
    {
        Q_ASSERT(false);
        return;
    }

    auto itNext = std::next(it);

    if (itNext == m_steps.end())
        return; // Already bottom element, nothing to do

    std::iter_swap(it, itNext);
    emit setChanged();
}

void TurboSetModel::onTypeChanged(Step *step, const StepType newType)
{
    if (m_steps.empty())
    {
        Q_ASSERT(false);
        return;
    }

    auto it = m_steps.begin();
    while (it != m_steps.end() && *it != step)
    {
        ++it;
    }

    if (it == m_steps.end())
    {
        Q_ASSERT(false);
        return;
    }

    if (newType == StepType::Loop)
    {
        delete *it;
        *it = new LoopStep(this);
    }
    else
    {
        (*it)->setType(newType);
    }

    emit setChanged();
}

void TurboSetModel::onStepFinished()
{
    if (!m_timer)
    {
        Q_ASSERT(false);
        return;
    }

    m_timer->stop();
    m_timeRemaining = -1;

    if (!m_currentStep)
    {
        Q_ASSERT(false);
        emit playbackError("An unexpected error occurred during playback");
        return;
    }

    if (m_playbackState == PlaybackState::Playing)
        startCurrentStep();
}

bool TurboSetModel::processXmlNode(QDomNode &node, LoopStep *parent)
{
    QDomElement el = node.toElement();

    if (!el.isNull())
    {
        if (el.tagName().compare(IntervalTag, Qt::CaseInsensitive) == 0)
        {
            Interval *interval = Interval::fromXml(el, this);
            if (interval)
            {
                if (parent)
                    parent->addChild(interval);
                else
                    m_steps.push_back(interval);
            }
        }
        else if (el.tagName().compare(LoopTag, Qt::CaseInsensitive) == 0)
        {
            LoopStep *loop = new LoopStep(this);

            if (el.attributeNode(IterationsAttr).isNull())
            {
                Q_ASSERT(false);
                delete loop;
            }
            else
            {
                loop->setIterations(el.attribute(IterationsAttr).toUInt());
                if (parent)
                    parent->addChild(loop);
                else
                    m_steps.push_back(loop);

                QDomNode loopChild = el.firstChild();

                while (!loopChild.isNull())
                {
                    processXmlNode(loopChild, loop);
                    loopChild = loopChild.nextSibling();
                }
            }
        }
    }
    return true;
}

void TurboSetModel::clearSet()
{
    for (auto step: m_steps)
    {
        delete step;
    }
    m_steps.clear();
}

Step* TurboSetModel::nextStep(Step *current)
{
    if (m_steps.empty())
    {
        Q_ASSERT(false);
        return nullptr;
    }

    if (!current)
    {
        return m_steps.front();
    }

    auto it = m_steps.begin();
    while (it != m_steps.end())
    {
        if (*it == current)
            break;

        ++it;
    }

    if (it == m_steps.end() || ++it == m_steps.end())
    {
        emit setComplete();
        resetPlaybackStates();
        return nullptr;
    }

    return *it;
}

void TurboSetModel::startCurrentStep()
{
    if (!m_currentStep)
    {
        Q_ASSERT(false);
        return;
    }

    if (m_timeRemaining != -1 && m_currentStep && m_currentInterval)
    {
        m_timer->start(m_timeRemaining);
    }
    else
    {
        m_currentInterval = m_currentStep->nextInterval();
        if (!m_currentInterval)
        {
            m_currentStep = nextStep(m_currentStep);
            if (!m_currentStep)
                return;

            m_currentInterval = m_currentStep->nextInterval();
            if (!m_currentInterval)
            {
                return;
            }
        }

        m_nextInterval = m_currentStep->queryNext();
        if (!m_nextInterval)
        {
            Step *next = nextStep(m_currentStep);
            if (next)
                m_nextInterval = next->queryNext();
        }

        timeCurrentInterval();

        emit intervalStarted();
    }
}

void TurboSetModel::timeCurrentInterval()
{
    int durationMs = (int)m_currentInterval->duration() * 1000;
    m_timer->start(durationMs);
}

void TurboSetModel::resetPlaybackStates()
{
    m_timeRemaining = -1;

    m_currentStep = nullptr;
    m_currentInterval = nullptr;
    m_nextInterval = nullptr;

    for (auto step : m_steps)
    {
        step->resetPlayback();
    }
}
