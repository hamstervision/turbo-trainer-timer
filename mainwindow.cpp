#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "stepwidget.h"
#include "stepresources.h"

#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QCloseEvent>
#include <QSettings>

static const QString AppRegKey = "TurboTrainerTimer";
static const QString FileFilter = "Turbo Trainer Timer Set (*.tttset)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_scrollArea(nullptr)
    , m_stagingArea(nullptr)
    , m_showtimeWindow(nullptr)
    , m_showtimeWidget(nullptr)
    , m_fontAwesome(nullptr)
    , m_scrollPos(0)
{

    m_fontAwesome = new QtAwesome(this);
    if (!m_fontAwesome->initFontAwesome())
    {
        Q_ASSERT(false);
    }

    m_scrollArea = new QScrollArea(this);
    m_stagingArea = new StagingArea(&m_setModel, this, this);
    m_showtimeWidget = new ShowTimeWidget(&m_setModel, this, this);
    m_showtimeWidget->hide();

    ui->setupUi(this);
    setCentralWidget(m_scrollArea);
    m_scrollArea->setWidget(m_stagingArea);

    ui->actionAddStep->setIcon(m_fontAwesome->icon(fa::plus));
    ui->actionPlay->setIcon(m_fontAwesome->icon(fa::play));
    ui->actionPause->setIcon(m_fontAwesome->icon(fa::pause));
    ui->actionStop->setIcon(m_fontAwesome->icon(fa::stop));

    ui->actionPlay->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(false);

    QObject::connect(&m_setModel, SIGNAL(setChanged()), m_stagingArea, SLOT(onSetChanged()));
    QObject::connect(m_stagingArea, SIGNAL(sizeChanged()), this, SLOT(onStagingAreaResized()));
    QObject::connect(&m_setModel, SIGNAL(intervalStarted()), m_showtimeWidget, SLOT(onIntervalStarted()));
    QObject::connect(&m_setModel, SIGNAL(setStarted()), m_showtimeWidget, SLOT(onSetStarted()));
    QObject::connect(&m_setModel, SIGNAL(setPaused()), m_showtimeWidget, SLOT(onSetPaused()));
    QObject::connect(&m_setModel, SIGNAL(setResumed()), m_showtimeWidget, SLOT(onSetResumed()));
    QObject::connect(&m_setModel, SIGNAL(setComplete()), m_showtimeWidget, SLOT(onSetComplete()));
    QObject::connect(&m_setModel, SIGNAL(setStopped()), m_showtimeWidget, SLOT(onSetStopped()));
    QObject::connect(&m_setModel, SIGNAL(setStarted()), this, SLOT(onSetStarted()));
    QObject::connect(&m_setModel, SIGNAL(setPaused()), this, SLOT(onSetPaused()));
    QObject::connect(&m_setModel, SIGNAL(setResumed()), this, SLOT(onSetResumed()));
    QObject::connect(&m_setModel, SIGNAL(setComplete()), this, SLOT(onSetComplete()));
    QObject::connect(&m_setModel, SIGNAL(setStopped()), this, SLOT(onSetStopped()));
    QObject::connect(&m_setModel, SIGNAL(playbackError(QString)), m_showtimeWidget, SLOT(onPlaybackError(QString)));
    QObject::connect(m_scrollArea->verticalScrollBar(), SIGNAL(sliderMoved(int)), this, SLOT(onSliderMoved(int)));

    // Restore position/state from previous session
    QSettings settings(AppRegKey);
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

MainWindow::~MainWindow()
{
    delete ui;
}

QIcon MainWindow::faIcon(const int character)
{
    if (!m_fontAwesome)
    {
        Q_ASSERT(false);
        return QIcon();
    }

    return m_fontAwesome->icon(character);
}

QFont MainWindow::faFont(const int size)
{
    if (!m_fontAwesome)
    {
        Q_ASSERT(false);
        return QFont();
    }

    return m_fontAwesome->font(size);
}

void MainWindow::on_actionAddStep_triggered()
{
    onShowAddMenu(ui->mainToolBar->mapToGlobal(ui->mainToolBar->actionGeometry(ui->actionAddStep).bottomRight()), nullptr);
}

void MainWindow::onStagingAreaResized()
{
    if (m_stagingArea && !m_stagingArea->isHidden())
        m_stagingArea->setGeometry(0, 0, m_scrollArea->viewport()->width(), m_stagingArea->idealHeight());

    // Force scroll area to move staging area to current scroll position.
    if (m_scrollArea && !m_scrollArea->isHidden())
        m_scrollArea->verticalScrollBar()->setSliderPosition(m_scrollPos);
}

void MainWindow::onSliderMoved(int pos)
{
    m_scrollPos = pos;
}


void MainWindow::onShowAddMenu(const QPoint menuPos, Step *parent)
{
    QMenu menu(this);
    QAction *warmUp     = menu.addAction(TypeToString(StepType::WarmUp));
    QAction *drill      = menu.addAction(TypeToString(StepType::Drill));
    QAction *effort     = menu.addAction(TypeToString(StepType::Effort));
    QAction *recovery   = menu.addAction(TypeToString(StepType::Recovery));
    QAction *coolDown   = menu.addAction(TypeToString(StepType::CoolDown));
    QAction *loop       = menu.addAction(TypeToString(StepType::Loop));

    QAction *act = menu.exec(menuPos);
    if (act == warmUp)
    {
        m_setModel.addStep(StepType::WarmUp, parent);
    }
    else if (act == drill)
    {
        m_setModel.addStep(StepType::Drill, parent);
    }
    else if (act == effort)
    {
        m_setModel.addStep(StepType::Effort, parent);
    }
    else if (act == recovery)
    {
        m_setModel.addStep(StepType::Recovery, parent);
    }
    else if (act == coolDown)
    {
        m_setModel.addStep(StepType::CoolDown, parent);
    }
    else if (act == loop)
    {
        m_setModel.addStep(StepType::Loop, parent);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    onStagingAreaResized();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(AppRegKey);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    if (m_setModel.dirty())
    {
        QMessageBox::StandardButton button = QMessageBox::question(this, "Save current set",
                                                                   "You have unsaved changes to the current set. Would you like to save these changes before exiting?",
                                                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch(button)
        {
        case QMessageBox::Yes:
            if (!save(false))
            {
                event->ignore();
                return; // Don't skip closing if save failed or was cancelled.
            }
            break;
        case QMessageBox::No:
            break;
        case QMessageBox::Cancel:
            event->ignore();
            return; // skip closing of application
        }
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::onSetStarted()
{
    ui->actionPlay->setEnabled(false);
    ui->actionPause->setEnabled(true);
    ui->actionStop->setEnabled(true);
    ui->actionAddStep->setEnabled(false);
}

void MainWindow::onSetPaused()
{
    ui->actionPlay->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(true);
    ui->actionAddStep->setEnabled(false);
}

void MainWindow::onSetResumed()
{
    ui->actionPlay->setEnabled(false);
    ui->actionPause->setEnabled(true);
    ui->actionStop->setEnabled(true);
    ui->actionAddStep->setEnabled(false);
}

void MainWindow::onSetComplete()
{
    ui->actionPlay->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(false);
    ui->actionAddStep->setEnabled(false);
}

void MainWindow::onSetStopped()
{
    ui->actionPlay->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(false);
    ui->actionAddStep->setEnabled(true);

    m_showtimeWidget->setParent(nullptr);
    m_showtimeWidget->hide();

    setCentralWidget(m_scrollArea);
    m_scrollArea->show();
}

void MainWindow::on_actionPlay_triggered()
{
    if (m_scrollArea && !m_scrollArea->isHidden())
    {
        m_scrollArea->setParent(nullptr);
        m_scrollArea->hide();
    //m_showtimeWidget->showFullScreen();
    }

    setCentralWidget(m_showtimeWidget);
    m_showtimeWidget->show();

    m_setModel.startSet();
}

void MainWindow::on_actionNew_triggered()
{
    if (m_setModel.dirty()
        || QMessageBox::question(this, "Discard Unsaved Data", "Any unsaved changes will be lost. Continue?") != QMessageBox::Yes)
        return;
    m_setModel.newSet();
}

void MainWindow::on_actionOpen_triggered()
{
    m_filePath = QFileDialog::getOpenFileName(this, "Open set", QString(), FileFilter);
    if (!m_filePath.isEmpty())
        m_setModel.deserialise(m_filePath);
}

void MainWindow::on_actionSave_triggered()
{
    save(false);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionSaveAs_triggered()
{
    save(true);
}

bool MainWindow::save(bool forcePrompt)
{
    if (m_filePath.isEmpty() || forcePrompt)
    {
        m_filePath = QFileDialog::getSaveFileName(this, "Save Set", QString(), FileFilter);
        if (m_filePath.isEmpty())
            return false;
    }

    return m_setModel.serialise(m_filePath);
}

void MainWindow::on_actionPause_triggered()
{
    m_setModel.pauseSet();
}

void MainWindow::on_actionStop_triggered()
{
    m_setModel.stopSet();
}
