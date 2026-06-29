#include "dashboardwindow.h"
#include "ui_dashboardwindow.h"
#include "mainwindow.h"
#include "challengewindow.h"
#include "leaderboardwindow.h"
#include "statisticswindow.h"
#include "questionmanagerwindow.h"
#include "historywindow.h"
#include "startchallengedialog.h"
#include "usermanagerwindow.h"
#include "databasemanager.h"
#include "achievementswindow.h"


#include <QMessageBox>
#include <QCoreApplication>
#include <QPixmap>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DashboardWindow::DashboardWindow(const QString &username, const QString &role, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DashboardWindow)
    , currentUsername(username)
    , currentRole(role)
    , backgroundLabel(nullptr)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));
    // Background
    backgroundLabel = new QLabel(this);
    QString imagePath = QCoreApplication::applicationDirPath() + "/../../images/background.png";
    QPixmap bg(imagePath);
    backgroundLabel->setPixmap(bg);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(this->rect());
    backgroundLabel->lower();

    ui->centralwidget->setStyleSheet("background: transparent;");

    // Main panel/frame style
    ui->frameDashboard->setStyleSheet(
        "#frameDashboard {"
        "background-color: rgba(0, 0, 0, 125);"
        "border-radius: 18px;"
        "}"
        );

    // Optional: give the dashboard a comfortable minimum size
    ui->frameDashboard->setMinimumSize(560, 620);

    // Welcome label
    if (currentRole == "admin") {
        ui->labelWelcome->setText("Welcome Admin, " + currentUsername + "!");
    } else {
        ui->labelWelcome->setText("Welcome, " + currentUsername + "!");
    }

    ui->labelTitle->setText("Challenge 30");

    ui->labelTitle->setStyleSheet(
        "color: white;"
        "font-size: 28px;"
        "font-weight: bold;"
        );

    ui->labelWelcome->setStyleSheet(
        "color: white;"
        "font-size: 18px;"
        "font-weight: 600;"
        );

    // Role badge
    if (currentRole == "admin") {
        ui->labelRoleBadge->setText("ADMIN");
        ui->labelRoleBadge->setStyleSheet(
            "background-color: #dc2626;"
            "color: white;"
            "font-size: 12px;"
            "font-weight: bold;"
            "padding: 6px 14px;"
            "border-radius: 12px;"
            );
    } else {
        ui->labelRoleBadge->setText("PLAYER");
        ui->labelRoleBadge->setStyleSheet(
            "background-color: #2563eb;"
            "color: white;"
            "font-size: 12px;"
            "font-weight: bold;"
            "padding: 6px 14px;"
            "border-radius: 12px;"
            );
    }

    // Stats labels style
    QString statStyle =
        "color: white;"
        "font-size: 16px;"
        "font-weight: 600;";

    ui->labelTotalQuestions->setStyleSheet(statStyle);
    ui->labelTotalPlayers->setStyleSheet(statStyle);
    ui->labelMyGames->setStyleSheet(statStyle);
    ui->labelPlayerLevel->setStyleSheet(statStyle);
    ui->labelPlayerXp->setStyleSheet(statStyle);
    ui->labelAchievementCount->setStyleSheet(statStyle);
    ui->labelBestScoreGlobal->setStyleSheet(
        "color: #22c55e;"
        "font-size: 16px;"
        "font-weight: bold;"
        );

    // Buttons style
    QString blueButtonStyle =
        "QPushButton {"
        "background-color: #2563eb;"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 10px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #1d4ed8;"
        "}";

    QString darkButtonStyle =
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
        "}";

    ui->pushButtonStartChallenge->setStyleSheet(blueButtonStyle);
    ui->pushButtonLeaderboard->setStyleSheet(blueButtonStyle);
    ui->pushButtonStatistics->setStyleSheet(blueButtonStyle);
    ui->pushButtonManageQuestions->setStyleSheet(darkButtonStyle);
    ui->pushButtonLogout->setStyleSheet(darkButtonStyle);
    ui->pushButtonHistory->setStyleSheet(blueButtonStyle);
    ui->pushButtonManageUsers->setStyleSheet(darkButtonStyle);
    ui->pushButtonAchievements->setStyleSheet(blueButtonStyle);
    ui->labelAchievementList->setStyleSheet(
        "color: #facc15;"
        "font-size: 15px;"
        "font-weight: 600;"
        );
    ui->labelAchievementList->setWordWrap(true);
    ui->labelAchievementList->setAlignment(Qt::AlignCenter);


    // Hide admin-only feature
    if (currentRole != "admin") {
        ui->pushButtonManageQuestions->hide();
        ui->pushButtonManageUsers->hide();
    }


    loadDashboardStats();

    this->showMaximized();
}
void DashboardWindow::on_pushButtonManageUsers_clicked()
{
    if (currentRole != "admin") {
        QMessageBox::warning(this, "Access Denied", "Only admin can manage users.");
        return;
    }

    UserManagerWindow *manager = new UserManagerWindow(currentUsername, currentRole);
    manager->show();
    this->close();
}

DashboardWindow::~DashboardWindow()
{
    delete ui;
}

void DashboardWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (backgroundLabel) {
        backgroundLabel->setGeometry(this->rect());
    }
}

void DashboardWindow::loadDashboardStats()
{
    // Total questions
    QSqlQuery totalQuestionsQuery("SELECT COUNT(*) FROM questions");
    int totalQuestions = 0;
    if (totalQuestionsQuery.next()) {
        totalQuestions = totalQuestionsQuery.value(0).toInt();
    }
    ui->labelTotalQuestions->setText("Total Questions: " + QString::number(totalQuestions));

    // Total players
    QSqlQuery totalPlayersQuery("SELECT COUNT(*) FROM users");
    int totalPlayers = 0;
    if (totalPlayersQuery.next()) {
        totalPlayers = totalPlayersQuery.value(0).toInt();
    }
    ui->labelTotalPlayers->setText("Total Players: " + QString::number(totalPlayers));

    // My games
    QSqlQuery myGamesQuery;
    myGamesQuery.prepare("SELECT COUNT(*) FROM scores WHERE username = ?");
    myGamesQuery.addBindValue(currentUsername);

    int myGames = 0;
    if (myGamesQuery.exec() && myGamesQuery.next()) {
        myGames = myGamesQuery.value(0).toInt();
    }
    ui->labelMyGames->setText("My Games Played: " + QString::number(myGames));

    // Global best score
    QSqlQuery bestScoreQuery("SELECT MAX(score) FROM scores");
    int bestScore = 0;
    if (bestScoreQuery.next()) {
        bestScore = bestScoreQuery.value(0).toInt();
    }
    ui->labelBestScoreGlobal->setText("Global Best Score: " + QString::number(bestScore));
    DatabaseManager::ensurePlayerProgressExists(currentUsername);

    int playerLevel = DatabaseManager::getPlayerLevel(currentUsername);
    int playerXp = DatabaseManager::getPlayerXp(currentUsername);
    int achievementCount = DatabaseManager::getAchievementCount(currentUsername);

    ui->labelPlayerLevel->setText("Level: " + QString::number(playerLevel));
    ui->labelPlayerXp->setText("XP: " + QString::number(playerXp));
    ui->labelAchievementCount->setText("Achievements: " + QString::number(achievementCount));
    QStringList unlockedAchievements = DatabaseManager::getUnlockedAchievements(currentUsername);

    if (unlockedAchievements.isEmpty()) {
        ui->labelAchievementList->setText("Unlocked: None");
    } else {
        ui->labelAchievementList->setText("Unlocked:\n- " + unlockedAchievements.join("\n- "));
    }


}

void DashboardWindow::on_pushButtonLogout_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Logout",
                                  "Are you sure you want to logout?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    MainWindow *login = new MainWindow();
    login->show();
    this->close();
}
void DashboardWindow::on_pushButtonAchievements_clicked()
{
    AchievementsWindow *window = new AchievementsWindow(currentUsername, currentRole);
    window->show();
    this->close();
}


void DashboardWindow::on_pushButtonStartChallenge_clicked()
{
    StartChallengeDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString selectedDifficulty = dialog.selectedDifficulty();
    QString selectedCategory = dialog.selectedCategory();

    if (selectedDifficulty.isEmpty() || selectedCategory.isEmpty()) {
        return;
    }

    ChallengeWindow *challenge = new ChallengeWindow(currentUsername,
                                                     currentRole,
                                                     selectedDifficulty,
                                                     selectedCategory);
    challenge->show();
    this->close();
}

void DashboardWindow::on_pushButtonLeaderboard_clicked()
{
    LeaderboardWindow *leaderboard = new LeaderboardWindow(currentUsername, currentRole);
    leaderboard->show();
    this->close();
}

void DashboardWindow::on_pushButtonStatistics_clicked()
{
    StatisticsWindow *stats = new StatisticsWindow(currentUsername, currentRole);
    stats->show();
    this->close();
}

void DashboardWindow::on_pushButtonManageQuestions_clicked()
{
    if (currentRole != "admin") {
        QMessageBox::warning(this, "Access Denied", "Only admin can manage questions.");
        return;
    }

    QuestionManagerWindow *manager = new QuestionManagerWindow(currentUsername, currentRole);
    manager->show();
    this->close();
}
void DashboardWindow::on_pushButtonHistory_clicked()
{
    HistoryWindow *history = new HistoryWindow(currentUsername, currentRole);
    history->show();
    this->close();
}
