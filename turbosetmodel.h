#ifndef TURBOSETMODEL_H
#define TURBOSETMODEL_H

#include "types.h"
#include "step.h"
#include "isetmanager.h"
#include "istepmanager.h"
#include <QAbstractListModel>
#include <QtXml/QDomElement>
#include <vector>

class QTimer;

class TurboSetModel : public QAbstractListModel, public IStepManager, public ISetManager
{
    Q_OBJECT

public:

    enum class PlaybackState
    {
        Ready,
        Playing,
        Paused
    };

    static const QString TurboSetTag;
    static const QString IntervalTag;
    static const QString LoopTag;
    static const QString TypeAttr;
    static const QString TextAttr;
    static const QString DurationAttr;
    static const QString IterationsAttr;

    explicit TurboSetModel(QObject *parent = 0);

    std::vector<Step*> getIntervals() const;

    void addStep(StepType type, Step *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool serialise(const QString &file);
    bool deserialise(const QString &file);
    bool newSet();

    bool dirty() const;

    void startSet();
    void pauseSet();
    void stopSet();

public: // IStepManager
    virtual void notifyChange(bool redrawNeeded = false) override;

public: // ISetManager
    virtual Interval* currentInterval() override;
    virtual Interval* nextInterval() override;
    virtual bool currentIteration(uint32_t &current, uint32_t &total) override;

signals:
    void setChanged();
    void setStarted();
    void intervalStarted();
    void setPaused();
    void setResumed();
    void setComplete();
    void setStopped();
    void playbackError(const QString &error);

protected slots:
    void onStepDeleted(Step *step);
    void onStepMovedUp(Step *step);
    void onStepMovedDown(Step *step);
    void onTypeChanged(Step *step, const StepType newType);
    void onStepFinished();

protected:
    bool processXmlNode(QDomNode &node, LoopStep *parent);
    void clearSet();

    Step* nextStep(Step *current);

    void startCurrentStep();
    void timeCurrentInterval();
    void resetPlaybackStates();

protected:
    std::vector<Step*> m_steps;
    bool m_dirty;
    Step *m_currentStep;
    Interval *m_currentInterval;
    Interval *m_nextInterval;
    QTimer *m_timer;
    int m_timeRemaining;
    PlaybackState m_playbackState;
};

#endif // TURBOSETMODEL_H
