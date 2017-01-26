#include "stepwidget.h"
#include <QPainter>
#include <QMessageBox>
#include <QMenu>
#include "step.h"
#include "stepresources.h"

static const int StepHeight = 110;
static const int LoopSpacing = 10;

StepWidget::StepWidget(Step *step, IFontAwesome *fontAwesome, QWidget *parent)
    : QWidget(parent)
    , m_step(step)
    , m_fontAwesome(fontAwesome)
    , m_editFont("Century Gothic", 12, QFont::Normal)
    , m_labelFont("Century Gothic", 12, QFont::Bold)
    , m_titleArea(nullptr)
    , m_title(nullptr)
    , m_icon(nullptr)
{
}

void StepWidget::onDelete(bool checked)
{
    Q_UNUSED(checked);
    QMessageBox confirm;
    confirm.setText("Are you sure you want to delete this item from the set?");
    confirm.setInformativeText("This action cannot be undone.");
    confirm.setIcon(QMessageBox::Question);
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    int button = confirm.exec();
    if (button == QMessageBox::Yes)
        emit deleted(m_step);
}

void StepWidget::onMoveUp(bool checked)
{
    Q_UNUSED(checked);
    emit movedUp(m_step);
}

void StepWidget::onMoveDown(bool checked)
{
    Q_UNUSED(checked);
    emit movedDown(m_step);
}

void StepWidget::initAlignment()
{
    if (!m_icon || !m_title)
    {
        Q_ASSERT(false);
        return;
    }

    DrawFaIconToLabel(m_icon, TypeToFaIcon(m_step->type()), m_fontAwesome);
    m_icon->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_title->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void StepWidget::initButtons()
{
    // Controls Area
    m_buttonsArea = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_buttonsArea);

    m_delete = new QPushButton(m_fontAwesome->faIcon(fa::remove), QString(), m_buttonsArea);
    m_moveUp = new QPushButton(m_fontAwesome->faIcon(fa::longarrowup), QString(), m_buttonsArea);
    m_moveDown = new QPushButton(m_fontAwesome->faIcon(fa::longarrowdown), QString(), m_buttonsArea);

    m_delete->setFlat(true);
    m_moveUp->setFlat(true);
    m_moveDown->setFlat(true);

    m_delete->setToolTip("Delete");
    m_moveUp->setToolTip("Move Up");
    m_moveDown->setToolTip("Move Down");

    layout->addWidget(m_delete);
    layout->addWidget(m_moveUp);
    layout->addWidget(m_moveDown);

    m_buttonsArea->setLayout(layout);

    QObject::connect(m_delete, SIGNAL(clicked(bool)), this, SLOT(onDelete(bool)));
    QObject::connect(m_moveUp, SIGNAL(clicked(bool)), this, SLOT(onMoveUp(bool)));
    QObject::connect(m_moveDown, SIGNAL(clicked(bool)), this, SLOT(onMoveDown(bool)));
}

void StepWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (!m_fontAwesome)
    {
        Q_ASSERT(false);
        return;
    }

    QMenu menu(this);
    QMenu *changeType = menu.addMenu(m_fontAwesome->faIcon(fa::cogs), "Change type");
    auto typeMap = AddTypesToMenu(changeType, m_fontAwesome);
    menu.addSeparator();
    QAction *moveUp = menu.addAction(m_fontAwesome->faIcon(fa::longarrowup), "Move up");
    QAction *moveDown = menu.addAction(m_fontAwesome->faIcon(fa::longarrowdown), "Move down");
    menu.addSeparator();
    QAction *del = menu.addAction(m_fontAwesome->faIcon(fa::remove), "Delete");

    QAction *act = menu.exec(event->globalPos());

    auto it = typeMap.find(act);
    if (it != typeMap.end())
        emit typeChanged(m_step, it->second);
    else if (act == moveUp)
        onMoveUp();
    else if (act == moveDown)
        onMoveDown();
    else if (act == del)
        onDelete();
}

IntervalWidget::IntervalWidget(Step *step, IFontAwesome *fontAwesome, QWidget *parent)
    : StepWidget(step, fontAwesome, parent)
    , m_layout(nullptr)
    , m_timeArea(nullptr)
    , m_timeLabel(nullptr)
    , m_timeEdit(nullptr)
    , m_descriptionArea(nullptr)
    , m_descriptionLabel(nullptr)
    , m_textEdit(nullptr)
{
    m_layout = new QHBoxLayout(this);

    // Title/icon area
    m_titleArea = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_titleArea);

    m_title = new QLabel(m_titleArea);
    m_icon = new QLabel(m_titleArea);

    m_title->setFont(m_labelFont);
    m_title->setText(TypeToString(m_step->type()));

    initAlignment();

    layout->addWidget(m_title);
    layout->addWidget(m_icon);

    m_titleArea->setLayout(layout);
    m_layout->addWidget(m_titleArea);
    m_layout->addSpacing(20);

    // Time/duration area
    m_timeArea = new QWidget(this);
    layout = new QVBoxLayout(m_timeArea);

    m_timeLabel= new QLabel("Duration (hh:mm:ss)", m_timeArea);
    m_timeLabel->setFont(m_editFont);

    m_timeEdit = new QTimeEdit(m_timeArea);
    m_timeEdit->setFont(m_editFont);
    m_timeEdit->setDisplayFormat("hh:mm:ss");
    m_timeEdit->setMinimumTime(QTime(0, 0, 1));

    QTime time(0,0);
    time = time.addSecs(m_step->duration());
    m_timeEdit->setTime(time);

    layout->addWidget(m_timeLabel);
    layout->addWidget(m_timeEdit);
    m_timeArea->setLayout(layout);
    m_layout->addWidget(m_timeArea);
    m_layout->addSpacing(20);

    // Description area
    m_descriptionArea = new QWidget(this);
    layout = new QVBoxLayout(m_descriptionArea);

    m_descriptionLabel = new QLabel("Description", m_descriptionArea);
    m_descriptionLabel->setFont(m_editFont);

    m_textEdit = new QTextEdit(m_descriptionArea);
    m_textEdit->setFont(m_editFont);
    Interval *interval = dynamic_cast<Interval*>(m_step);
    if (interval)
        m_textEdit->setText(interval->text());

    layout->addWidget(m_descriptionLabel);
    layout->addWidget(m_textEdit);

    m_descriptionArea->setLayout(layout);
    m_layout->addWidget(m_descriptionArea);

    // Controls Area
    initButtons();
    m_layout->addWidget(m_buttonsArea);

    setLayout(m_layout);

    QObject::connect(m_timeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(onTimeChanged(QTime)));
    QObject::connect(m_textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

int IntervalWidget::idealHeight() const
{
    return StepHeight;
}

void IntervalWidget::onTimeChanged(const QTime &time)
{
    uint duration = (time.hour() * 3600) + (time.minute() * 60) + time.second();
    m_step->setDuration(duration);
}

void IntervalWidget::onTextChanged()
{
    Interval *interval = dynamic_cast<Interval*>(m_step);
    if (!interval || !m_textEdit || !m_textEdit->document())
    {
        Q_ASSERT(false);
        return;
    }
    interval->setText(m_textEdit->document()->toPlainText());
}

void IntervalWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), TypeToBgColour(m_step->type()));
}

LoopStepWidget::LoopStepWidget(Step *step, IFontAwesome *fontAwesome, QWidget *parent)
    : StepWidget(step, fontAwesome, parent)
    , m_topRow(nullptr)
    , m_topRowLayout(nullptr)
    , m_loopArea(nullptr)
    , m_loopLabel(nullptr)
    , m_spinBox(nullptr)
    , m_addButtonArea(nullptr)
    , m_buttonLabel(nullptr)
    , m_addChildButton(nullptr)
{
    m_topRow = new QWidget(this);
    m_topRowLayout = new QHBoxLayout(m_topRow);

    // Title/icon area
    m_titleArea = new QWidget(m_topRow);

    QVBoxLayout *layout = new QVBoxLayout(m_titleArea);

    m_title = new QLabel("Loop", m_titleArea);
    m_icon = new QLabel(m_titleArea);

    layout->addWidget(m_title);
    layout->addWidget(m_icon);

    m_title->setFont(m_labelFont);

    m_titleArea->setLayout(layout);
    m_topRowLayout->addWidget(m_titleArea);
    m_topRowLayout->addSpacing(20);

    initAlignment();

    // Loop area
    m_loopArea = new QWidget(m_topRow);
    layout = new QVBoxLayout(m_loopArea);
    m_loopLabel = new QLabel("Repeat x", m_loopArea);
    m_loopLabel->setFont(m_editFont);

    m_spinBox = new QSpinBox(m_loopArea);
    m_spinBox->setFont(m_editFont);
    m_spinBox->setRange(1, 1000);
    LoopStep *loop = dynamic_cast<LoopStep*>(m_step);
    if (loop)
        m_spinBox->setValue(loop->iterations());

    layout->addWidget(m_loopLabel);
    layout->addWidget(m_spinBox);

    m_loopArea->setLayout(layout);

    m_topRowLayout->addWidget(m_loopArea);
    m_topRowLayout->addSpacing(20);

    // Button area
    m_addButtonArea = new QWidget(m_topRow);
    layout = new QVBoxLayout(m_addButtonArea);

    m_buttonLabel = new QLabel("Add to loop", m_topRow);
    m_buttonLabel->setFont(m_editFont);

    m_addChildButton = new QPushButton(m_fontAwesome->faIcon(fa::plus), QString(), this);
    m_addChildButton->setIconSize(QSize(32, 32));
    m_addChildButton->setFlat(true);

    m_addButtonArea->setLayout(layout);
    layout->addWidget(m_buttonLabel);
    layout->addWidget(m_addChildButton);
    m_topRowLayout->addWidget(m_addButtonArea);

    m_topRowLayout->addStretch();

    initButtons();
    m_topRowLayout->addWidget(m_buttonsArea);

    m_topRow->setLayout(m_topRowLayout);

    setLayout(nullptr);

    AdjustLayout();

    QObject::connect(m_addChildButton, SIGNAL(pressed()), this, SLOT(onAddChildPressed()));
    QObject::connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(onIterationsChanged(int)));
}

int LoopStepWidget::idealHeight() const
{
    int height = StepHeight;
    for (auto child : m_children)
    {
        height += child->idealHeight() + LoopSpacing;
    }
    return height;
}

void LoopStepWidget::addChild(StepWidget *child)
{
    m_children.push_back(child);
    AdjustLayout();
}

void LoopStepWidget::onStepDeleted(Step *step)
{
    for (auto child : m_children)
    {
        if (child->step() == step)
            child->hide();
    }

    LoopStep *loop = dynamic_cast<LoopStep*>(m_step);
    if (!loop || !loop->deleteStep(step))
    {
        Q_ASSERT(false);
    }
}

void LoopStepWidget::onStepMovedUp(Step *step)
{
    LoopStep *loop = dynamic_cast<LoopStep*>(m_step);
    if (!loop || !loop->moveStepUp(step))
    {
        Q_ASSERT(false);
    }
}

void LoopStepWidget::onStepMovedDown(Step *step)
{
    LoopStep *loop = dynamic_cast<LoopStep*>(m_step);
    if (!loop || loop->moveStepDown(step))
    {
        Q_ASSERT(false);
    }
}

void LoopStepWidget::onTypeChanged(Step *step, const StepType newType)
{
    LoopStep *loop = dynamic_cast<LoopStep*>(m_step);
    if (!loop || loop->changeType(step, newType))
    {
        Q_ASSERT(false);
    }
}

void LoopStepWidget::onAddChildPressed()
{
    QMetaObject::invokeMethod(window(), "onShowAddMenu", Qt::DirectConnection, Q_ARG(const QPoint, mapToGlobal(m_addChildButton->geometry().topRight())), Q_ARG(Step *, m_step));
}

void LoopStepWidget::onIterationsChanged(int iterations)
{
    LoopStep *loop = dynamic_cast<LoopStep*>(m_step);
    if (!loop)
    {
        Q_ASSERT(false);
        return;
    }

    loop->setIterations(iterations);
}

void LoopStepWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), TypeToBgColour(StepType::Loop));
    painter.fillRect(QRect(5, m_topRow->rect().bottom(), 3, height()), QColor(0x40, 0x40, 0x40));
}

void LoopStepWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    AdjustLayout();
}

void LoopStepWidget::AdjustLayout()
{
    static const int Spacing = 10;
    static const int MarginLeft = 20;

    QRect rect(0, 0, width(), StepHeight);
    m_topRow->setGeometry(rect);

    int y = m_topRow->height();

    for (auto child : m_children)
    {
        rect.setRect(MarginLeft, y, width() - Spacing, child->idealHeight());
        child->setGeometry(rect);
        y += child->idealHeight() + Spacing;
    }
}
