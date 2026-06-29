#include "achievementswindow.h"
#include "ui_achievementswindow.h"
#include "dashboardwindow.h"
#include "databasemanager.h"

#include <QHeaderView>
#include <QTableWidgetItem>
#include <QCoreApplication>
#include <QPixmap>

AchievementsWindow::AchievementsWindow(const QString &username,
                                       const QString &role,
                                       QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AchievementsWindow)
    , backgroundLabel(nullptr)
    , currentUsername(username)
    , currentRole(role)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));
    setWindowTitle("Challenge 30 - Achievements");

    backgroundLabel = new QLabel(this);
    QString imagePath = QCoreApplication::applicationDirPath() + "/../../images/background.png";
    QPixmap bg(imagePath);
    backgroundLabel->setPixmap(bg);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(this->rect());
    backgroundLabel->lower();

    ui->centralwidget->setStyleSheet("background: transparent;");

    ui->frameAchievements->setStyleSheet(
        "#frameAchievements {"
        "background-color: rgba(0, 0, 0, 95);"
        "border-radius: 18px;"
        "}"
        );

    ui->labelTitle->setStyleSheet(
        "color: white;"
        "font-size: 28px;"
        "font-weight: bold;"
        );

    ui->tableWidgetAchievements->setColumnCount(3);
    ui->tableWidgetAchievements->setHorizontalHeaderLabels(
        QStringList() << "Achievement" << "Status" << "Unlocked At"
        );

    ui->tableWidgetAchievements->setStyleSheet(
        "QTableWidget {"
        "background-color: rgba(255, 255, 255, 230);"
        "color: #0f172a;"
        "border: none;"
        "border-radius: 10px;"
        "gridline-color: #cbd5e1;"
        "font-size: 14px;"
        "}"
        "QHeaderView::section {"
        "background-color: #2563eb;"
        "color: white;"
        "padding: 8px;"
        "border: none;"
        "font-weight: bold;"
        "}"
        );

    ui->tableWidgetAchievements->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetAchievements->verticalHeader()->setVisible(false);
    ui->tableWidgetAchievements->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetAchievements->setSelectionMode(QAbstractItemView::NoSelection);

    ui->pushButtonBack->setStyleSheet(
        "QPushButton {"
        "background-color: #111827;"
        "color: white;"
        "border: 1px solid white;"
        "border-radius: 10px;"
        "padding: 10px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1f2937;"
        "}"
        );

    loadAchievements();
    this->showMaximized();
}

AchievementsWindow::~AchievementsWindow()
{
    delete ui;
}

void AchievementsWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (backgroundLabel) {
        backgroundLabel->setGeometry(this->rect());
    }
}

void AchievementsWindow::loadAchievements()
{
    ui->tableWidgetAchievements->setRowCount(0);

    struct AchievementInfo {
        QString key;
        QString name;
    };

    QList<AchievementInfo> allAchievements = {
        {"first_game", "First Game"},
        {"first_win", "First Win"},
        {"perfect_score", "Perfect Score"},
        {"five_games", "Five Games Played"}
    };

    int row = 0;
    for (const AchievementInfo &achievement : allAchievements) {
        bool unlocked = DatabaseManager::hasAchievement(currentUsername, achievement.key);
        QString unlockedAt = unlocked
                                 ? DatabaseManager::getAchievementUnlockedAt(currentUsername, achievement.key)
                                 : "Locked";

        ui->tableWidgetAchievements->insertRow(row);

        QTableWidgetItem *nameItem = new QTableWidgetItem(achievement.name);
        QTableWidgetItem *statusItem = new QTableWidgetItem(unlocked ? "Unlocked" : "Locked");
        QTableWidgetItem *dateItem = new QTableWidgetItem(unlocked ? unlockedAt : "-");

        nameItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setTextAlignment(Qt::AlignCenter);
        dateItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidgetAchievements->setItem(row, 0, nameItem);
        ui->tableWidgetAchievements->setItem(row, 1, statusItem);
        ui->tableWidgetAchievements->setItem(row, 2, dateItem);

        row++;
    }
}

void AchievementsWindow::on_pushButtonBack_clicked()
{
    DashboardWindow *dashboard = new DashboardWindow(currentUsername, currentRole);
    dashboard->show();
    this->close();
}
