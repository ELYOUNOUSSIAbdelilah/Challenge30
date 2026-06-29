#include "leaderboardwindow.h"
#include "ui_leaderboardwindow.h"
#include "dashboardwindow.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QCoreApplication>
#include <QPixmap>
#include <QDebug>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <QColor>

LeaderboardWindow::LeaderboardWindow(const QString &username, const QString &role, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LeaderboardWindow)
    , currentUsername(username)
    , currentRole(role)
    , backgroundLabel(nullptr)
{
    ui->setupUi(this);
   this->setWindowIcon(QIcon(":/icon.ico"));
    setWindowTitle("Challenge 30 - Leaderboard");
    // Background
    backgroundLabel = new QLabel(this);
    QString imagePath = QCoreApplication::applicationDirPath() + "/../../images/background.png";
    QPixmap bg(imagePath);
    backgroundLabel->setPixmap(bg);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(this->rect());
    backgroundLabel->lower();

    ui->centralwidget->setStyleSheet("background: transparent;");

    // Main frame style
    ui->frameLeaderboard->setStyleSheet(
        "#frameLeaderboard {"
        "background-color: rgba(0, 0, 0, 95);"
        "border-radius: 18px;"
        "}"
        );

    // Title
    ui->labelTitle->setText("Leaderboard");
    ui->labelTitle->setStyleSheet(
        "color: white;"
        "font-size: 28px;"
        "font-weight: bold;"
        );

    // Table style
    ui->tableWidgetLeaderboard->setStyleSheet(
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

    ui->tableWidgetLeaderboard->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetLeaderboard->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetLeaderboard->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidgetLeaderboard->verticalHeader()->setVisible(false);
    ui->tableWidgetLeaderboard->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetLeaderboard->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetLeaderboard->setAlternatingRowColors(true);

    // Back button style
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

    loadScores();

    // Fade-in animation
    this->setWindowOpacity(0.0);

    QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setDuration(400);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    this->showMaximized();
}

LeaderboardWindow::~LeaderboardWindow()
{
    delete ui;
}

void LeaderboardWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (backgroundLabel) {
        backgroundLabel->setGeometry(this->rect());
    }
}

void LeaderboardWindow::loadScores()
{
    ui->tableWidgetLeaderboard->clearContents();
    ui->tableWidgetLeaderboard->setRowCount(0);

    ui->tableWidgetLeaderboard->setColumnCount(4);
    ui->tableWidgetLeaderboard->setHorizontalHeaderLabels({"Rank", "Username", "Score", "Date"});

    QSqlQuery query;
    query.prepare("SELECT username, score, played_at FROM scores ORDER BY score DESC, played_at DESC");

    if (!query.exec()) {
        qDebug() << "Failed to load leaderboard:" << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next()) {
        ui->tableWidgetLeaderboard->insertRow(row);

        QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(row + 1));
        QTableWidgetItem *usernameItem = new QTableWidgetItem(query.value(0).toString());
        QTableWidgetItem *scoreItem = new QTableWidgetItem(query.value(1).toString());
        QTableWidgetItem *dateItem = new QTableWidgetItem(query.value(2).toString());

        rankItem->setTextAlignment(Qt::AlignCenter);
        usernameItem->setTextAlignment(Qt::AlignCenter);
        scoreItem->setTextAlignment(Qt::AlignCenter);
        dateItem->setTextAlignment(Qt::AlignCenter);

        if (row == 0) {
            rankItem->setBackground(QColor("#facc15"));      // gold
            usernameItem->setBackground(QColor("#facc15"));
            scoreItem->setBackground(QColor("#facc15"));
            dateItem->setBackground(QColor("#facc15"));
        } else if (row == 1) {
            rankItem->setBackground(QColor("#d1d5db"));      // silver
            usernameItem->setBackground(QColor("#d1d5db"));
            scoreItem->setBackground(QColor("#d1d5db"));
            dateItem->setBackground(QColor("#d1d5db"));
        } else if (row == 2) {
            rankItem->setBackground(QColor("#b45309"));      // bronze
            usernameItem->setBackground(QColor("#b45309"));
            scoreItem->setBackground(QColor("#b45309"));
            dateItem->setBackground(QColor("#b45309"));
        }

        ui->tableWidgetLeaderboard->setItem(row, 0, rankItem);
        ui->tableWidgetLeaderboard->setItem(row, 1, usernameItem);
        ui->tableWidgetLeaderboard->setItem(row, 2, scoreItem);
        ui->tableWidgetLeaderboard->setItem(row, 3, dateItem);

        row++;
    }
}

void LeaderboardWindow::on_pushButtonBack_clicked()
{
    DashboardWindow *dashboard = new DashboardWindow(currentUsername, currentRole);
    dashboard->show();
    this->close();
}