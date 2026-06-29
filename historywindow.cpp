#include "historywindow.h"
#include "ui_historywindow.h"
#include "dashboardwindow.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDebug>
#include <QCoreApplication>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QAbstractAnimation>

HistoryWindow::HistoryWindow(const QString &username, const QString &role, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HistoryWindow)
    , currentUsername(username)
    , currentRole(role)
    , backgroundLabel(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("Challenge 30 - History");

    // Background
    backgroundLabel = new QLabel(this);
    QString imagePath = QCoreApplication::applicationDirPath() + "/../../images/background.png";
    QPixmap bg(imagePath);
    backgroundLabel->setPixmap(bg);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(this->rect());
    backgroundLabel->lower();

    ui->centralwidget->setStyleSheet("background: transparent;");

    // Main frame
    ui->frameHistory->setStyleSheet(
        "#frameHistory {"
        "background-color: rgba(0, 0, 0, 125);"
        "border-radius: 18px;"
        "}"
        );

    ui->labelTitle->setText("Game History");
    ui->labelTitle->setStyleSheet(
        "color: white;"
        "font-size: 28px;"
        "font-weight: bold;"
        );

    ui->tableWidgetHistory->setColumnCount(5);
    ui->tableWidgetHistory->setHorizontalHeaderLabels(
        QStringList() << "Date" << "Username" << "Category" << "Difficulty" << "Score"
        );

    ui->tableWidgetHistory->setStyleSheet(
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

    ui->tableWidgetHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetHistory->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidgetHistory->verticalHeader()->setVisible(false);
    ui->tableWidgetHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetHistory->setSelectionMode(QAbstractItemView::NoSelection);

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

    loadHistory();

    this->setWindowOpacity(0.0);
    QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setDuration(400);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    this->showMaximized();
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}

void HistoryWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (backgroundLabel) {
        backgroundLabel->setGeometry(this->rect());
    }
}

void HistoryWindow::loadHistory()
{
    ui->tableWidgetHistory->setRowCount(0);

    QSqlQuery query;
    query.prepare("SELECT played_at, username, category, difficulty, score "
                  "FROM scores "
                  "ORDER BY id DESC");

    if (!query.exec()) {
        qDebug() << "Failed to load history:" << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next()) {
        ui->tableWidgetHistory->insertRow(row);

        for (int col = 0; col < 5; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidgetHistory->setItem(row, col, item);
        }

        row++;
    }

    if (row == 0) {
        ui->tableWidgetHistory->setRowCount(1);
        ui->tableWidgetHistory->setSpan(0, 0, 1, 5);

        QTableWidgetItem *item = new QTableWidgetItem("No history available");
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetHistory->setItem(0, 0, item);
    }
}

void HistoryWindow::on_pushButtonBack_clicked()
{
    DashboardWindow *dashboard = new DashboardWindow(currentUsername, currentRole);
    dashboard->show();
    this->close();
}