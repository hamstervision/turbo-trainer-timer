#ifndef STEP_H
#define STEP_H

#include "types.h"
#include "istepmanager.h"
#include <QtXml/QDomElement>
#include <vector>
#include <cstdint>

class Interval;

class Step
{
public:
    Step(const StepType type, IStepManager *manager)
        : m_type(type)
        , m_manager(manager)
        , m_duration(60)
    {
        Q_ASSERT(m_manager != nullptr);
    }

    StepType type() const
    {
        return m_type;
    }

    void setType(const StepType type)
    {
        m_type = type;
    }

    virtual unsigned int duration() const
    {
        return m_duration;
    }

    void setDuration(const unsigned int duration)
    {
        m_duration = duration;
        notifyChange();
    }

    virtual Interval* currentInterval() = 0;
    virtual Interval* nextInterval() = 0;
    virtual Interval* queryNext(bool reset = false) = 0;
    virtual void resetPlayback() = 0;
    virtual bool currentIteration(uint32_t &current, uint32_t &total) = 0;

    virtual void serialise(QDomDocument &file, QDomElement &parent) const = 0;

    virtual size_t getChildCount() const = 0;
    virtual Step* getChild(size_t index) const = 0;

    void notifyChange()
    {
        if (m_manager)
            m_manager->notifyChange();
    }

protected:
    StepType m_type;
    IStepManager *m_manager;
    unsigned int m_duration;
};

class Interval : public Step
{
public:

    Interval(const StepType type, IStepManager *manager)
        : Step(type, manager)
        , m_played(false)
    {

    }

    virtual QString text() const
    {
        return m_text;
    }

    void setText(const QString &text)
    {
        m_text = text;
        notifyChange();
    }

    virtual size_t getChildCount() const override
    {
        return 0;
    }

    virtual Step* getChild(size_t index) const override
    {
        Q_UNUSED(index);
        return nullptr;
    }

    virtual void serialise(QDomDocument &file, QDomElement &parent) const override;

    virtual Interval* nextInterval() override;
    virtual Interval* queryNext(bool reset = false) override;

    virtual void resetPlayback() override
    {
        m_played = false;
    }

    virtual Interval* currentInterval() override
    {
        return this;
    }

    virtual bool currentIteration(uint32_t &current, uint32_t &total) override
    {
        current = 1;
        total = 1;
        return false; // only ever 1 iteration for an interval
    }

    static Interval* fromXml(QDomElement &node, IStepManager *manager);

protected:
    bool populateFromXml(QDomElement &node);

protected:
    QString m_text;
    bool m_played;
};


class LoopStep : public Step
{
public:
    LoopStep(IStepManager *manager);
    virtual ~LoopStep();

    unsigned int duration() const override;

    size_t getChildCount() const override
    {
        return m_children.size();
    }

    void addChild(Step *child);

    Step* getChild(size_t index) const override;

    virtual void serialise(QDomDocument &file, QDomElement &parent) const override;

    virtual Interval* nextInterval() override;
    virtual Interval* queryNext(bool reset = false) override;

    virtual void resetPlayback() override;

    virtual Interval* currentInterval() override;

    unsigned int iterations() const
    {
        return m_iterations;
    }

    virtual bool currentIteration(uint32_t &current, uint32_t &total) override
    {
        current = m_playbackIteration + 1; // zero-based index, present as one-based
        total = m_iterations;
        return true;
    }

    bool deleteStep(Step *step);
    bool moveStepUp(Step *step);
    bool moveStepDown(Step *step);
    bool changeType(Step *step, const StepType newType);

    void setIterations(const unsigned int it)
    {
        m_iterations = it;
        notifyChange();
    }

protected:
    std::vector<Step*> m_children;
    unsigned int m_iterations;
    unsigned int m_playbackIteration;
    Step *m_playbackStep;
};

#endif // STEP_H
