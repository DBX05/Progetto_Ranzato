#include "MainWindow.h"
#include "EventModel.h"
#include "NewEventiDialog.h"
#include "DayWeekView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QLabel>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_calendar(new QCalendarWidget(this)),
      m_listView(new QListView(this)),
      m_modeCombo(new QComboBox(this)),
      m_dayWeekView(new DayWeekView(this)),
      m_model(new EventModel(this))
{
    setWindowTitle("Agenda Qt - Day/Week View Example");

    m_listView->setModel(m_model);

    m_modeCombo->addItem("Day");
    m_modeCombo->addItem("Week");
    m_modeCombo->setCurrentIndex(0);

    auto* addBtn = new QPushButton("Nuovo evento", this);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddEvent);
    connect(m_calendar, &QCalendarWidget::selectionChanged, this, [this](){
        onCalendarSelectionChanged(m_calendar->selectedDate());
    });
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onModeChanged);

    m_dayWeekView->setModel(m_model);
    m_dayWeekView->setDate(m_calendar->selectedDate());

    auto* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(new QLabel("Calendario"));
    leftLayout->addWidget(m_calendar);
    leftLayout->addWidget(addBtn);
    leftLayout->addWidget(m_modeCombo);

    auto* leftWidget = new QWidget;
    leftWidget->setLayout(leftLayout);

    auto* rightLayout = new QVBoxLayout;
    rightLayout->addWidget(new QLabel("Vista Giorno/Settimana"));
    rightLayout->addWidget(m_dayWeekView);
    rightLayout->addWidget(new QLabel("Eventi del giorno"));
    rightLayout->addWidget(m_listView);

    auto* rightWidget = new QWidget;
    rightWidget->setLayout(rightLayout);

    auto* splitter = new QSplitter;
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(1, 1);

    setCentralWidget(splitter);

    populateSampleData();
    refreshListForDate(m_calendar->selectedDate());
}

void MainWindow::onAddEvent() {
    NewEventDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        auto ev = dlg.createEvento();
        m_model->addEvent(ev);
        m_dayWeekView->update();
        refreshListForDate(m_calendar->selectedDate());
    }
}

void MainWindow::onCalendarSelectionChanged(const QDate& date) {
    m_dayWeekView->setDate(date);
    refreshListForDate(date);
}

void MainWindow::onModeChanged(int index) {
    m_dayWeekView->setMode(index == 0 ? DayWeekView::Day : DayWeekView::Week);
}

void MainWindow::populateSampleData() {
    // Create sample events using domain classes
    dateTime now;
    // Event 1: meeting
    orario s1(0, 30, 9);
    orario e1(0, 0, 10);
    auto ev1 = std::make_shared<evento>(1, now, 1, "Team Meeting", s1, e1);
    m_model->addEvent(ev1);

    // Event 2: birthday (assumes compleanno derives from eventoLungo and has getType()==1)
    orario s2(0, 0, 11);
    orario e2(0, 0, 12);
    // If compleanno class exists and constructor matches, create it; otherwise fallback to evento
    try {
        auto bday = std::make_shared<compleanno>(dateTime(), 1, "Compleanno Marco", dateTime(), "Festeggia!", 5, s2, e2);
        m_model->addEvent(bday);
    } catch (...) {
        auto ev2 = std::make_shared<evento>(2, now, 1, "Compleanno Marco", s2, e2);
        m_model->addEvent(ev2);
    }

    // Event 3: group block (raggruppa)
    orario s3(0, 0, 13);
    orario e3(0, 0, 15);
    try {
        auto group = std::make_shared<raggruppa>(3, dateTime(), 1, "Project Block", dateTime(), "Blocco di lavoro");
        m_model->addEvent(group);
    } catch (...) {
        auto ev3 = std::make_shared<evento>(3, now, 1, "Project Block", s3, e3);
        m_model->addEvent(ev3);
    }
}

void MainWindow::refreshListForDate(const QDate& date) {
    auto events = m_model->eventsForDate(date);
    // For simplicity, rebuild a temporary QStringListModel-like display
    QStringList list;
    for (const auto& ev : events) {
        QString line = QString::fromStdString(ev->getNome()) + "  " +
                       QString::fromStdString(ev->getMomentoInizio().getDateTime()).mid(11,5) + " - " +
                       QString::fromStdString(ev->getMomentoFine().getDateTime()).mid(11,5);
        list << line;
    }
    // Use a simple QStringListModel to show items
    auto* model = new QStringListModel(list, this);
    m_listView->setModel(model);
}
